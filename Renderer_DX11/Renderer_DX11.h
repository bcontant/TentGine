#pragma once

#include "../Renderer_Base/Renderer.h"

//--------------------------------------------------------------------------------
class Renderer_DX11 : public Renderer
{
public:
	Renderer_DX11(Window* in_pWindow);
	virtual ~Renderer_DX11();

	void Initialize(DisplayAdapter* in_pAdapter) override;

	virtual Texture* CreateTexture(const Path& filename, EAddressingMode in_eAddressingMode, EFilteringMode in_eMinFilter, EFilteringMode in_eMagFilter, EFilteringMode in_eMipMapFilter) override;
	virtual Texture* CreateTexture(const BitmapData* in_pData, EAddressingMode in_eAddressingMode, EFilteringMode in_eMinFilter, EFilteringMode in_eMagFilter, EFilteringMode in_eMipMapFilter) override;
	virtual Quad* CreateQuad(float posX, float posY, Texture* texture) override;
	virtual Text* CreateText(float poxX, float posY, Font* in_pFont, const std_string& in_strText) override;
	virtual Text* CreateText(const rect_f& rc, Font* in_pFont, const std_string& in_strText) override;

	virtual VertexBuffer* CreateVertexBuffer(u32 in_uiVertexCount, u32 in_uiVertexMask, EPrimitiveType in_ePrimitiveType, void* in_pVBData, u32 in_eBufferType) override;
	virtual Shader* CreateShader(const Path& in_shaderFile);

	virtual void StartFrame() override;
	virtual void EndFrame() override;

	virtual u32 GetBackBufferWidth() const override;
	virtual u32 GetBackBufferHeight() const override;

public:
	ID3D11Device* GetDevice() const { return pD3D11Device; }
	ID3D11DeviceContext* GetContext() const { return pD3D11DeviceContext; }

private:
	void ReportLiveObjects() const;

	ID3D11Device* pD3D11Device;
	ID3D11DeviceContext* pD3D11DeviceContext;

	IDXGISwapChain* pSwapChain;
	ID3D11Texture2D* pBackBuffer;
	ID3D11RenderTargetView* pBackBufferView;

	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;
	ID3D11BlendState* pBlendState;

	ID3D11VertexShader* pVertexShader2;
	ID3D11PixelShader* pPixelShader2;
};