#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include <iphlpapi.h>

#include <format>
#include <string>
#include <vector>


#define MALLOC(x) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
template<typename T1, typename T2>
constexpr auto SafeSub(T1 x, T2  y) { return ((x > y) ? x - y : 0); }

constexpr ULONG64 HUNDRED = 100;
constexpr ULONG64 KB = 1 << 10;
constexpr ULONG64 MB = 1 << 20;
constexpr ULONG64 GB = 1 << 30;

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
	template<class... Args>
	deletable_facet(Args&&... args) : Facet(std::forward<Args>(args)...) {}
	~deletable_facet() {}
};


class CNetViewUtil
{
public:
	CNetViewUtil()
		: m_pIfTable(nullptr)
		, m_dwSize(sizeof(MIB_IFTABLE))
		, m_ulTotalRecvBytes (0)
		, m_ulTotalSendBytes (0)
		, m_ulPreTime(GetTickCount64())
	{
		m_pIfTable = reinterpret_cast<PMIB_IFTABLE>(MALLOC(m_dwSize));
		if (m_pIfTable == nullptr) {
			return;
		}

		if (GetIfTable(m_pIfTable, &m_dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
			FREE(m_pIfTable);
			// realloc memory
			m_pIfTable = reinterpret_cast<PMIB_IFTABLE>(MALLOC(m_dwSize));
			if (m_pIfTable == nullptr) {
				return;
			}
		}
		StoreAdapterInfo();
	}

	virtual ~CNetViewUtil()
	{
		// destroy resources
		if (m_pIfTable != nullptr)
		{
			FREE(m_pIfTable);
			m_pIfTable = nullptr;
		}
	}

	void UpdateNetOctets(void)
	{
		ULONG64 ulTotalRecvBytes = 0;
		ULONG64 ulTotalSendBytes = 0;
		ULONG64	ulTime = GetTickCount64();	// record current time in order to caculate speed
		BOOL ret = CalculateNetOctets(ulTotalRecvBytes, ulTotalSendBytes);
		if (ret == FALSE) {
			return;
		}

		// once when bytes reach 0xFFFFFFFF, it will return to 0.
		ULONG64 curRecvBytes = SafeSub(ulTotalRecvBytes, m_ulTotalRecvBytes);		// download
		ULONG64 curSendBytes = SafeSub(ulTotalSendBytes, m_ulTotalSendBytes);      // upload
		DOUBLE dTime = (static_cast<DOUBLE>(ulTime - m_ulPreTime) / 1000.0f);
		if (m_ulTotalRecvBytes != 0) {
			m_strRecvSpeed = TransNetOctets2Wstr(curRecvBytes, dTime, L"\x2193");
		}
		if (m_ulTotalSendBytes != 0) {
			m_strSendSpeed = TransNetOctets2Wstr(curSendBytes, dTime, L"\x2191");
		}

		// save previous in/out octets
		m_ulTotalRecvBytes = ulTotalRecvBytes;
		m_ulTotalSendBytes = ulTotalSendBytes;
		m_ulPreTime = ulTime;
	}

	inline	const std::wstring GetRecvSpeed() const { return m_strRecvSpeed; }		// ·µ»ØRecvSpeed
	inline	const std::wstring GetSendSpeed() const { return m_strSendSpeed; }		// ·µ»ØSendSpeed

private:

	std::string Wstr2Str(std::wstring wstr)
	{
		std::string str;
		int ret = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), lstrlenW(wstr.data()), NULL, 0, NULL, NULL);
		if (ret <= 0) {
			return std::string();
		}

		str.resize(ret + 10);
		ret = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), lstrlenW(wstr.data()), &str[0], (int)str.size(), NULL, NULL);
		if (ret <= 0) {
			return std::string();
		}
		return str;
	}

	bool IsAdapterInuse(std::string strAdapterName)
	{
		if (strAdapterName.empty()) {
			return false;
		}

		for (const auto& i : m_vecAdapterName) {
			size_t index = strAdapterName.find(i);
			if (index != strAdapterName.length() && index >= 0) {
				return true;
			}
		}
		return false;
	}

	void StoreAdapterInfo(void)
	{
		DWORD size = sizeof(IP_ADAPTER_ADDRESSES);
		PIP_ADAPTER_ADDRESSES pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(MALLOC(size));
		if (pAddresses == nullptr) {
			return;
		}

		if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &size) == ERROR_BUFFER_OVERFLOW) {
			FREE(pAddresses);
			pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(MALLOC(size));
			if (pAddresses == nullptr) {
				return;
			}
		}

		if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &size) == NO_ERROR) {
			for (PIP_ADAPTER_ADDRESSES pCurrentAddresses = pAddresses;
				pCurrentAddresses != nullptr;
				pCurrentAddresses = pCurrentAddresses->Next) {
				m_vecAdapterName.emplace_back(pCurrentAddresses->AdapterName);
			}
		}

		FREE(pAddresses);
	}

	BOOL CalculateNetOctets(ULONG64& ulTotalRecvBytes, ULONG64& ulTotalSendBytes)
	{
		if (m_pIfTable == nullptr) {
			return FALSE;
		}

		if (GetIfTable(m_pIfTable, &m_dwSize, FALSE) != NO_ERROR) {
			return FALSE;
		}
		// Caculate all octets 
		for (DWORD i = 0; i < m_pIfTable->dwNumEntries; i++) {
			PMIB_IFROW pIfRow = &m_pIfTable->table[i];

			// transform wchar to char
			if (IsAdapterInuse(Wstr2Str(pIfRow->wszName))) {
				ulTotalRecvBytes += pIfRow->dwInOctets;
				ulTotalSendBytes += pIfRow->dwOutOctets;
			}
		}

		return TRUE;
	}

	std::wstring TransNetOctets2Wstr(ULONG64 traffic, DOUBLE dTime, const std::wstring& arrow)
	{
		std::wstring ret;
		ULONG64 trafficPerSec = traffic * HUNDRED / (ULONG64)dTime / HUNDRED;
		if (trafficPerSec < KB) {
			ret = arrow + std::to_wstring(trafficPerSec) + L" B/s";
		} else if (traffic < MB) {
			ret = arrow.data() + std::to_wstring(trafficPerSec / KB) + L" KB/s";
		} else if (traffic < GB) {
			ret = arrow.data() + std::to_wstring(trafficPerSec / MB) + L" MB/s";
		} else {
			ret = //std::format(L"{} {:.2f} GB/s", arrow.data(), 1.0 * traffic / dTime / GB);
				arrow.data() + std::to_wstring(trafficPerSec / GB) + L" GB/s";
		}

		return ret;
	}

	PMIB_IFTABLE	m_pIfTable;
	DWORD			m_dwSize;
	ULONG64			m_ulTotalRecvBytes;		// previous recv the number of octets of data 
	ULONG64			m_ulTotalSendBytes;		// previous send ... 
	ULONG64			m_ulPreTime;			// previous time
	std::wstring	m_strRecvSpeed;
	std::wstring	m_strSendSpeed;
	std::vector<std::string> m_vecAdapterName;
};
