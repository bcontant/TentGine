#include "precompiled.h"

#include "Text_DX11.h"

#include "../Renderer_Base/Font.h"

// #include "simple_vertex.h"
// #include "vs_simple_2d_ptc.h"

//--------------------------------------------------------------------------------
struct CBStruct
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

//--------------------------------------------------------------------------------
Text_DX11::Text_DX11(Renderer* pOwner)
	: Text(pOwner) 
{
	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

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
	SET_D3D11_OBJECT_NAME(pConstantBuffer);

	D3D11_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",	  0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	File shaderFile;
#ifdef _DEBUG
	shaderFile.Open(L("../../data/shaders/Debug/vs_simple_2d_ptc.cso"), FileMode::ReadOnly);
#else
	shaderFile.Open(L("../../data/shaders/Release/vs_simple_2d_ptc.cso"), FileMode::ReadOnly);
#endif

	u8* shaderData = new u8[shaderFile.Size()];
	shaderFile.Read(shaderData, shaderFile.Size());

	SAFE_RELEASE(pInputLayout);
	//hr = pDX11Renderer->GetDevice()->CreateInputLayout(inputElementDescs, 3, g_vs_simple_2d_ptc, sizeof(g_vs_simple_2d_ptc), &pInputLayout);
	hr = pDX11Renderer->GetDevice()->CreateInputLayout(inputElementDescs, 3, shaderData, shaderFile.Size(), &pInputLayout);
	SET_D3D11_OBJECT_NAME(pInputLayout);
}
	
//--------------------------------------------------------------------------------
Text_DX11::~Text_DX11() 
{
	pInputLayout->Release();
	pConstantBuffer->Release();
}

//--------------------------------------------------------------------------------
void Text_DX11::Draw() 
{
	PROFILE_BLOCK;

	UpdateText();

	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HResult hr = pDX11Renderer->GetContext()->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	CBStruct* dataPtr = (CBStruct*)mappedResource.pData;

	dataPtr->world = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(1.f / GetOwner()->GetBackBufferWidth(), 1.f / GetOwner()->GetBackBufferHeight(), 1.f) * DirectX::XMMatrixTranslation(m_xPos, m_yPos, 0.f));
	dataPtr->view = DirectX::XMMatrixIdentity();
	dataPtr->proj = DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(2.f, -2.f, 1.f) * DirectX::XMMatrixTranslation(-1.f, 1.f, 0.f));

	pDX11Renderer->GetContext()->Unmap(pConstantBuffer, 0);

	pDX11Renderer->GetContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer);

	m_pFont->GetTexture()->Bind(0);
	m_pVertexBuffer->Bind();

	pDX11Renderer->GetContext()->IASetInputLayout(pInputLayout);

	pDX11Renderer->GetContext()->Draw(m_pVertexBuffer->GetVertexCount(), 0);
}
