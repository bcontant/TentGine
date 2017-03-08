#pragma once

class Renderer;

//--------------------------------------------------------------------------------
class RendererObject
{
public:
	RendererObject(Renderer* pOwner) : m_pOwner(pOwner) {}
	virtual ~RendererObject() { m_pOwner = 0; }

protected:
	Renderer* GetOwner() const { return m_pOwner; }

private:
	Renderer* m_pOwner;
};
