#pragma once
//#include "atlwin.h"
#include <ShObjIdl.h>
#include <Uxtheme.h>
#include "NetViewUtil.hpp"
#pragma comment(lib, "Uxtheme.lib")

using namespace ATL;

class CNetViewWnd :
	public CWindowImpl<CNetViewWnd>
{
public:
	CNetViewWnd();
	~CNetViewWnd();

	BOOL Create(HWND hWndParent, LPUNKNOWN lpDeskBand, LPUNKNOWN pios);// , LPUNKNOWN InputObjectSite);

	POINTL CalcWindowSize(void);

	BOOL HasFocus(void);

	BEGIN_MSG_MAP(CNetViewWnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

private:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);

private:
	LPUNKNOWN	m_lpDeskBand;
	BOOL		m_fHasFocus;

	CComQIPtr<IInputObjectSite> m_spios;

	CNetViewUtil m_NetViewUtil;

};

