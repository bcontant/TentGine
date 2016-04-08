#pragma once

#include "RendererObject.h"

class BackBuffer : public RendererObject
{
public:
	BackBuffer(Renderer* pOwner) : RendererObject(pOwner) {}
	virtual ~BackBuffer() {};

	virtual unsigned int GetWidth() = 0;
	virtual unsigned int GetHeight() = 0;
};
