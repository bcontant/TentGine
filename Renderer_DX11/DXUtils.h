#pragma once

enum class BufferFormat;
enum class EFilteringMode;
enum class EAddressingMode;

DXGI_FORMAT GetDXGIFormat(BufferFormat in_eFormat);

D3D11_FILTER GetD3D11FilteringMode(EFilteringMode in_eMinFilter, EFilteringMode in_eMagFilter, EFilteringMode in_eMipFilter);
D3D11_TEXTURE_ADDRESS_MODE GetD3D11AddressingMode(EAddressingMode in_eAddressingMode);
D3D11_USAGE GetD3D11Usage(u32 in_uiUsageType);

u32 GetCPUAccessFlags(u32 in_uiUsageType);



#define SAFE_RELEASE(ptr) \
{ \
	if(ptr != nullptr) \
	{ \
		ptr->Release(); \
	} \
	ptr = 0; \
} 

#ifndef _RETAIL
#define SET_D3D11_OBJECT_NAME(obj) \
{ \
	HResult setPrivateDataRes = obj->SetPrivateData( WKPDID_D3DDebugObjectName, (unsigned int)strlen(__FUNCTION__##"::"#obj), __FUNCTION__##"::"#obj); \
}
#else
#define SET_D3D11_OBJECT_NAME(obj, name)
#endif

class HResult
{
public:
	HResult(HRESULT in_hr);

	operator HRESULT() const { return m_hr; }

	HRESULT m_hr;
};

