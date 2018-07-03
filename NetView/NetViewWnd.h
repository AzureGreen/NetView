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

	VOID UpdateTextHeight(void);

	BEGIN_MSG_MAP(CNetViewWnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		//MESSAGE_HANDLER(WM_SIZING, OnSizing)
		//MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

private:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	//LRESULT OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	//LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);

private:
	LPUNKNOWN	m_lpDeskBand;
	BOOL		m_bHasFocus;
	//BOOL		m_bWndSizeChanged;
	int			m_iHeight;
	CComQIPtr<IInputObjectSite> m_spios;

	CNetViewUtil m_NetViewUtil;

};

