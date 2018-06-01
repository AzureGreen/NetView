#include "stdafx.h"
#include "NetViewWnd.h"


#define UPDATE_TIMER_ID 1

#define RECTWIDTH(x)   ((x).right - (x).left)
#define RECTHEIGHT(x)  ((x).bottom - (x).top)

CNetViewWnd::CNetViewWnd()
	: m_lpDeskBand(NULL)
	, m_fHasFocus(FALSE)
{
}


CNetViewWnd::~CNetViewWnd()
{
}

BOOL CNetViewWnd::Create(HWND hWndParent, LPUNKNOWN lpDeskBand, LPUNKNOWN pios)//, LPUNKNOWN InputObjectSite)
{
	if (!__super::Create(hWndParent))
	{
		return FALSE;
	}

	m_lpDeskBand = lpDeskBand;
	m_spios = pios;

	return 0;
}

POINTL CNetViewWnd::CalcWindowSize(void)
{
	POINTL ptWndSize = { 0 };
	if (!IsWindow())
	{
		ptWndSize.x = GetSystemMetrics(SM_CXMIN);
		ptWndSize.y = GetSystemMetrics(SM_CYMIN);
	}
	else
	{
		ptWndSize.x = 80;
		ptWndSize.y = 36;
	}
	return ptWndSize;
}

BOOL CNetViewWnd::HasFocus(void)
{
	return m_fHasFocus;
}

LRESULT CNetViewWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
{
	// set the timer
	SetTimer(UPDATE_TIMER_ID, 1000);

	// set font style
	//HFONT hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
	//SetFont(hFont, TRUE);
	return LRESULT();
}

LRESULT CNetViewWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
{
	PAINTSTRUCT ps = { 0 };
	HDC hdc = BeginPaint(&ps);	//  prepares the specified window for painting &  fills a PAINTSTRUCT structure with information about the painting.

	std::wstring strRecv = m_NetViewUtil.GetRecvSpeed();
	std::wstring strSend = m_NetViewUtil.GetSendSpeed();

	if (hdc)
	{
		RECT rc = { 0 };
		GetClientRect(&rc);

		HTHEME hTheme = ::OpenThemeData(NULL, L"TextStyle");
		if (hTheme)
		{
			HDC hdcPaint = NULL;
			HPAINTBUFFER hBufferedPaint = ::BeginBufferedPaint(hdc, &rc, BPBF_TOPDOWNDIB, NULL, &hdcPaint);
			
			// before other operations, draw the background
			::DrawThemeParentBackground(m_hWnd, hdcPaint, &rc);
			
			// set font
			HFONT hFont = ::CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, 
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
			HFONT hOldFont = static_cast<HFONT>(::SelectObject(hdcPaint, hFont));

			// calculate the size of the string, 'cx' represents width while 'cy' is height 
			SIZE sRecv = { 0 };
			SIZE sSend = { 0 };
			::GetTextExtentPointW(hdc, strRecv.c_str(), static_cast<int>(strRecv.length()), &sRecv);
			::GetTextExtentPointW(hdc, strSend.c_str(), static_cast<int>(strSend.length()), &sSend);

			// prepare rects of recv & send text
			RECT rcRecv = { 0 };
			RECT rcSend = { 0 };
			rcRecv.left = (RECTWIDTH(rc) - sRecv.cx) / 2;
			rcRecv.right = rcRecv.left + sRecv.cx;
			rcRecv.bottom = RECTHEIGHT(rc) - (RECTHEIGHT(rc) / 2 - sRecv.cy) / 2;
			rcRecv.top = rcRecv.bottom - sRecv.cy;
			
			rcSend.left = (RECTWIDTH(rc) - sSend.cx) / 2;
			rcSend.right = rcSend.left + sSend.cx;
			rcSend.top = (RECTHEIGHT(rc) / 2 - sSend.cy) / 2;
			rcSend.bottom = rcSend.top + sSend.cy;


			DTTOPTS dttOpts = { sizeof(dttOpts) };
			dttOpts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_GLOWSIZE;
			dttOpts.crText = RGB(242, 242, 242);
			dttOpts.iGlowSize = 8;
			::DrawThemeTextEx(hTheme, hdcPaint, 0, 0, strRecv.c_str(), -1, 0, &rcRecv, &dttOpts);
			::DrawThemeTextEx(hTheme, hdcPaint, 0, 0, strSend.c_str(), -1, 0, &rcSend, &dttOpts);
			
			::SelectObject(hdcPaint, hOldFont);
			::EndBufferedPaint(hBufferedPaint, TRUE);
			::CloseThemeData(hTheme);
		}
	}
	EndPaint(&ps);	// marks the end of painting in the specified window
	return LRESULT(0);
}

LRESULT CNetViewWnd::OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
{
	LRESULT hr = S_FALSE;
	m_fHasFocus = (uMsg == WM_SETFOCUS);
	if (m_spios)
	{
		hr = m_spios->OnFocusChangeIS(m_lpDeskBand, m_fHasFocus);	// Informs the browser that the focus has changed.
	}
	return hr;
}

LRESULT CNetViewWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
{
	// caculate current net octets
	m_NetViewUtil.CalculateNetOctets();

	Invalidate();
	UpdateWindow();

	return LRESULT(0);
}
