#pragma once

#include "d3d11.h"
#include "../TgRendererBase/Renderer.h"

class Renderer_DX11 : public Renderer
{
public:
	Renderer_DX11();
	virtual ~Renderer_DX11();

	virtual Texture* CreateTexture(std::wstring filename);
	virtual Texture* CreateTexture(unsigned int width, unsigned int height, const unsigned char* pBits);
	virtual Quad* CreateQuad(float posX, float posY, Texture* texture);

	virtual void StartFrame();
	virtual void EndFrame();

	ID3D11Device* pD3D11Device;
	ID3D11DeviceContext* pD3D11DeviceContext;
	IDXGISwapChain* pSwapChain;
	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;
	ID3D11BlendState* pBlendState;
};