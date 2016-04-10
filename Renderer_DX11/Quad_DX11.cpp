#include "precompiled.h"

#include "simple_vertex.h"

struct CBStruct
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 UV;
};

/*
Explanation for why seemingly CCW triangles in a triangle strip work out to be CW :
Source : https://en.wikipedia.org/wiki/Triangle_strip
It follows from definition that a subsequence of vertices of a triangle strip also represents a triangle strip.
However, if this substrip starts at an even(with 1 - based counting) vertex, then the resulting triangles will change their orientation.
For example a substrip BCDEF would represent triangles : BCD, CED, DEF.

So in this case {ABCD}, we get ABC (CW) and BDC (CW, instead of BCD, which would be CCW)
*/
SimpleVertex quad_vertices[] =
{
	DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),
	DirectX::XMFLOAT2(1.0f, 0.0f),

	DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f),
	DirectX::XMFLOAT2(1.0f, 1.0f),

	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	DirectX::XMFLOAT2(0.0f, 0.0f),

	DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
	DirectX::XMFLOAT2(0.0f, 1.0f),
};

Quad_DX11::Quad_DX11(Renderer* pOwner) : Quad(pOwner)
{
	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = 4 * sizeof(SimpleVertex);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferInitData;
	vertexBufferInitData.pSysMem = quad_vertices;
	vertexBufferInitData.SysMemPitch = 0;
	vertexBufferInitData.SysMemSlicePitch = 0;

	HRESULT hr = pDX11Renderer->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferInitData, &pVertexBuffer);

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

	hr = pDX11Renderer->GetDevice()->CreateBuffer(&constantBufferDesc, &constantBufferInitData, &pConstantBuffer);

	D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = pDX11Renderer->GetDevice()->CreateSamplerState(&samplerDesc, &pDefaultSampler);

	D3D11_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = pDX11Renderer->GetDevice()->CreateInputLayout(inputElementDescs, 2, g_VShader, sizeof(g_VShader), &pInputLayout);
}

Quad_DX11::~Quad_DX11()
{
	pInputLayout->Release();
	pDefaultSampler->Release();
	pVertexBuffer->Release();
	pConstantBuffer->Release();
}

void Quad_DX11::SetPosition(float x, float y)
{
	m_Position.x = x;
	m_Position.y = y;
}

void Quad_DX11::Draw()
{
	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDX11Renderer->GetContext()->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	CBStruct* dataPtr = (CBStruct*)mappedResource.pData;

	//Scale Quad to texture's size and position it where it needs to go.
	dataPtr->world = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(float(pTexture->GetWidth()) / GetOwner()->GetBackBufferWidth(), float(pTexture->GetHeight()) / GetOwner()->GetBackBufferHeight(), 1.f) * DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, 0.f));
	dataPtr->view = DirectX::XMMatrixIdentity();
	//Transform our [0..1][0..1] coords to [-1..1][1..-1]
	dataPtr->proj = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(2.f, -2.f, 1.f) * DirectX::XMMatrixTranslation(-1.f, 1.f, 0.f));

	pDX11Renderer->GetContext()->Unmap(pConstantBuffer, 0);
	
	pDX11Renderer->GetContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer);
//	pDX11Renderer->GetContext()->PSSetShaderResources(0, 1, &((Texture_DX11*)pTexture)->pShaderResourceView);
	pTexture->Bind(0);
	pDX11Renderer->GetContext()->PSSetSamplers(0, 1, &pDefaultSampler);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pDX11Renderer->GetContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pDX11Renderer->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pDX11Renderer->GetContext()->IASetInputLayout(pInputLayout);

	pDX11Renderer->GetContext()->Draw(4, 0);
}