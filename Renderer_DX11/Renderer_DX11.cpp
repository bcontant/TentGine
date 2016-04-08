#include "Renderer_DX11.h"

#include <DirectXMath.h>

#include "../OS_Base/Window.h"

#include "Texture_DX11.h"
#include "Quad_DX11.h"

#include "simple_pixel.h"
#include "simple_vertex.h"

Renderer_DX11::Renderer_DX11(Window* in_pWindow)
	:Renderer(in_pWindow)
{
}

Renderer_DX11::~Renderer_DX11()
{
	pBlendState->Release();
	pPixelShader->Release();
	pVertexShader->Release();
	pBackBufferView->Release();
	pBackBuffer->Release();
	pSwapChain->Release();
	pD3D11DeviceContext->Release();
	pD3D11Device->Release();
}

Texture* Renderer_DX11::CreateTexture(std::wstring filename)
{
	Texture* pTexture = new Texture_DX11(this);
	pTexture->Initialize(filename);
	return pTexture;
}

Texture* Renderer_DX11::CreateTexture(unsigned int width, unsigned int height, const unsigned char* pBits)
{
	Texture* pTexture = new Texture_DX11(this);
	pTexture->Initialize(width, height, pBits);
	return pTexture;
}

Quad* Renderer_DX11::CreateQuad(float posX, float posY, Texture* texture)
{
	Quad_DX11* pQuad = new Quad_DX11(this);
	pQuad->SetTexture(texture);
	pQuad->SetPosition(posX, posY);

	return pQuad;
}

void Renderer_DX11::StartFrame()
{
	if (GetBackBufferWidth() != m_pWindow->GetWidth() || GetBackBufferHeight() != m_pWindow->GetHeight())
	{
		wchar_t debug[512];
		swprintf_s(debug, 512, L"Resizing. [Backbuffer : %d x %d] [ClientRect : %d x %d]\n", GetBackBufferWidth(), GetBackBufferHeight(), m_pWindow->GetWidth(), m_pWindow->GetHeight());
		OutputDebugString(debug);

		pBackBufferView->Release();
		pBackBuffer->Release();

		HRESULT hr = S_OK;
		hr = pSwapChain->ResizeBuffers(1, m_pWindow->GetWidth(), m_pWindow->GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
		hr = pD3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &pBackBufferView);

		pD3D11DeviceContext->OMSetRenderTargets(1, &pBackBufferView, NULL);

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

void Renderer_DX11::EndFrame()
{
	HRESULT hr = pSwapChain->Present(0, 0);
}

unsigned int Renderer_DX11::GetBackBufferWidth() const
{
	D3D11_TEXTURE2D_DESC desc;
	pBackBuffer->GetDesc(&desc);
	return desc.Width;
}
unsigned int Renderer_DX11::GetBackBufferHeight() const
{
	D3D11_TEXTURE2D_DESC desc;
	pBackBuffer->GetDesc(&desc);
	return desc.Height;
}

void Renderer_DX11::Initialize(DisplayAdapter* in_pAdapter, Window* in_pWindow)
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

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, 0, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pD3D11Device, NULL, &pD3D11DeviceContext);
	//pRenderer->pSwapChain->SetFullscreenState(TRUE, NULL);

	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	hr = pD3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &pBackBufferView);
	pD3D11DeviceContext->OMSetRenderTargets(1, &pBackBufferView, NULL);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)in_pWindow->GetWidth();
	viewport.Height = (FLOAT)in_pWindow->GetHeight();

	pD3D11DeviceContext->RSSetViewports(1, &viewport);

	hr = pD3D11Device->CreateVertexShader(g_VShader, sizeof(g_VShader), NULL, &pVertexShader);
	hr = pD3D11Device->CreatePixelShader(g_PShader, sizeof(g_PShader), NULL, &pPixelShader);
	pD3D11DeviceContext->VSSetShader(pVertexShader, 0, 0);
	pD3D11DeviceContext->PSSetShader(pPixelShader, 0, 0);

	D3D11_BLEND_DESC BlendState = {};
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pD3D11Device->CreateBlendState(&BlendState, &pBlendState);

	pD3D11DeviceContext->OMSetBlendState(pBlendState, NULL, 0xffffffff);

	IDXGIDevice * pDXGIDevice;
	hr = pD3D11Device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);

	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

	IDXGIFactory * pIDXGIFactory;
	hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

	//hr = pIDXGIFactory->MakeWindowAssociation(window->m_hwnd, DXGI_MWA_NO_WINDOW_CHANGES);
	hr = pIDXGIFactory->MakeWindowAssociation((HWND)in_pWindow->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);

	pIDXGIFactory->Release();
	pDXGIAdapter->Release();
	pDXGIDevice->Release();
}

Renderer* Renderer::CreateRenderer(DisplayAdapter* in_pAdapter, Window* in_pWindow)
{
	Renderer_DX11* pRenderer = new Renderer_DX11(in_pWindow);
	pRenderer->Initialize(in_pAdapter, in_pWindow);
	return pRenderer;
}