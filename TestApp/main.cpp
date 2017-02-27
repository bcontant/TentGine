#include <windows.h>
#include <stdio.h>

#include "../Base/Assert.h"
#include "../Base/Logger.h"
#include "../Base/Path.h"
#include "../Base/BitmapData.h"
#include "../Base/TGAFile.h"

#include "../OS_Base/System.h"
#include "../OS_Base/FontBuilder.h"
#include "../OS_Base/Window.h"

#include "../Renderer_Base/Renderer.h"
#include "../Renderer_Base/Font.h"
#include "../Renderer_Base/Quad.h"
#include "../Renderer_Base/Texture.h"


#include "ft2build.h"
#include FT_FREETYPE_H


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Logger::CreateInstance();

	CoInitialize(NULL);

	FT_Library library;
	FT_Error error;
	error = FT_Init_FreeType(&library);

	FT_Face face;
	error = FT_New_Face(library, "../../data/FFF_Tusj.ttf", 0, &face);

	error = FT_Set_Pixel_Sizes(face, 0,	4096);

	error = FT_Load_Char(face, 'W', FT_LOAD_RENDER);
				
	Window* pWindow = Window::Create(1024, 1024, false, hInstance);

	OS::BuildFont(L("Comic Sans MS"), 36.f, 512);

	BitmapData* pData = new BitmapData(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer, BitmapData::eBF_U8);
	SaveTGA("f:\\test.tga", pData);
	delete pData;
	
	//OS::GetDisplayAdapters();

	Renderer* pRenderer = Renderer::CreateRenderer(NULL, pWindow); 
	Texture* pTexture1 = pRenderer->CreateTexture(L("../../data/stuff.gif"));
	Texture* pTexture2 = pRenderer->CreateTexture(L("../../data/papa.jpg"));

	Texture* pTexture3 = pRenderer->CreateTexture(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
	
	Font* pFont = pRenderer->LoadFont(L("Comic Sans MS_36pt.dat"));

	Quad* pQuad = pRenderer->CreateQuad(0.f, 0.f, pTexture2);
	Quad* pQuad2 = pRenderer->CreateQuad(0.5f, 0.5f, pTexture1);
	Quad* pQuad3 = pRenderer->CreateQuad(0.75f, 0.25f, pTexture3);

	//Quad* pQuad2 = pRenderer->CreateQuad(0.5f, 0.5f, pFont->m_pFontTexture);
	
	while(1)
	{
		if (pWindow->ProcessMessages() == false)
			break;
		
		pRenderer->StartFrame();
		pQuad->Draw();
		pQuad2->Draw();
		pQuad3->Draw();
		pRenderer->EndFrame();
	}

	delete pQuad;
	delete pQuad2;
	delete pQuad3;
	delete pTexture1;
	delete pTexture2;
	delete pTexture3;
	delete pFont;
	delete pRenderer;
	delete pWindow;

	error = FT_Done_FreeType(library);

	Logger::DestroyInstance();
}