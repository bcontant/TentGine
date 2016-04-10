#pragma once

#include "d3d11.h"
#include "../Renderer_Base/Texture.h"

class Texture_DX11 : public Texture
{
public:
	Texture_DX11(Renderer* pOwner) : Texture(pOwner) {}
	virtual ~Texture_DX11();

	virtual void Initialize(const Path& filename);
	virtual void Initialize(unsigned int width, unsigned int height, const unsigned char* pBits);

	virtual void Bind(unsigned int startSlot);

	virtual unsigned int GetWidth() const;
	virtual unsigned int GetHeight() const;

private:
	ID3D11Texture2D* pTexture;
	ID3D11ShaderResourceView* pShaderResourceView;
};
