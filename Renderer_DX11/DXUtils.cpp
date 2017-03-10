#include "precompiled.h"

#include "DXUtils.h"
#include "../Base/BitmapData.h"

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

StringChar* GetDXErrorString(HRESULT hr) 
{
	static StringChar s_DXError[4096];
	if (FAILED(hr))
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), s_DXError, 4096, NULL);
	}
	
	return s_DXError;
}

//#ifdef _DEBUG

HResult::HResult(HRESULT in_hr)
	:m_hr(in_hr)
{
#ifdef _DEBUG
	if (FAILED(m_hr))
	{
		StringChar* strDXError = GetDXErrorString(m_hr);
		Logger::GetInstance()->Log(LogCategory::System, LogSeverity::Error, LogType::FileAndDebug, "DirectX Call Failed with this error : %s (%d)", strDXError, m_hr);
	}
#endif
}

//#endif
