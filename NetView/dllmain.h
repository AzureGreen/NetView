// dllmain.h: 模块类的声明。

class CNetViewModule : public ATL::CAtlDllModuleT< CNetViewModule >
{
public :
	DECLARE_LIBID(LIBID_NetViewLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_NETVIEW, "{F5430BE5-BE55-4CA9-8FEE-AA2D4C6A0BA5}")
};

extern class CNetViewModule _AtlModule;
