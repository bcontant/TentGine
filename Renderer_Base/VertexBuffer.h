#pragma once

#include "Renderer.h"
#include "RendererObject.h"

enum class EPrimitiveType
{
	ePT_TriangleList,
	ePT_TriangleStrip,
};

class VertexBuffer : public RendererObject
{
public:
	enum EVBContents
	{
		eVB_Position		= 1 << 0,
		eVB_2D_TexCoords	= 1 << 1,
		eVB_Color			= 1 << 2
	};
	
	VertexBuffer(Renderer* in_pOwner, u32 in_uiVertexCount, u32 in_uiVertexMask, EPrimitiveType in_ePrimitiveType, void* in_pVBData = nullptr, u32 in_eBufferType = eUT_Constant);
	virtual ~VertexBuffer();

	u32 GetVertexCount() const { return m_uiVertexCount; }
	virtual void SetVertexCount(u32 in_uiVertexCount);

	u32 GetVertexSize() const { return m_uiVertexSize; }
	
	virtual void* Lock() = 0;
	virtual void Unlock() = 0;

	virtual void Bind() const = 0;

protected:
	void CalculateVertexSize();

	u32 m_uiVertexCount = 0;
	u32 m_uiVertexMask = 0;
	u32 m_uiVertexSize = 0;
	EPrimitiveType m_ePrimitiveType = EPrimitiveType::ePT_TriangleList;
	
	u32 m_uiBufferType = eUT_Constant;
};
