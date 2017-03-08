#pragma once

#include <string>
#include "RendererObject.h"
#include "../Base/Path.h"

//--------------------------------------------------------------------------------
class Texture : public RendererObject
{
public:
	Texture(Renderer* pOwner) : RendererObject(pOwner) {}
	virtual ~Texture() {};

	virtual void Initialize(const Path& filename) = 0;
	virtual void Initialize(unsigned int width, unsigned int height, const unsigned char* pBits) = 0;

	virtual void Bind(unsigned int startSlot) = 0;

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;
};
