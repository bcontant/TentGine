#pragma once

#include "../Renderer_Base/Text.h"

class Text_DX11 : public Text
{
public:
	Text_DX11(Renderer* pOwner);
	virtual ~Text_DX11();

	virtual void Draw();

protected:
	virtual void UpdateText() override;

	ID3D11Buffer* pVertexBuffer = nullptr;
	ID3D11Buffer* pConstantBuffer = nullptr;
	ID3D11SamplerState* pDefaultSampler = nullptr;
	ID3D11InputLayout* pInputLayout = nullptr;
};
