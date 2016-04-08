#pragma once

#include <string>
#include "RendererObject.h"

class Texture : public RendererObject
{
public:
	Texture(Renderer* pOwner) : RendererObject(pOwner) {}
	virtual ~Texture() {};

	virtual void Initialize(std::wstring filename) = 0;
	virtual void Initialize(unsigned int width, unsigned int height, const unsigned char* pBits) = 0;

	virtual void Bind(unsigned int startSlot) = 0;

	virtual unsigned int GetWidth() = 0;
	virtual unsigned int GetHeight() = 0;
};
