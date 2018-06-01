#pragma once
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <string>

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

	void CalculateNetOctets(void)
	{
		if (m_pIfTable == nullptr)
		{
			return;
		}


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
			/*if (pIfRow->dwType != MIB_IF_TYPE_LOOPBACK)*/
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
			m_bFirst = FALSE;
		}
		else
		{
			DOUBLE dTime = (static_cast<DOUBLE>(dwTime - m_dwPreTime) / 1000.0f);
			// transform unit for recv(download speed)
			if (CurrentInOctets / 1024 < 1)		// byte/s
			{
				m_strRecvSpeed = std::_Floating_to_wstring(L"¡ý %.2lfb/s", CurrentInOctets / dTime);
			}
			else if (CurrentInOctets / (1024 * 1024) < 1)	// kb/s
			{
				m_strRecvSpeed = std::_Floating_to_wstring(L"¡ý %.2lfkb/s", (CurrentInOctets / 1024) / dTime);
			}
			else  // mb/s			generally, net speed will be smaller than gb/s, so I won't consider more than mb/s.
			{
				m_strRecvSpeed = std::_Floating_to_wstring(L"¡ý %.2lfmb/s", (CurrentInOctets / pow(1024, 2)) / dTime);
			}
			// transform unit for send(upload speed)
			if (CurrentOutOctets / 1024 < 1)		// byte/s
			{
				m_strSendSpeed = std::_Floating_to_wstring(L"¡ü %.2lfb/s", CurrentOutOctets / dTime);
			}
			else if (CurrentOutOctets / (1024 * 1024) < 1)	// kb/s
			{
				m_strSendSpeed = std::_Floating_to_wstring(L"¡ü %.2lfkb/s", (CurrentOutOctets / 1024) / dTime);
			}
			else  // mb/s			generally, net speed will be smaller than gb/s, so I won't consider more than mb/s.
			{
				m_strSendSpeed = std::_Floating_to_wstring(L"¡ü %.2lfmb/s", (CurrentOutOctets / pow(1024, 2)) / dTime);
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
};
