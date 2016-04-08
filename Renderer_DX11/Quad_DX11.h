#pragma once

#include "d3d11.h"
#include <DirectXMath.h>

#include "../Renderer_Base/Quad.h"

class Texture;

class Quad_DX11 : public Quad
{
public:
	Quad_DX11(Renderer* pOwner);
	virtual ~Quad_DX11();

	virtual void Draw();
	virtual void SetPosition(float x, float y);

protected:
	DirectX::XMFLOAT2 m_Position;

	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pConstantBuffer;
	ID3D11SamplerState* pDefaultSampler;
	ID3D11InputLayout* pInputLayout;
};