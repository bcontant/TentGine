#pragma once

#include "../Renderer_Base/VertexBuffer.h"

class VertexBuffer_DX11 : public VertexBuffer
{
public:
	VertexBuffer_DX11(Renderer* in_pOwner, u32 in_uiVertexCount, u32 in_uiVertexMask, EPrimitiveType in_ePrimitiveType, void* in_pVBData, u32 in_eBufferType);
	virtual ~VertexBuffer_DX11();

	virtual void* Lock() override;
	virtual void Unlock() override;

	virtual void Bind() const override;

	virtual void SetVertexCount(u32 in_uiVertexCount) override;

private:
	void Init(const void* in_pData);

	D3D_PRIMITIVE_TOPOLOGY GetD3DPrimitiveTopology() const;

	ID3D11Buffer* m_pD3D11VertexBuffer = nullptr;
};
