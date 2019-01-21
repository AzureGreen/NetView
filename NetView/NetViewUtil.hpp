#pragma once
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <string>
#include <vector>
#include <codecvt>

#pragma comment(lib, "IPHlpApi.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

class CNetViewUtil
{
public:
	CNetViewUtil()
		: m_bFirst(TRUE)
		, m_pIfTable(nullptr)
		, m_dwPreInOctets(0)
		, m_dwPreOutOctets(0)
		, m_dwPreTime(GetCurrentTime())
	{
		m_dwSize = sizeof(MIB_IFTABLE);

		m_pIfTable = reinterpret_cast<PMIB_IFTABLE>(MALLOC(m_dwSize));
		if (m_pIfTable == nullptr)
		{
			return;
		}
		if (GetIfTable(m_pIfTable, &m_dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER)
		{
			FREE(m_pIfTable);
			// realloc memory
			m_pIfTable = reinterpret_cast<PMIB_IFTABLE>(MALLOC(m_dwSize));
			if (m_pIfTable == nullptr)
			{
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

	void StoreAdapterInfo(void)
	{
		DWORD size = sizeof(IP_ADAPTER_ADDRESSES);
		PIP_ADAPTER_ADDRESSES pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(MALLOC(size));
		if (pAddresses == nullptr)
		{
			return;
		}

		if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &size) == ERROR_BUFFER_OVERFLOW)
		{
			FREE(pAddresses);
			pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(MALLOC(size));
			if (pAddresses == nullptr)
			{
				return;
			}
		}

		if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &size) == NO_ERROR)
		{
			for (PIP_ADAPTER_ADDRESSES pCurrentAddresses = pAddresses; pCurrentAddresses != nullptr; pCurrentAddresses = pCurrentAddresses->Next)
			{
				m_vecAdapterName.push_back(pCurrentAddresses->AdapterName);
			}
		}

		FREE(pAddresses);
	}

	std::string wstr2str(std::wstring wstr)
	{
		// consider the my system's GBK
		typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> cvt_facet;
		std::wstring_convert<cvt_facet> conv(new cvt_facet(".936"));
		std::string str = conv.to_bytes(wstr);
		return (!str.empty()) ? str : std::string();
	}

	bool IsAdapterInuse(std::string strAdapterName)
	{
		if (strAdapterName.empty()) return false;

		for (auto i : m_vecAdapterName)
		{
			int index = strAdapterName.find(i);
			if (index != strAdapterName.length() && index >= 0) return true;
		}
		return false;
	}

	void CalculateNetOctets(void)
	{
		if (m_pIfTable == nullptr) return;

		DWORD	dwInOctets = 0;			// total received 
		DWORD	dwOutOctets = 0;		// total sent
		DOUBLE	CurrentInOctets = 0;	// this time received
		DOUBLE	CurrentOutOctets = 0;	// this time sent
		DWORD	dwTime = GetCurrentTime();	// record current time in order to caculate speed
		if (GetIfTable(m_pIfTable, &m_dwSize, FALSE) != NO_ERROR)
		{
			return;
		}
		// Caculate all octets 
		for (DWORD i = 0; i < m_pIfTable->dwNumEntries; i++)
		{
			PMIB_IFROW pIfRow = &m_pIfTable->table[i];

			// transform wchar to char
			if (IsAdapterInuse(wstr2str(pIfRow->wszName)))
			{
				dwInOctets += pIfRow->dwInOctets;
				dwOutOctets += pIfRow->dwOutOctets;
			}
		}

		// once when InOctects reach 0xFFFFFFFF, it will return to 0.

		CurrentInOctets = static_cast<DOUBLE>(m_dwPreInOctets < 0 ? 0 : dwInOctets - m_dwPreInOctets);	// download
		CurrentOutOctets = static_cast<DOUBLE>(m_dwPreOutOctets < 0 ? 0 : dwOutOctets - m_dwPreOutOctets); // upload
		
		if (m_bFirst)
		{
			m_strRecvSpeed = L"¡ý 0 B/s";
			m_strSendSpeed = L"¡ü 0 B/s";
			m_bFirst = FALSE;
		}
		else
		{
			DOUBLE dTime = (static_cast<DOUBLE>(dwTime - m_dwPreTime) / 1000.0f);
			// transform unit for recv(download speed)
			if (CurrentInOctets / 1000 < 1)		// B/s
			{
				m_strRecvSpeed = std::_Floating_to_wstring(L"¡ý %.0lf B/s", CurrentInOctets / dTime);
			}
			else if (CurrentInOctets / (1000 * 1000) < 1)	// KB/s
			{
				m_strRecvSpeed = std::_Floating_to_wstring(L"¡ý %.0lf KB/s", (CurrentInOctets / 1000) / dTime);
			}
			else  // MB/s			generally, net speed will be smaller than GB/s, so I won't consider more than MB/s.
			{
				m_strRecvSpeed = std::_Floating_to_wstring(L"¡ý %.1lf MB/s", (CurrentInOctets / (1000 * 1000)) / dTime);
			}
			// transform unit for send(upload speed)
			if (CurrentOutOctets / 1000 < 1)		// B/s
			{
				m_strSendSpeed = std::_Floating_to_wstring(L"¡ü %.0lf B/s", CurrentOutOctets / dTime);
			}
			else if (CurrentOutOctets / (1000 * 1000) < 1)	// KB/s
			{
				m_strSendSpeed = std::_Floating_to_wstring(L"¡ü %.0lf KB/s", (CurrentOutOctets / 1000) / dTime);
			}
			else  // MB/s			generally, net speed will be smaller than GB/s, so I won't consider more than MB/s.
			{
				m_strSendSpeed = std::_Floating_to_wstring(L"¡ü %.1lf MB/s", (CurrentOutOctets / (1000 * 1000)) / dTime);
			}
		}

		// save previous in/out octets
		m_dwPreInOctets = dwInOctets;
		m_dwPreOutOctets = dwOutOctets;
		m_dwPreTime = dwTime;
	}

	inline	std::wstring GetRecvSpeed() { return m_strRecvSpeed; }		// ·µ»ØRecvSpeed
	inline	std::wstring GetSendSpeed() { return m_strSendSpeed; }		// ·µ»ØSendSpeed

private:
	BOOL			m_bFirst;
	PMIB_IFTABLE	m_pIfTable;
	DWORD			m_dwSize;
	DWORD			m_dwPreInOctets;		// previous received the number of octets of data 
	DWORD			m_dwPreOutOctets;		// previous sent ... 
	DWORD			m_dwPreTime;			// previous time
	std::wstring	m_strRecvSpeed;
	std::wstring	m_strSendSpeed;

	std::vector<std::string> m_vecAdapterName;
};
