#pragma once

enum class BufferFormat;

DXGI_FORMAT GetDXGIFormat(BufferFormat in_eFormat);

#define SAFE_RELEASE(ptr) \
{ \
	if(ptr != nullptr) \
	{ \
		ptr->Release(); \
	} \
	ptr = 0; \
} 

class HResult
{
public:
	HResult(HRESULT in_hr);

	operator HRESULT() const { return m_hr; }

	HRESULT m_hr;
};

