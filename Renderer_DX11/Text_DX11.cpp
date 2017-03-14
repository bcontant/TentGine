#include "precompiled.h"

#include "Text_DX11.h"

#include "../Renderer_Base/Font.h"
#include "simple_vertex.h"

//--------------------------------------------------------------------------------
struct CBStruct
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

//--------------------------------------------------------------------------------
struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 UV;
};


Text_DX11::Text_DX11(Renderer* pOwner)
	: Text(pOwner) 
{
}
	
Text_DX11::~Text_DX11() 
{
	pInputLayout->Release();
	pDefaultSampler->Release();
	pVertexBuffer->Release();
	pConstantBuffer->Release();
}



void Text_DX11::Draw() 
{
	PROFILE_BLOCK;

	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HResult hr = pDX11Renderer->GetContext()->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	CBStruct* dataPtr = (CBStruct*)mappedResource.pData;

	//Scale Quad to texture's size and position it where it needs to go.
	dataPtr->world = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(1.f / GetOwner()->GetBackBufferWidth(), 1.f / GetOwner()->GetBackBufferHeight(), 1.f) * DirectX::XMMatrixTranslation(m_xPos, m_yPos, 0.f));
	dataPtr->view = DirectX::XMMatrixIdentity();
	//Transform our [0..1][0..1] coords to [-1..1][1..-1]
	dataPtr->proj = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(2.f, -2.f, 1.f) * DirectX::XMMatrixTranslation(-1.f, 1.f, 0.f));

	pDX11Renderer->GetContext()->Unmap(pConstantBuffer, 0);

	pDX11Renderer->GetContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	//	pDX11Renderer->GetContext()->PSSetShaderResources(0, 1, &((Texture_DX11*)pTexture)->pShaderResourceView);
	m_pFont->m_pFontTexture->Bind(0);
	pDX11Renderer->GetContext()->PSSetSamplers(0, 1, &pDefaultSampler);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pDX11Renderer->GetContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pDX11Renderer->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDX11Renderer->GetContext()->IASetInputLayout(pInputLayout);

	pDX11Renderer->GetContext()->Draw( static_cast<unsigned int>(6 * m_vCharacters.size()), 0);
}



SimpleVertex* pVBData = nullptr;

void Text_DX11::UpdateText() 
{
	Text::UpdateText();

	if (m_vCharacters.size() == 0)
		return;

	PROFILE_BLOCK;

	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	pVBData = new SimpleVertex[6 * static_cast<unsigned int>(m_vCharacters.size())];

	for (unsigned int i = 0; i < m_vCharacters.size(); i++)
	{
		Character& currentChar = m_vCharacters[i];
		SimpleVertex* pVB = &pVBData[i * 6];

		//Top Left
		pVB->Pos.x = (float)currentChar.m_Position.m_left; pVB->Pos.y = (float)currentChar.m_Position.m_top; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_left; pVB->UV.y = currentChar.m_UVs.m_top; pVB++;

		//Top Right
		pVB->Pos.x = (float)currentChar.m_Position.m_right; pVB->Pos.y = (float)currentChar.m_Position.m_top; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_right; pVB->UV.y = currentChar.m_UVs.m_top; pVB++;

		//Bottom Right
		pVB->Pos.x = (float)currentChar.m_Position.m_right; pVB->Pos.y = (float)currentChar.m_Position.m_bottom; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_right; pVB->UV.y = currentChar.m_UVs.m_bottom; pVB++;

		//Bottom Right
		pVB->Pos.x = (float)currentChar.m_Position.m_right; pVB->Pos.y = (float)currentChar.m_Position.m_bottom; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_right; pVB->UV.y = currentChar.m_UVs.m_bottom; pVB++;

		//Bottom Left
		pVB->Pos.x = (float)currentChar.m_Position.m_left; pVB->Pos.y = (float)currentChar.m_Position.m_bottom; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_left; pVB->UV.y = currentChar.m_UVs.m_bottom; pVB++;

		//Top Left
		pVB->Pos.x = (float)currentChar.m_Position.m_left; pVB->Pos.y = (float)currentChar.m_Position.m_top; pVB->Pos.z = 1.f; pVB->UV.x = currentChar.m_UVs.m_left; pVB->UV.y = currentChar.m_UVs.m_top; pVB++;
	}


	if (pVertexBuffer == nullptr)
	{
		
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.ByteWidth = 6 * sizeof(SimpleVertex) * static_cast<unsigned int>(m_vCharacters.size());
		//vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		//vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferInitData;
		vertexBufferInitData.pSysMem = pVBData;
		vertexBufferInitData.SysMemPitch = 0;
		vertexBufferInitData.SysMemSlicePitch = 0;

		SAFE_RELEASE(pVertexBuffer);
		HResult hr = pDX11Renderer->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferInitData, &pVertexBuffer);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		pDX11Renderer->GetContext()->Map(pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, pVBData, 6 * sizeof(SimpleVertex) * static_cast<unsigned int>(m_vCharacters.size()) );
		pDX11Renderer->GetContext()->Unmap(pVertexBuffer, 0);
	}

	if (pConstantBuffer == nullptr)
	{
		CBStruct WVP;

		D3D11_BUFFER_DESC constantBufferDesc;
		constantBufferDesc.ByteWidth = sizeof(CBStruct);
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;
		constantBufferDesc.StructureByteStride = 0;

		// Fill in the subresource data.
		D3D11_SUBRESOURCE_DATA constantBufferInitData;
		constantBufferInitData.pSysMem = &WVP;
		constantBufferInitData.SysMemPitch = 0;
		constantBufferInitData.SysMemSlicePitch = 0;

		SAFE_RELEASE(pConstantBuffer);
		HResult hr = pDX11Renderer->GetDevice()->CreateBuffer(&constantBufferDesc, &constantBufferInitData, &pConstantBuffer);

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		SAFE_RELEASE(pDefaultSampler);
		hr = pDX11Renderer->GetDevice()->CreateSamplerState(&samplerDesc, &pDefaultSampler);

		D3D11_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		SAFE_RELEASE(pInputLayout);
		hr = pDX11Renderer->GetDevice()->CreateInputLayout(inputElementDescs, 2, g_VShader, sizeof(g_VShader), &pInputLayout);
	}
}
