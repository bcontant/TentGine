#include "precompiled.h"

#include "DXUtils.h"
#include "../Base/BitmapData.h"


//--------------------------------------------------------------------------------
D3D11_USAGE GetD3D11Usage(u32 in_uiUsageType)
{
	if (in_uiUsageType & eUT_CPU_Readable)
		return D3D11_USAGE_STAGING;
	else if (in_uiUsageType & eUT_CPU_Writable)
		return D3D11_USAGE_DYNAMIC;
	else if (in_uiUsageType & eUT_GPU_Writable)
		return D3D11_USAGE_DEFAULT;

	return D3D11_USAGE_IMMUTABLE;
}

//--------------------------------------------------------------------------------
u32 GetCPUAccessFlags(u32 in_uiUsageType)
{
	if (in_uiUsageType & eUT_CPU_Readable && in_uiUsageType & eUT_CPU_Writable)
		return D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	else if (in_uiUsageType & eUT_CPU_Readable)
		return D3D11_CPU_ACCESS_READ;
	else if (in_uiUsageType & eUT_CPU_Writable)
		return D3D11_CPU_ACCESS_WRITE;

	return 0;
}

//--------------------------------------------------------------------------------
D3D11_FILTER GetD3D11FilteringMode(EFilteringMode in_eMinFilter, EFilteringMode in_eMagFilter, EFilteringMode in_eMipFilter)
{
	if (in_eMinFilter == EFilteringMode::ePoint  && in_eMagFilter == EFilteringMode::ePoint  && in_eMipFilter == EFilteringMode::ePoint)  return D3D11_FILTER_MIN_MAG_MIP_POINT;
	if (in_eMinFilter == EFilteringMode::ePoint  && in_eMagFilter == EFilteringMode::ePoint  && in_eMipFilter == EFilteringMode::eLinear) return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	if (in_eMinFilter == EFilteringMode::ePoint  && in_eMagFilter == EFilteringMode::eLinear && in_eMipFilter == EFilteringMode::ePoint)  return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	if (in_eMinFilter == EFilteringMode::ePoint  && in_eMagFilter == EFilteringMode::eLinear && in_eMipFilter == EFilteringMode::eLinear) return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
	if (in_eMinFilter == EFilteringMode::eLinear && in_eMagFilter == EFilteringMode::ePoint  && in_eMipFilter == EFilteringMode::ePoint)  return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	if (in_eMinFilter == EFilteringMode::eLinear && in_eMagFilter == EFilteringMode::ePoint  && in_eMipFilter == EFilteringMode::eLinear) return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	if (in_eMinFilter == EFilteringMode::eLinear && in_eMagFilter == EFilteringMode::eLinear && in_eMipFilter == EFilteringMode::ePoint)  return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	if (in_eMinFilter == EFilteringMode::eLinear && in_eMagFilter == EFilteringMode::eLinear && in_eMipFilter == EFilteringMode::eLinear) return D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	Assert(false);
	return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
}

//--------------------------------------------------------------------------------
D3D11_TEXTURE_ADDRESS_MODE GetD3D11AddressingMode(EAddressingMode in_eAddressingMode)
{
	switch (in_eAddressingMode)
	{
	case EAddressingMode::eWrap:	return D3D11_TEXTURE_ADDRESS_WRAP;
	case EAddressingMode::eMirror:  return D3D11_TEXTURE_ADDRESS_MIRROR;
	case EAddressingMode::eClamp:	return D3D11_TEXTURE_ADDRESS_CLAMP;
	case EAddressingMode::eBorder:	return D3D11_TEXTURE_ADDRESS_BORDER;
	}

	Assert(false);
	return D3D11_TEXTURE_ADDRESS_WRAP;
}

//--------------------------------------------------------------------------------
DXGI_FORMAT GetDXGIFormat(BufferFormat in_eFormat)
{
	switch (in_eFormat)
	{
	case BufferFormat::A_U8:			return DXGI_FORMAT_A8_UNORM;
	case BufferFormat::R_U8:			return DXGI_FORMAT_R8_UNORM;

	case BufferFormat::X1R5G5B5_U16:	return DXGI_FORMAT_B5G5R5A1_UNORM;
	case BufferFormat::R5G6B5_U16:		return DXGI_FORMAT_B5G6R5_UNORM;
	case BufferFormat::A1R5G5B5_U16:	return DXGI_FORMAT_B5G5R5A1_UNORM;

	case BufferFormat::ARGB_U32:		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case BufferFormat::ABGR_U32:		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case BufferFormat::R_F32:			return DXGI_FORMAT_R32_FLOAT;

	case BufferFormat::A_U1:			return DXGI_FORMAT_R1_UNORM;

	case BufferFormat::A_U5:			
	case BufferFormat::BGR_U24:			
	case BufferFormat::RGB_U24:			
	case BufferFormat::RGBA_U32:
	case BufferFormat::INVALID_FORMAT:  break;
	}

	Assert(false);
	return DXGI_FORMAT_FORCE_UINT;
}

//--------------------------------------------------------------------------------
string_char* GetDXErrorString(HRESULT hr) 
{
	static string_char s_DXError[4096];
	if (FAILED(hr))
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), s_DXError, 4096, nullptr);
	}
	
	return s_DXError;
}

//--------------------------------------------------------------------------------
HResult::HResult(HRESULT in_hr)
	:m_hr(in_hr)
{
#ifndef _RETAIL
	if (FAILED(m_hr))
	{
		string_char* strDXError = GetDXErrorString(m_hr);
		Logger::GetInstance()->Log(LogCategory::System, LogSeverity::Error, LogType::FileAndDebug, L("DirectX Call Failed with this error : %s (%d)"), strDXError, m_hr);
	}
#endif
}

