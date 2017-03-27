#include "precompiled.h"

#include "VertexBuffer.h"

//--------------------------------------------------------------------------------
VertexBuffer::VertexBuffer(Renderer* in_pOwner, u32 in_uiVertexCount, u32 in_uiVertexMask, EPrimitiveType in_ePrimitiveType, void* /*in_pVBData*/, u32 in_eBufferType)
	: RendererObject(in_pOwner)
	, m_uiVertexCount(in_uiVertexCount)
	, m_ePrimitiveType(in_ePrimitiveType)
	, m_uiVertexMask(in_uiVertexMask)
	, m_uiBufferType(in_eBufferType)
{
	CalculateVertexSize();
}

//--------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
}

//--------------------------------------------------------------------------------
void VertexBuffer::CalculateVertexSize()
{
	m_uiVertexSize = 0;

	if (m_uiVertexMask & eVB_Position)
		m_uiVertexSize += sizeof(float) * 3;

	if(m_uiVertexMask & eVB_2D_TexCoords)
		m_uiVertexSize += sizeof(float) * 2;

	if (m_uiVertexMask & eVB_Color)
	//	m_uiVertexSize += sizeof(float) * 4;
		m_uiVertexSize += sizeof(u32);

	Assert(m_uiVertexSize > 0);
}

//--------------------------------------------------------------------------------
void VertexBuffer::SetVertexCount(u32 in_uiVertexCount)
{
	Assert(m_uiBufferType & eUT_CPU_Writable || m_uiBufferType & eUT_GPU_Writable );

	m_uiVertexCount = in_uiVertexCount;
}