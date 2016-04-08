#include "Renderer.h"
#include "BackBuffer.h"
#include "Texture.h"
#include "Quad.h"
#include "Font.h"

#include "../TgOSBase/Window.h"


Renderer::Renderer(Window* in_pWindow)
	:m_pWindow(in_pWindow)
{

}

Renderer::~Renderer()
{
}


//Create Font from a pre-built .font file
Font* Renderer::LoadFont(std::wstring in_Filename)
{
	Font* newFont = new Font(this);
	newFont->Load(in_Filename);
	return newFont;
}

