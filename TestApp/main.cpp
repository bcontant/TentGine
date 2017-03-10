#include <windows.h>
#include <stdio.h>
#include <algorithm>

#include "../Base/Assert.h"
#include "../Base/Logger.h"
#include "../Base/Profiler.h"
#include "../Base/Path.h"
#include "../Base/BitmapData.h"
#include "../Base/TGAFile.h"
#include "../Base/PNGFile.h"

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

	BitmapData* pPNGTest = LoadPNG(L("../../data/red.png"));
	SavePNG(L("../../data/TEST.png"), pPNGTest);
	//pPNGTest->ConvertTo(BitmapData::eBF_RGB_U24);
	SaveTGA(L("../../data/pngtest.tga"), pPNGTest, false);

	OS::BuildFont(L("../../data/Comic.ttf"), 72, 1024, OS::eFF_ForceAutoHint/* | OS::eFF_Mono*/);
					
	Window* pWindow = Window::Create(1024, 1024, false, hInstance);

	//BitmapData* pImage = LoadTGA(L("../../data/8bit_Grayscale_Uncompressed.tga"));
	BitmapData* pImage = LoadTGA(L("../../data/red16bit.tga"));
	pImage->ConvertTo(BufferFormat::RGB_U24);
	pImage->ConvertTo(BufferFormat::ARGB_U32);
	SaveTGA(L("../../data/TEST.tga"), pImage, true);
	pImage = LoadTGA(L("../../data/TEST.tga"));
	SaveTGA(L("../../data/TEST2.tga"), pImage, false);
	
	//OS::GetDisplayAdapters();
	
	Renderer* pRenderer = Renderer::CreateRenderer(nullptr, pWindow); 
	Texture* pTexture1 = pRenderer->CreateTexture(L("../../data/stuff.gif"));
	Texture* pTexture2 = pRenderer->CreateTexture(L("../../data/papa.jpg"));
	Texture* pTexture3 = pRenderer->CreateTexture(L("../../data/red.png"));

	Font* pFont = pRenderer->LoadFont(L("Comic Sans MS Regular 72pt.font"));

	Quad* pQuad = pRenderer->CreateQuad(0.f, 0.f, pTexture2);
	//Quad* pQuad2 = pRenderer->CreateQuad(0.5f, 0.5f, pTexture1);
	Quad* pQuad3 = pRenderer->CreateQuad(0.75f, 0.25f, pTexture3);

	Quad* pQuad2 = pRenderer->CreateQuad(0.f, 0.f, pFont->m_pFontTexture);

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

	Profiler::DestroyInstance();
	Logger::DestroyInstance();
}