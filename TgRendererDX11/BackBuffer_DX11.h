#pragma once

#include <d3d11.h>
#include "../TgRendererBase/BackBuffer.h"

class BackBuffer_DX11 : public BackBuffer
{
public:
	BackBuffer_DX11(Renderer* pOwner) : BackBuffer(pOwner) {}
	virtual ~BackBuffer_DX11();

	virtual unsigned int GetWidth();
	virtual unsigned int GetHeight();

	ID3D11Texture2D* pBackBuffer;
	ID3D11RenderTargetView* pBackBufferView;
};