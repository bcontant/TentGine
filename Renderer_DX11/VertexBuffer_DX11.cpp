#include "precompiled.h"

#include "VertexBuffer_DX11.h"

//--------------------------------------------------------------------------------
VertexBuffer_DX11::VertexBuffer_DX11(Renderer* in_pOwner, u32 in_uiVertexCount, u32 in_uiVertexMask, EPrimitiveType in_ePrimitiveType, void* in_pVBData, u32 in_eBufferType)
	: VertexBuffer(in_pOwner, in_uiVertexCount, in_uiVertexMask, in_ePrimitiveType, in_pVBData, in_eBufferType)
{
	Init(in_pVBData);
}

//--------------------------------------------------------------------------------
VertexBuffer_DX11::~VertexBuffer_DX11()
{
	SAFE_RELEASE(m_pD3D11VertexBuffer);
}

//--------------------------------------------------------------------------------
void* VertexBuffer_DX11::Lock()
{
	Assert(m_uiBufferType & eUT_CPU_Readable || m_uiBufferType & eUT_CPU_Writable);

	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	D3D11_MAPPED_SUBRESOURCE resource;
	HResult hr = pDX11Renderer->GetContext()->Map(m_pD3D11VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	return resource.pData;
}

//--------------------------------------------------------------------------------
void VertexBuffer_DX11::Unlock()
{
	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	pDX11Renderer->GetContext()->Unmap(m_pD3D11VertexBuffer, 0);
}

//--------------------------------------------------------------------------------
void VertexBuffer_DX11::Bind() const
{
	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	u32 stride = GetVertexSize();
	u32 offset = 0;
	pDX11Renderer->GetContext()->IASetVertexBuffers(0, 1, &m_pD3D11VertexBuffer, &stride, &offset);
	pDX11Renderer->GetContext()->IASetPrimitiveTopology(GetD3DPrimitiveTopology());
}

//--------------------------------------------------------------------------------
void VertexBuffer_DX11::SetVertexCount(u32 in_uiVertexCount)
{
	if (in_uiVertexCount == m_uiVertexCount)
		return;

	u32 uiOldCount = m_uiVertexCount;

	VertexBuffer::SetVertexCount(in_uiVertexCount);

	if (m_uiVertexCount > uiOldCount)
	{
		Init(nullptr);
	}
}

//--------------------------------------------------------------------------------
void VertexBuffer_DX11::Init(const void* in_pData)
{
	//If buffer is constant, it needs to be initialized with data.
	AssertMsg(!(m_uiBufferType & eUT_Constant && in_pData == nullptr), L("Constant Buffers need to be initialized with data."));
	AssertMsg(!(m_uiBufferType & eUT_Constant && m_uiVertexCount == 0), L("Constant Buffers need to be initialized with data."));

	//If buffer is not CPU writable, it needs to be initialized with data. (at least until GPU write support/use)
	AssertMsg(!(!(m_uiBufferType & eUT_CPU_Writable) && in_pData == nullptr), L("Non-CPU Writable Buffers need to be initialized with data."));
	AssertMsg(!(!(m_uiBufferType & eUT_CPU_Writable) && m_uiVertexCount == 0), L("Non-CPU Writable Buffers need to be initialized with data."));

	if (GetVertexCount() == 0)
		return;

	Renderer_DX11* pDX11Renderer = (Renderer_DX11*)GetOwner();

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = GetVertexCount() * GetVertexSize();
	vertexBufferDesc.Usage = GetD3D11Usage(m_uiBufferType);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = GetCPUAccessFlags(m_uiBufferType);
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferInitData;
	vertexBufferInitData.pSysMem = in_pData;
	vertexBufferInitData.SysMemPitch = 0;
	vertexBufferInitData.SysMemSlicePitch = 0;

	SAFE_RELEASE(m_pD3D11VertexBuffer);

	if(in_pData == nullptr)
		HResult hr = pDX11Renderer->GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_pD3D11VertexBuffer);
	else
		HResult hr = pDX11Renderer->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferInitData, &m_pD3D11VertexBuffer);

	SET_D3D11_OBJECT_NAME(m_pD3D11VertexBuffer);
}

//--------------------------------------------------------------------------------
D3D_PRIMITIVE_TOPOLOGY VertexBuffer_DX11::GetD3DPrimitiveTopology() const
{
	switch (m_ePrimitiveType)
	{
	case EPrimitiveType::ePT_TriangleList:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case EPrimitiveType::ePT_TriangleStrip:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	default:
		Assert(false);
	}
	return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

