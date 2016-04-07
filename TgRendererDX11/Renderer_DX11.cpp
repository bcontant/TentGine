#include "Renderer_DX11.h"

#include <DirectXMath.h>

#include "../TgOSBase/Window.h"

#include "BackBuffer_DX11.h"
#include "Texture_DX11.h"
#include "Quad_DX11.h"

#include "simple_pixel.h"
#include "simple_vertex.h"

Renderer_DX11::Renderer_DX11()
{
}

Renderer_DX11::~Renderer_DX11()
{
	pBlendState->Release();
	pPixelShader->Release();
	pVertexShader->Release();
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
	pQuad->pTexture = texture;
	pQuad->m_Position.x = posX;
	pQuad->m_Position.y = posY;

	return pQuad;
}

void Renderer_DX11::StartFrame()
{
	DirectX::XMFLOAT4 clearColor(0.f, 0.2f, 0.4f, 1.f);
	pD3D11DeviceContext->ClearRenderTargetView(((BackBuffer_DX11*)m_pBackBuffer)->pBackBufferView, (CONST FLOAT*)&clearColor);
}

void Renderer_DX11::EndFrame()
{
	pSwapChain->Present(0, 0);
}


Renderer* Renderer::CreateRenderer(DisplayAdapter* adapter, Window* window)
{
	Renderer_DX11* pRenderer = new Renderer_DX11();

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = window->GetWidth();
	swapChainDesc.BufferDesc.Height = window->GetHeight();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = (HWND)window->GetWindowHandle();
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, 0, D3D11_SDK_VERSION, &swapChainDesc, &pRenderer->pSwapChain, &pRenderer->pD3D11Device, NULL, &pRenderer->pD3D11DeviceContext);
	//pRenderer->pSwapChain->SetFullscreenState(TRUE, NULL);

	BackBuffer_DX11* pBackBuffer = new BackBuffer_DX11(pRenderer);
	pRenderer->m_pBackBuffer = pBackBuffer;
	hr = pRenderer->pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer->pBackBuffer);
	hr = pRenderer->pD3D11Device->CreateRenderTargetView(pBackBuffer->pBackBuffer, NULL, &pBackBuffer->pBackBufferView);
	pRenderer->pD3D11DeviceContext->OMSetRenderTargets(1, &pBackBuffer->pBackBufferView, NULL);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)window->GetWidth();
	viewport.Height = (FLOAT)window->GetHeight();

	pRenderer->pD3D11DeviceContext->RSSetViewports(1, &viewport);

	hr = pRenderer->pD3D11Device->CreateVertexShader(g_VShader, sizeof(g_VShader), NULL, &pRenderer->pVertexShader);
	hr = pRenderer->pD3D11Device->CreatePixelShader(g_PShader, sizeof(g_PShader), NULL, &pRenderer->pPixelShader);
	pRenderer->pD3D11DeviceContext->VSSetShader(pRenderer->pVertexShader, 0, 0);
	pRenderer->pD3D11DeviceContext->PSSetShader(pRenderer->pPixelShader, 0, 0);

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
	pRenderer->pD3D11Device->CreateBlendState(&BlendState, &pRenderer->pBlendState);

	pRenderer->pD3D11DeviceContext->OMSetBlendState(pRenderer->pBlendState, NULL, 0xffffffff);

	IDXGIDevice * pDXGIDevice;
	hr = pRenderer->pD3D11Device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);

	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

	IDXGIFactory * pIDXGIFactory;
	hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

	//hr = pIDXGIFactory->MakeWindowAssociation(window->m_hwnd, DXGI_MWA_NO_WINDOW_CHANGES);
	hr = pIDXGIFactory->MakeWindowAssociation((HWND)window->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);

	pIDXGIFactory->Release();
	pDXGIAdapter->Release();
	pDXGIDevice->Release();

	return pRenderer;
}