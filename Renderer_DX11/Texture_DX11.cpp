#include "precompiled.h"

#include "DXUtils.h"
#include "../Base/BitmapData.h"

//--------------------------------------------------------------------------------
Texture_DX11::Texture_DX11(Renderer* pOwner)
	: Texture(pOwner)
{
}

//--------------------------------------------------------------------------------
Texture_DX11::~Texture_DX11()
{
	m_pShaderResourceView->Release();
	m_pTexture->Release();
	m_pDefaultSampler->Release();
}

#include "../Base/TGAFile.h"

//--------------------------------------------------------------------------------
void Texture_DX11::Initialize(const Path& filename)
{
	BitmapData* pData = LoadBitmapData(filename);

	//TODO : ConvertToDXCompatibleFormat
	if (pData->GetFormat() == BufferFormat::BGR_U24)
		pData->ConvertTo(BufferFormat::ARGB_U32);

	Initialize(pData);
}

//--------------------------------------------------------------------------------
void Texture_DX11::Initialize(const BitmapData* in_pData)
{
	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	// Create texture
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = in_pData->GetWidth();
	desc.Height = in_pData->GetHeight();
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = GetDXGIFormat(in_pData->GetFormat());
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	//desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = in_pData->GetBuffer();
	initData.SysMemPitch = in_pData->GetBufferPitch();;
	initData.SysMemSlicePitch = in_pData->GetBufferSize();

	HResult hr = pDX11Renderer->GetDevice()->CreateTexture2D(&desc, &initData, &m_pTexture);
	SET_D3D11_OBJECT_NAME(m_pTexture);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	memset(&SRVDesc, 0, sizeof(SRVDesc));
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = pDX11Renderer->GetDevice()->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pShaderResourceView); 
	SET_D3D11_OBJECT_NAME(m_pShaderResourceView);

	CreateTextureSampler();
}

//--------------------------------------------------------------------------------
void Texture_DX11::CreateTextureSampler()
{
	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
	samplerDesc.Filter = GetD3D11FilteringMode(m_eMinFilter, m_eMagFilter, m_eMipFilter);
	samplerDesc.AddressU = GetD3D11AddressingMode(m_eAddressingMode);
	samplerDesc.AddressV = GetD3D11AddressingMode(m_eAddressingMode);
	samplerDesc.AddressW = GetD3D11AddressingMode(m_eAddressingMode);

	HResult hr = pDX11Renderer->GetDevice()->CreateSamplerState(&samplerDesc, &m_pDefaultSampler);
	SET_D3D11_OBJECT_NAME(m_pDefaultSampler);
}

//--------------------------------------------------------------------------------
void Texture_DX11::Bind(u32 startSlot) const
{
	Renderer_DX11* pDX11Renderer = (Renderer_DX11*) GetOwner();
	pDX11Renderer->GetContext()->PSSetShaderResources(startSlot, 1, &m_pShaderResourceView);
	pDX11Renderer->GetContext()->PSSetSamplers(startSlot, 1, &m_pDefaultSampler);
}

//--------------------------------------------------------------------------------
u32 Texture_DX11::GetWidth() const
{
	//TODO : this is dumb.  cache this shit and make it cross platform
	D3D11_TEXTURE2D_DESC textureDesc;
	m_pTexture->GetDesc(&textureDesc);
	return textureDesc.Width;
}

//--------------------------------------------------------------------------------
u32 Texture_DX11::GetHeight() const
{
	//TODO : this is dumb.  cache this shit and make it cross platform
	D3D11_TEXTURE2D_DESC textureDesc;
	m_pTexture->GetDesc(&textureDesc);
	return textureDesc.Height;
}

