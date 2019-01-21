#include "stdafx.h"
#include "NetViewWnd.h"


#define UPDATE_TIMER_ID 1

#define RECTWIDTH(x)   ((x).right - (x).left)
#define RECTHEIGHT(x)  ((x).bottom - (x).top)

CNetViewWnd::CNetViewWnd()
	: m_lpDeskBand(NULL)
	, m_bHasFocus(FALSE)
	//, m_bWndSizeChanged(FALSE)
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
		UpdateTextHeight();
		ptWndSize.x = 96;
		ptWndSize.y = 24;
	}
	return ptWndSize;
}

BOOL CNetViewWnd::HasFocus(void)
{
	return m_bHasFocus;
}

VOID CNetViewWnd::UpdateTextHeight(void)
{
	RECT rect = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	int y = GetSystemMetrics(SM_CYSCREEN);
	m_iHeight = (y - rect.bottom) * 0.45;
	//m_bWndSizeChanged = FALSE;
}

LRESULT CNetViewWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
{
	// set the timer
	SetTimer(UPDATE_TIMER_ID, 1000);

	return LRESULT();
}

LRESULT CNetViewWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
{
	PAINTSTRUCT ps = { 0 };
	HDC hdc = BeginPaint(&ps);	//  prepares the specified window for painting &  fills a PAINTSTRUCT structure with information about the painting.

	int x = ps.rcPaint.left;

	std::wstring strRecv = m_NetViewUtil.GetRecvSpeed();
	std::wstring strSend = m_NetViewUtil.GetSendSpeed();

	if (hdc)
	{
		RECT rc = { 0 };
		GetClientRect(&rc);

		HTHEME hTheme = ::OpenThemeData(NULL, L"TextStyle");

		if (hTheme)
		{
			HDC hMemDC = ::CreateCompatibleDC(hdc);
			HBITMAP hMemBitMap = ::CreateCompatibleBitmap(hdc, RECTWIDTH(rc), RECTHEIGHT(rc));
			HBITMAP hOldMemBitMap = static_cast<HBITMAP>(::SelectObject(hMemDC, hMemBitMap));

			// before other operations, draw the background
			//::DrawThemeParentBackground(m_hWnd, hMemDC, &rc);

			// set font
			HFONT hFont = ::CreateFontW(m_iHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"@system");
			HFONT hOldFont = static_cast<HFONT>(::SelectObject(hMemDC, hFont));

			// calculate the size of the string, 'cx' represents width while 'cy' is height 
			SIZE sRecv = { 0 };
			SIZE sSend = { 0 };
			::GetTextExtentPoint32W(hMemDC, strRecv.c_str(), static_cast<int>(strRecv.length()), &sRecv);
			::GetTextExtentPoint32W(hMemDC, strSend.c_str(), static_cast<int>(strSend.length()), &sSend);

			// prepare rects of recv & send text
			RECT rcRecv = { 0 };
			RECT rcSend = { 0 };
			rcRecv.left = (RECTWIDTH(rc) - sRecv.cx) / 2;
			rcRecv.right = rcRecv.left + sRecv.cx;
			rcRecv.top = RECTHEIGHT(rc) / 2 + (RECTHEIGHT(rc) / 2 - sRecv.cy) / 2;
			rcRecv.bottom = rcRecv.top + sRecv.cy;

			rcSend.left = (RECTWIDTH(rc) - sSend.cx) / 2;
			rcSend.right = rcSend.left + sSend.cx;
			rcSend.bottom = RECTHEIGHT(rc) / 2 - (RECTHEIGHT(rc) / 2 - sSend.cy) / 2;
			rcSend.top = rcSend.bottom - sSend.cy;

			DTTOPTS dttOpts = { sizeof(dttOpts) };
			dttOpts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_GLOWSIZE;
			dttOpts.crText = RGB(250, 250, 250);
			dttOpts.iGlowSize = 8;
			::DrawThemeTextEx(hTheme, hMemDC, 0, 0, strRecv.c_str(), -1, 0, &rcRecv, &dttOpts);
			::DrawThemeTextEx(hTheme, hMemDC, 0, 0, strSend.c_str(), -1, 0, &rcSend, &dttOpts);

			//::BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, RECTWIDTH(ps.rcPaint), RECTHEIGHT(ps.rcPaint), hMemDC, 0, 0, SRCCOPY);
			::BitBlt(hdc, 0, 0, RECTWIDTH(rc), RECTHEIGHT(rc), hMemDC, 0, 0, SRCCOPY);

			//::SelectObject(hMemDC, hOldFont);
			::SelectObject(hMemDC, hOldMemBitMap);
			::DeleteObject(hMemBitMap);
			::DeleteDC(hMemDC);
			
			::CloseThemeData(hTheme);
		}
	}

	EndPaint(&ps);	// marks the end of painting in the specified window
	return LRESULT(0);
}

//LRESULT CNetViewWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
//{
//	PAINTSTRUCT ps = { 0 };
//	HDC hdc = BeginPaint(&ps);	//  prepares the specified window for painting &  fills a PAINTSTRUCT structure with information about the painting.
//
//	std::wstring strRecv = m_NetViewUtil.GetRecvSpeed();
//	std::wstring strSend = m_NetViewUtil.GetSendSpeed();
//
//	if (hdc)
//	{
//		RECT rc = { 0 };
//		GetClientRect(&rc);
//
//		HTHEME hTheme = ::OpenThemeData(NULL, L"TextStyle");
//		if (hTheme)
//		{
//			HDC hdcPaint = NULL;
//			HPAINTBUFFER hBufferedPaint = ::BeginBufferedPaint(hdc, &rc, BPBF_TOPDOWNDIB, NULL, &hdcPaint);
//
//			// before other operations, draw the background
//			::DrawThemeParentBackground(m_hWnd, hdcPaint, &rc);
//
//			// set font
//			HFONT hFont = ::CreateFontW(m_iHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
//				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"@system");
//			HFONT hOldFont = static_cast<HFONT>(::SelectObject(hdcPaint, hFont));
//
//			// calculate the size of the string, 'cx' represents width while 'cy' is height 
//			SIZE sRecv = { 0 };
//			SIZE sSend = { 0 };
//			::GetTextExtentPoint32W(hdcPaint, strRecv.c_str(), static_cast<int>(strRecv.length()), &sRecv);
//			::GetTextExtentPoint32W(hdcPaint, strSend.c_str(), static_cast<int>(strSend.length()), &sSend);
//
//			// prepare rects of recv & send text
//			RECT rcRecv = { 0 };
//			RECT rcSend = { 0 };
//			rcRecv.left = (RECTWIDTH(rc) - sRecv.cx) / 2;
//			rcRecv.right = rcRecv.left + sRecv.cx;
//			rcRecv.top = RECTHEIGHT(rc) / 2 + (RECTHEIGHT(rc) / 2 - sRecv.cy) / 2;
//			rcRecv.bottom = rcRecv.top + sRecv.cy;
//
//			rcSend.left = (RECTWIDTH(rc) - sSend.cx) / 2;
//			rcSend.right = rcSend.left + sSend.cx;
//			rcSend.bottom = RECTHEIGHT(rc) / 2 - (RECTHEIGHT(rc) / 2 - sSend.cy) / 2;
//			rcSend.top = rcSend.bottom - sSend.cy;
//
//
//
//			DTTOPTS dttOpts = { sizeof(dttOpts) };
//			dttOpts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_GLOWSIZE;
//			dttOpts.crText = RGB(250, 250, 250);
//			dttOpts.iGlowSize = 8;
//			::DrawThemeTextEx(hTheme, hdcPaint, 0, 0, strRecv.c_str(), -1, 0, &rcRecv, &dttOpts);
//			::DrawThemeTextEx(hTheme, hdcPaint, 0, 0, strSend.c_str(), -1, 0, &rcSend, &dttOpts);
//
//			//::SelectObject(hdcPaint, hOldFont);
//			::EndBufferedPaint(hBufferedPaint, TRUE);
//			::CloseThemeData(hTheme);
//		}
//	}
//
//	EndPaint(&ps);	// marks the end of painting in the specified window
//	return LRESULT(0);
//}

//LRESULT CNetViewWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
//{
//	PAINTSTRUCT ps = { 0 };
//	HDC hdc = BeginPaint(&ps);	//  prepares the specified window for painting &  fills a PAINTSTRUCT structure with information about the painting.
//
//	std::wstring strRecv = m_NetViewUtil.GetRecvSpeed();
//	std::wstring strSend = m_NetViewUtil.GetSendSpeed();
//
//	if (hdc)
//	{
//		RECT rc = { 0 };
//		GetClientRect(&rc);
//
//		HDC hdcPaint = NULL;
//		HPAINTBUFFER hBufferedPaint = ::BeginBufferedPaint(hdc, &rc, BPBF_TOPDOWNDIB, NULL, &hdcPaint);
//
//		// before other operations, draw the background
//		//::DrawThemeParentBackground(m_hWnd, hdcPaint, &rc);
//
//		// set font
//		LOGFONT logFont = { 0 };
//		logFont.lfCharSet = ANSI_CHARSET;
//		//logFont.lfHeight = -MulDiv(9, GetDeviceCaps(hdcPaint, LOGPIXELSY), 72);
//		logFont.lfHeight = 16;
//
//		HFONT hFont = CreateFontIndirect(&logFont);
//
//		::SetTextColor(hdcPaint, RGB(255, 255, 255));
//		::SetBkColor(hdcPaint, RGB(0, 0, 0));
//		::SetBkMode(hdcPaint, TRANSPARENT);
//
//		HFONT hOldFont = static_cast<HFONT>(::SelectObject(hdcPaint, hFont));
//
//		// calculate the size of the string, 'cx' represents width while 'cy' is height 
//		SIZE sRecv = { 0 };
//		SIZE sSend = { 0 };
//		::GetTextExtentPoint32W(hdcPaint, strRecv.c_str(), static_cast<int>(strRecv.length()), &sRecv);
//		::GetTextExtentPoint32W(hdcPaint, strSend.c_str(), static_cast<int>(strSend.length()), &sSend);
//
//		// prepare rects of recv & send text
//		RECT rcRecv = { 0 };
//		RECT rcSend = { 0 };
//		rcRecv.left = (RECTWIDTH(rc) - sRecv.cx) / 2;
//		rcRecv.right = rcRecv.left + sRecv.cx;
//		rcRecv.bottom = RECTHEIGHT(rc) - (RECTHEIGHT(rc) / 2 - sRecv.cy) / 2;
//		rcRecv.top = rcRecv.bottom - sRecv.cy;
//
//		rcSend.left = (RECTWIDTH(rc) - sSend.cx) / 2;
//		rcSend.right = rcSend.left + sSend.cx;
//		rcSend.top = (RECTHEIGHT(rc) / 2 - sSend.cy) / 2;
//		rcSend.bottom = rcSend.top + sSend.cy;
//
//		::DrawTextEx(hdcPaint, const_cast<LPWSTR>(strRecv.c_str()), strRecv.length(), &rcRecv, DT_VCENTER | DT_CENTER, NULL);
//		::DrawTextEx(hdcPaint, const_cast<LPWSTR>(strSend.c_str()), strSend.length(), &rcSend, DT_VCENTER | DT_CENTER, NULL);
//
//		//::SelectObject(hdcPaint, hOldFont);
//		::EndBufferedPaint(hBufferedPaint, TRUE);
//
//	}
//
//	EndPaint(&ps);	// marks the end of painting in the specified window
//	return LRESULT(0);
//}

//LRESULT CNetViewWnd::OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
//{
//	RECT rect = { 0 };
//	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
//	int y = GetSystemMetrics(SM_CYSCREEN);
//	m_iHeight = (y - rect.bottom) * 0.4;
//	::MessageBoxA(NULL, const_cast<LPSTR>((std::to_string(m_iHeight)).c_str()), "SIZING", NULL);
//	return LRESULT();
//}
//
//LRESULT CNetViewWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
//{
//	RECT rect = { 0 };
//	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
//	int y = GetSystemMetrics(SM_CYSCREEN);
//	m_iHeight = (y - rect.bottom) * 0.4;
//	::MessageBoxA(NULL, const_cast<LPSTR>((std::to_string(m_iHeight)).c_str()), "GETMINMAXINFO", NULL);
//	return LRESULT();
//}

LRESULT CNetViewWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
{
	//m_bWndSizeChanged = TRUE;
	return LRESULT();
}

LRESULT CNetViewWnd::OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
{
	LRESULT hr = S_FALSE;
	m_bHasFocus = (uMsg == WM_SETFOCUS);
	if (m_spios)
	{
		hr = m_spios->OnFocusChangeIS(m_lpDeskBand, m_bHasFocus);	// Informs the browser that the focus has changed.
	}
	return hr;
}

LRESULT CNetViewWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandle)
{
	// caculate current net octets
	m_NetViewUtil.CalculateNetOctets();

	UpdateTextHeight();

	// update value shown in the window
	Invalidate();
	UpdateWindow();

	return LRESULT(0);
}
