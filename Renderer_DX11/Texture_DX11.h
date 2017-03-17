#pragma once

#include "../Renderer_Base/Texture.h"

//--------------------------------------------------------------------------------
class Texture_DX11 : public Texture
{
public:
	Texture_DX11(Renderer* pOwner);
	virtual ~Texture_DX11();

	virtual void Initialize(const Path& filename) override;
	virtual void Initialize(const BitmapData* in_pData) override;

	virtual void Bind(u32 startSlot) const override;

	virtual u32 GetWidth() const override;
	virtual u32 GetHeight() const override;

private:
	void CreateTextureSampler();

	ID3D11Texture2D* m_pTexture = nullptr;
	ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
	ID3D11SamplerState* m_pDefaultSampler = nullptr;
};
