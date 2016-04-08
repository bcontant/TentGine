#pragma once

#include "RendererObject.h"

class Texture;

class Quad : public RendererObject
{
public:
	Quad(Renderer* pOwner) : RendererObject(pOwner) {}
	virtual ~Quad() {}

	virtual void Draw() = 0;
	void SetTexture(Texture* in_pTexture) { pTexture = in_pTexture; }
	virtual void SetPosition(float x, float y) = 0;

protected:
	Texture* pTexture;
};