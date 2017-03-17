#pragma once

#include "d3d11.h"
#include <DirectXMath.h>

#include "../Renderer_Base/Quad.h"

class VertexBuffer;

//--------------------------------------------------------------------------------
class Quad_DX11 : public Quad
{
public:
	Quad_DX11(Renderer* pOwner);
	virtual ~Quad_DX11();

	virtual void Draw() const override;

protected:
	ID3D11Buffer* pConstantBuffer;
	ID3D11InputLayout* pInputLayout;
};