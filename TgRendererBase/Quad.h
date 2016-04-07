#pragma once

#include "RendererObject.h"

class Texture;

class Quad : public RendererObject
{
public:
	Quad(Renderer* pOwner) : RendererObject(pOwner) {}
	virtual ~Quad() {}

	virtual void Draw() = 0;

	Texture* pTexture;
};