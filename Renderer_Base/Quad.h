#pragma once

#include "RendererObject.h"

class Texture;

//--------------------------------------------------------------------------------
class Quad : public RendererObject
{
public:
	Quad(Renderer* pOwner);
	virtual ~Quad();

	virtual void Draw() const = 0;
	void SetTexture(Texture* in_pTexture) { m_pTexture = in_pTexture; }
	virtual void SetPosition(float x, float y);

protected:
	vec2 m_Position;

	Texture* m_pTexture;
	VertexBuffer* m_pVertexBuffer;
};