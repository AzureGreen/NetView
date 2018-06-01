// NetViewDeskBand.cpp : CNetViewDeskBand 的实现

#include "stdafx.h"
#include "NetViewDeskBand.h"


// CNetViewDeskBand

// IObjectWithSite
// step 2: Create DeskBand Window here
STDMETHODIMP CNetViewDeskBand::SetSite(IUnknown * pUnkSite)
{
	// Create DeskBand Window here

	HRESULT hr = __super::SetSite(pUnkSite);
	if (SUCCEEDED(hr) && pUnkSite)
	{
		/*CComQIPtr<IOleWindow> spOleWindow = pUnkSite;*/
		CComQIPtr<IOleWindow> spOleWindow(pUnkSite);
		if (spOleWindow)
		{
			HWND hWndParent = NULL;
			hr = spOleWindow->GetWindow(&hWndParent);	// retrieve the main window handle of an OLE site.

			if (SUCCEEDED(hr))
			{
				// Create window
				m_NetViewWnd.Create(hWndParent, static_cast<IDeskBand *>(this), pUnkSite);
				if (!m_NetViewWnd.IsWindow())
				{
					hr = E_FAIL;
				}
			}
		}
	}
	return hr;
}

// IOleWindow
// step 3: get DeskBand window handle
STDMETHODIMP CNetViewDeskBand::GetWindow(HWND * phwnd)
{
	if (phwnd)
	{
		*phwnd = HWND(m_NetViewWnd);
	}
	return S_OK;
}

STDMETHODIMP CNetViewDeskBand::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

// IDockingWindow
//
// step 5: show DeskBand window
STDMETHODIMP CNetViewDeskBand::ShowDW(BOOL fShow)
{
	if (m_NetViewWnd)
	{
		m_NetViewWnd.ShowWindow(fShow ? SW_SHOW : SW_HIDE);
	}
	return S_OK;
}

STDMETHODIMP CNetViewDeskBand::CloseDW(DWORD dwReserved)
{
	if (m_NetViewWnd)
	{
		m_NetViewWnd.ShowWindow(SW_HIDE);
		m_NetViewWnd.DestroyWindow();
	}
	return S_OK;
}

STDMETHODIMP CNetViewDeskBand::ResizeBorderDW(LPCRECT prcBorder, IUnknown * punkToolbarSite, BOOL fReserved)
{
	return E_NOTIMPL;
}

// IDeskBand
//
// step 4: Gets state information for a band object.
STDMETHODIMP CNetViewDeskBand::GetBandInfo(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO * pdbi)
{
	//ATLTRACE(atlTraceCOM, 2, _T("IDeskBand::GetBandInfo\r\n"));

	HRESULT hr = E_INVALIDARG;
	if (pdbi)
	{
		m_dwBandID = dwBandID;

		if (pdbi->dwMask & DBIM_MINSIZE)
		{
			pdbi->ptMinSize = m_NetViewWnd.CalcWindowSize();
			//pdbi->ptMinSize.x = 200;
			//pdbi->ptMinSize.y = 30;
		}
		if (pdbi->dwMask & DBIM_MAXSIZE)
		{
			pdbi->ptMaxSize.y = -1;	// no limit for maximum height
		}
		if (pdbi->dwMask & DBIM_INTEGRAL)
		{
			pdbi->ptIntegral.y = 1;
		}
		if (pdbi->dwMask & DBIM_ACTUAL)
		{
			pdbi->ptActual = m_NetViewWnd.CalcWindowSize();
			//pdbi->ptMinSize.x = 200;
			//pdbi->ptMinSize.y = 30;
		}
		if (pdbi->dwMask & DBIM_TITLE)
		{
			//if (dwViewMode == DBIF_VIEWMODE_FLOATING)
			//{
			//	StringCchCopy(pdbi->wszTitle, lstrlen(L"NetView") + 1, L"NetView");
			//}
			//else
			//{
			pdbi->dwMask &= ~DBIM_TITLE;	// do not show the title
											//}
		}
		if (pdbi->dwMask & DBIM_MODEFLAGS)
		{
			pdbi->dwModeFlags = DBIMF_NORMAL | DBIMF_VARIABLEHEIGHT;
		}
		if (pdbi->dwMask & DBIM_BKCOLOR)
		{
			pdbi->dwMask &= ~DBIM_BKCOLOR;	// use default background color, ignore crBkgnd
		}
		hr = S_OK;
	}
	return hr;
}

// IDeskBand2
//
// Indicates the deskband's ability to be displayed as translucent.
//
STDMETHODIMP CNetViewDeskBand::CanRenderComposited(BOOL * pfCanRenderComposited)
{
	if (pfCanRenderComposited)
	{
		*pfCanRenderComposited = TRUE;
	}
	return S_OK;
}
// Sets the composition state
//
STDMETHODIMP CNetViewDeskBand::SetCompositionState(BOOL fCompositionEnabled)
{
	m_fCompositionEnabled = fCompositionEnabled;
	// updatewindow 一下
	m_NetViewWnd.Invalidate();
	m_NetViewWnd.UpdateWindow();
	return S_OK;
}
// Gets the composition state
//
STDMETHODIMP CNetViewDeskBand::GetCompositionState(BOOL * pfCompositionEnabled)
{
	if (pfCompositionEnabled)
	{
		*pfCompositionEnabled = m_fCompositionEnabled;
	}
	return S_OK;
}

// IInputObject
//
STDMETHODIMP CNetViewDeskBand::UIActivateIO(BOOL fActivate, MSG * pMsg)
{
	if (fActivate)
	{
		m_NetViewWnd.SetFocus();
	}
	return S_OK;
}

STDMETHODIMP CNetViewDeskBand::HasFocusIO(void)
{
	return m_NetViewWnd.HasFocus() ? S_OK : S_FALSE;
}

STDMETHODIMP CNetViewDeskBand::TranslateAcceleratorIO(MSG * pMsg)
{
	return S_FALSE;
}
