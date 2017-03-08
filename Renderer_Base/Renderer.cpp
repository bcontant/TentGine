#include "precompiled.h"

//--------------------------------------------------------------------------------
Renderer::Renderer(Window* in_pWindow)
	:m_pWindow(in_pWindow)
{
}

//--------------------------------------------------------------------------------
Renderer::~Renderer()
{
}

//--------------------------------------------------------------------------------
Font* Renderer::LoadFont(const Path& in_Filename)
{
	Font* newFont = new Font(this);
	newFont->Load(in_Filename);
	return newFont;
}

