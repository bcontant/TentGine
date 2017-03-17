#include "precompiled.h"

#include "simple_pixel.h"
#include "simple_vertex.h"

//--------------------------------------------------------------------------------
Renderer_DX11::Renderer_DX11(Window* in_pWindow)
	:Renderer(in_pWindow)
{
}

//--------------------------------------------------------------------------------
Renderer_DX11::~Renderer_DX11()
{
	s32 bIsFullscreen;
	pSwapChain->GetFullscreenState(&bIsFullscreen, nullptr);
	if(bIsFullscreen)
		pSwapChain->SetFullscreenState(false, nullptr);

	pBlendState->Release();
	pPixelShader->Release();
	pVertexShader->Release();
	pBackBufferView->Release();
	pBackBuffer->Release();
	pSwapChain->Release();
	pD3D11DeviceContext->Release();

	u32 refCount = pD3D11Device->Release();

	if (refCount > 0)
		ReportLiveObjects();
}

//--------------------------------------------------------------------------------
void Renderer_DX11::ReportLiveObjects() const
{
	ID3D11Debug* pD3DDebug = nullptr;
	pD3D11Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pD3DDebug));

	if (pD3DDebug)
	{
		pD3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		pD3DDebug->Release();
	}
}

//--------------------------------------------------------------------------------
Texture* Renderer_DX11::CreateTexture(const Path& filename, EAddressingMode in_eAddressingMode, EFilteringMode in_eMinFilter, EFilteringMode in_eMagFilter, EFilteringMode in_eMipMapFilter)
{
	Texture* pTexture = new Texture_DX11(this);
	pTexture->SetAddressingMode(in_eAddressingMode);
	pTexture->SetMinFilter(in_eMinFilter);
	pTexture->SetMagFilter(in_eMagFilter);
	pTexture->SetMipMapFilter(in_eMipMapFilter);
	pTexture->Initialize(filename);
	return pTexture;
}

//--------------------------------------------------------------------------------
Texture* Renderer_DX11::CreateTexture(const BitmapData* in_pData, EAddressingMode in_eAddressingMode, EFilteringMode in_eMinFilter, EFilteringMode in_eMagFilter, EFilteringMode in_eMipMapFilter)
{
	Texture* pTexture = new Texture_DX11(this);
	pTexture->SetAddressingMode(in_eAddressingMode);
	pTexture->SetMinFilter(in_eMinFilter);
	pTexture->SetMagFilter(in_eMagFilter);
	pTexture->SetMipMapFilter(in_eMipMapFilter);
	pTexture->Initialize(in_pData);
	return pTexture;
}

//--------------------------------------------------------------------------------
Quad* Renderer_DX11::CreateQuad(float posX, float posY, Texture* texture)
{
	Quad_DX11* pQuad = new Quad_DX11(this);
	pQuad->SetTexture(texture);
	pQuad->SetPosition(posX, posY);

	return pQuad;
}

//--------------------------------------------------------------------------------
Text* Renderer_DX11::CreateText(float posX, float posY, Font* in_pFont, const std_string& in_strText)
{
	Text_DX11* pText = new Text_DX11(this);
	pText->SetPosition(posX, posY);
	pText->SetFont(in_pFont);
	pText->SetText(in_strText);
	return pText;
}

//--------------------------------------------------------------------------------
VertexBuffer* Renderer_DX11::CreateVertexBuffer(u32 in_uiVertexCount, u32 in_uiVertexMask, EPrimitiveType in_ePrimitiveType, void* in_pVBData, u32 in_eBufferType)
{
	VertexBuffer_DX11* pVB = new VertexBuffer_DX11(this, in_uiVertexCount, in_uiVertexMask, in_ePrimitiveType, in_pVBData, in_eBufferType);
	return pVB;
}

//--------------------------------------------------------------------------------
void Renderer_DX11::StartFrame()
{
	if (GetBackBufferWidth() != m_pWindow->GetWidth() || GetBackBufferHeight() != m_pWindow->GetHeight())
	{
		Logger::GetInstance()->Log(LogCategory::Rendering, LogSeverity::Message, LogType::FileAndDebug, L("Resizing. [Backbuffer : %d x %d] [ClientRect : %d x %d]"), GetBackBufferWidth(), GetBackBufferHeight(), m_pWindow->GetWidth(), m_pWindow->GetHeight());

		pBackBufferView->Release();
		pBackBuffer->Release();

		HResult hr = S_OK;
		hr = pSwapChain->ResizeBuffers(1, m_pWindow->GetWidth(), m_pWindow->GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
		hr = pD3D11Device->CreateRenderTargetView(pBackBuffer, nullptr, &pBackBufferView);

		SET_D3D11_OBJECT_NAME(pBackBuffer);
		SET_D3D11_OBJECT_NAME(pBackBufferView);

		pD3D11DeviceContext->OMSetRenderTargets(1, &pBackBufferView, nullptr);

		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (FLOAT)m_pWindow->GetWidth();
		viewport.Height = (FLOAT)m_pWindow->GetHeight();

		pD3D11DeviceContext->RSSetViewports(1, &viewport);
	}

	DirectX::XMFLOAT4 clearColor(0.f, 0.2f, 0.4f, 1.f);
	pD3D11DeviceContext->ClearRenderTargetView(pBackBufferView, (CONST FLOAT*)&clearColor);
}

//--------------------------------------------------------------------------------
void Renderer_DX11::EndFrame()
{
	HResult hr = pSwapChain->Present(0, 0);
}

//--------------------------------------------------------------------------------
u32 Renderer_DX11::GetBackBufferWidth() const
{
	D3D11_TEXTURE2D_DESC desc;
	pBackBuffer->GetDesc(&desc);
	return desc.Width;
}

//--------------------------------------------------------------------------------
u32 Renderer_DX11::GetBackBufferHeight() const
{
	D3D11_TEXTURE2D_DESC desc;
	pBackBuffer->GetDesc(&desc);
	return desc.Height;
}

//--------------------------------------------------------------------------------
void Renderer_DX11::Initialize(DisplayAdapter* /*in_pAdapter*/, Window* in_pWindow)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = in_pWindow->GetWidth();
	swapChainDesc.BufferDesc.Height = in_pWindow->GetHeight();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = (HWND)in_pWindow->GetWindowHandle();
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

#ifndef _RETAIL
	HResult hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pD3D11Device, nullptr, &pD3D11DeviceContext);
#else
	HResult hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pD3D11Device, nullptr, &pD3D11DeviceContext);
#endif

	SET_D3D11_OBJECT_NAME(pSwapChain);
	SET_D3D11_OBJECT_NAME(pD3D11Device);
	SET_D3D11_OBJECT_NAME(pD3D11DeviceContext);

	//pSwapChain->SetFullscreenState(TRUE, nullptr);
	
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	hr = pD3D11Device->CreateRenderTargetView(pBackBuffer, nullptr, &pBackBufferView);
	pD3D11DeviceContext->OMSetRenderTargets(1, &pBackBufferView, nullptr);

	SET_D3D11_OBJECT_NAME(pBackBuffer);
	SET_D3D11_OBJECT_NAME(pBackBufferView);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)in_pWindow->GetWidth();
	viewport.Height = (FLOAT)in_pWindow->GetHeight();

	pD3D11DeviceContext->RSSetViewports(1, &viewport);

	hr = pD3D11Device->CreateVertexShader(g_VShader, sizeof(g_VShader), nullptr, &pVertexShader);
	hr = pD3D11Device->CreatePixelShader(g_PShader, sizeof(g_PShader), nullptr, &pPixelShader);
	pD3D11DeviceContext->VSSetShader(pVertexShader, 0, 0);
	pD3D11DeviceContext->PSSetShader(pPixelShader, 0, 0);

	SET_D3D11_OBJECT_NAME(pVertexShader);
	SET_D3D11_OBJECT_NAME(pPixelShader);

	D3D11_BLEND_DESC BlendState = {};
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pD3D11Device->CreateBlendState(&BlendState, &pBlendState);

	SET_D3D11_OBJECT_NAME(pBlendState);

	pD3D11DeviceContext->OMSetBlendState(pBlendState, nullptr, 0xffffffff);

	/*IDXGIDevice * pDXGIDevice;
	hr = pD3D11Device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);

	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

	IDXGIFactory * pIDXGIFactory;
	hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

	//hr = pIDXGIFactory->MakeWindowAssociation((HWND)in_pWindow->GetWindowHandle(), DXGI_MWA_NO_WINDOW_CHANGES);
	//hr = pIDXGIFactory->MakeWindowAssociation((HWND)in_pWindow->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);

	pIDXGIFactory->Release();
	pDXGIAdapter->Release();
	pDXGIDevice->Release();*/
}

//--------------------------------------------------------------------------------
Renderer* Renderer::CreateRenderer(DisplayAdapter* in_pAdapter, Window* in_pWindow)
{
	Renderer_DX11* pRenderer = new Renderer_DX11(in_pWindow);
	pRenderer->Initialize(in_pAdapter, in_pWindow);
	return pRenderer;
}
