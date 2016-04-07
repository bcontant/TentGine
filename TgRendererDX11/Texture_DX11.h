#pragma once

#include "d3d11.h"
#include "../TgRendererBase/Texture.h"

class Texture_DX11 : public Texture
{
public:
	Texture_DX11(Renderer* pOwner) : Texture(pOwner) {}
	virtual ~Texture_DX11();

	virtual void Initialize(std::wstring filename);
	virtual void Initialize(unsigned int width, unsigned int height, const unsigned char* pBits);

	virtual unsigned int GetWidth();
	virtual unsigned int GetHeight();

	//private:
	ID3D11Texture2D* pTexture;
	ID3D11ShaderResourceView* pShaderResourceView;
};
