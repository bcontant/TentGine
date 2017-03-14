#pragma once

#include "../Renderer_Base/Renderer.h"

//--------------------------------------------------------------------------------
class Renderer_DX11 : public Renderer
{
public:
	Renderer_DX11(Window* in_pWindow);
	virtual ~Renderer_DX11();

	virtual void Initialize(DisplayAdapter* in_pAdapter, Window* in_pWindow);

	virtual Texture* CreateTexture(const Path& filename);
	virtual Texture* CreateTexture(const BitmapData* in_pData);
	virtual Quad* CreateQuad(float posX, float posY, Texture* texture);
	virtual Text* CreateText(float poxX, float posY, Font* in_pFont, const StdString& in_strText);

	virtual void StartFrame();
	virtual void EndFrame();

	virtual unsigned int GetBackBufferWidth() const;
	virtual unsigned int GetBackBufferHeight() const;

public:
	ID3D11Device* GetDevice() const { return pD3D11Device; }
	ID3D11DeviceContext* GetContext() const { return pD3D11DeviceContext; }

private:
	ID3D11Device* pD3D11Device;
	ID3D11DeviceContext* pD3D11DeviceContext;

	IDXGISwapChain* pSwapChain;
	ID3D11Texture2D* pBackBuffer;
	ID3D11RenderTargetView* pBackBufferView;

	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;
	ID3D11BlendState* pBlendState;
};