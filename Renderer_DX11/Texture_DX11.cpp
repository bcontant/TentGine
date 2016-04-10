#include "precompiled.h"

Texture_DX11::~Texture_DX11()
{
	pShaderResourceView->Release();
	pTexture->Release();
}

void Texture_DX11::Initialize(const Path& filename)
{
	Renderer_DX11* pOwner = (Renderer_DX11*)GetOwner();
	HRESULT hr = CreateWICTextureFromFile(pOwner->GetDevice(), pOwner->GetContext(), filename.GetData(), &pTexture, &pShaderResourceView, 0);
}

void Texture_DX11::Initialize(unsigned int width, unsigned int height, const unsigned char* pBits)
{
	Renderer_DX11* pOwner = (Renderer_DX11*)GetOwner();

	// Create texture
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	//desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Format = DXGI_FORMAT_R8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pBits;
	initData.SysMemPitch = width;
	initData.SysMemSlicePitch = width * height;

	HRESULT hr = pOwner->GetDevice()->CreateTexture2D(&desc, &initData, &pTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	memset(&SRVDesc, 0, sizeof(SRVDesc));
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = pOwner->GetDevice()->CreateShaderResourceView(pTexture, &SRVDesc, &pShaderResourceView);
}

void Texture_DX11::Bind(unsigned int startSlot)
{
	Renderer_DX11* pDX11Renderer = (Renderer_DX11*) GetOwner();
	pDX11Renderer->GetContext()->PSSetShaderResources(0, 1, &pShaderResourceView);
}

unsigned int Texture_DX11::GetWidth() const
{
	D3D11_TEXTURE2D_DESC textureDesc;
	pTexture->GetDesc(&textureDesc);
	return textureDesc.Width;
}

unsigned int Texture_DX11::GetHeight() const
{
	D3D11_TEXTURE2D_DESC textureDesc;
	pTexture->GetDesc(&textureDesc);
	return textureDesc.Height;
}
