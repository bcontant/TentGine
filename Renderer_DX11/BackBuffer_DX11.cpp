/*#include "BackBuffer_DX11.h"

BackBuffer_DX11::~BackBuffer_DX11()
{
	pBackBufferView->Release();
	pBackBuffer->Release();
}

unsigned int BackBuffer_DX11::GetWidth()
{
	D3D11_TEXTURE2D_DESC desc;
	pBackBuffer->GetDesc(&desc);
	return desc.Width;
}

unsigned int BackBuffer_DX11::GetHeight()
{
	D3D11_TEXTURE2D_DESC desc;
	pBackBuffer->GetDesc(&desc);
	return desc.Height;
}*/