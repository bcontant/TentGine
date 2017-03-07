#include <windows.h>
#include <stdio.h>
#include <algorithm>

#include "../Base/Assert.h"
#include "../Base/Logger.h"
#include "../Base/Profiler.h"
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

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	Logger::CreateInstance();
	Profiler::CreateInstance();

	CoInitialize(nullptr);

	OS::BuildFont(L("../../data/Comic.ttf"), 72, 1024, OS::eFF_ForceAutoHint | OS::eFF_Mono);
					
	Window* pWindow = Window::Create(1024, 1024, false, hInstance);

	BitmapData* pImage = LoadTGA(L("../../data/fontrect.tga"));
	SaveTGA(L("../../data/TEST.tga"), pImage);
	
	//OS::GetDisplayAdapters();
	
	Renderer* pRenderer = Renderer::CreateRenderer(nullptr, pWindow); 
	Texture* pTexture1 = pRenderer->CreateTexture(L("../../data/stuff.gif"));
	Texture* pTexture2 = pRenderer->CreateTexture(L("../../data/papa.jpg"));

	//Texture* pTexture3 = pRenderer->CreateTexture(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
	
	Font* pFont = pRenderer->LoadFont(L("Comic Sans MS Regular 72pt Mono.font"));

	Quad* pQuad = pRenderer->CreateQuad(0.f, 0.f, pTexture2);
	//Quad* pQuad2 = pRenderer->CreateQuad(0.5f, 0.5f, pTexture1);
	//Quad* pQuad3 = pRenderer->CreateQuad(0.75f, 0.25f, pTexture3);

	Quad* pQuad2 = pRenderer->CreateQuad(0.f, 0.f, pFont->m_pFontTexture);
	
	while(1)
	{
		if (pWindow->ProcessMessages() == false)
			break;
		
		pRenderer->StartFrame();
		pQuad->Draw();
		pQuad2->Draw();
		//pQuad3->Draw();
		pRenderer->EndFrame();
	}

	delete pQuad;
	delete pQuad2;
	//delete pQuad3;
	delete pTexture1;
	delete pTexture2;
	//delete pTexture3;
	delete pFont;
	delete pRenderer;
	delete pWindow;

	Profiler::DestroyInstance();
	Logger::DestroyInstance();
}