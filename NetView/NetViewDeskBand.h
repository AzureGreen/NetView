// NetViewDeskBand.h : CNetViewDeskBand 的声明

#pragma once
#include "resource.h"       // 主符号

#include <Shobjidl.h>
#include <ShlObj.h>
#include "NetViewWnd.h"
#include "NetView_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;


// CNetViewDeskBand

class ATL_NO_VTABLE CNetViewDeskBand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNetViewDeskBand, &CLSID_NetViewDeskBand>,
	public IDispatchImpl<INetViewDeskBand, &IID_INetViewDeskBand, &LIBID_NetViewLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IObjectWithSiteImpl<CNetViewDeskBand>,	// Provides a simple way to support communication between an object and its site in the container
	public IPersistStreamInitImpl<CNetViewDeskBand>,
	public IInputObject,	// 输入对象
	public IDeskBand2		// 继承自DeskBand
{
	typedef IPersistStreamInitImpl<CNetViewDeskBand> IPersistStreamImpl;
public:
	CNetViewDeskBand()
		: m_fCompositionEnabled(FALSE)
		, m_bRequiresSave(FALSE)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_NETVIEWDESKBAND)


BEGIN_COM_MAP(CNetViewDeskBand)
	COM_INTERFACE_ENTRY(INetViewDeskBand)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IOleWindow)
	COM_INTERFACE_ENTRY(IDockingWindow)
	COM_INTERFACE_ENTRY(IDeskBand)
	COM_INTERFACE_ENTRY(IDeskBand2)
	COM_INTERFACE_ENTRY(IInputObject)
	COM_INTERFACE_ENTRY_IID(IID_IPersist, IPersistStreamImpl)
	COM_INTERFACE_ENTRY_IID(IID_IPersistStream, IPersistStreamImpl)
	COM_INTERFACE_ENTRY_IID(IID_IPersistStreamInit, IPersistStreamImpl)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CNetViewDeskBand)
	IMPLEMENTED_CATEGORY(CATID_DeskBand)	// 组建类别注册为桌面区
END_CATEGORY_MAP()

BEGIN_PROP_MAP(CNetViewDeskBand)

END_PROP_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

	// IObjectWithSite
	//
	STDMETHOD(SetSite)(_In_opt_ IUnknown *pUnkSite);

	// IOleWindow
	//
	STDMETHOD(GetWindow)(__RPC__deref_out_opt HWND *phwnd);

	STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

	// IDockingWindow
	//
	STDMETHOD(ShowDW)(BOOL fShow);

	STDMETHOD(CloseDW)(DWORD dwReserved);

	STDMETHOD(ResizeBorderDW)(__RPC__in_opt LPCRECT prcBorder, __RPC__in_opt IUnknown *punkToolbarSite, BOOL fReserved);

	// IDeskBand
	//
	STDMETHOD(GetBandInfo)(DWORD dwBandID, DWORD dwViewMode, __RPC__inout DESKBANDINFO *pdbi);

	// IDeskBand2
	//
	STDMETHOD(CanRenderComposited)(__RPC__out BOOL *pfCanRenderComposited);

	STDMETHOD(SetCompositionState)(BOOL fCompositionEnabled);

	STDMETHOD(GetCompositionState)(__RPC__out BOOL *pfCompositionEnabled);

	// IInputObject
	//
	STDMETHOD(UIActivateIO)(BOOL fActivate, __RPC__in_opt MSG *pMsg);

	STDMETHOD(HasFocusIO)(void);

	STDMETHOD(TranslateAcceleratorIO)(__RPC__in MSG *pMsg);

	BOOL        m_bRequiresSave;

private:
	DWORD	    m_dwBandID;						// identifier of the band
	BOOL	    m_fCompositionEnabled;			// 
	CNetViewWnd m_NetViewWnd;

};

OBJECT_ENTRY_AUTO(__uuidof(NetViewDeskBand), CNetViewDeskBand)
//OBJECT_ENTRY_AUTO(CLSID_NetViewDeskBand, CNetViewDeskBand)
