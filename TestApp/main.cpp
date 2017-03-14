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
#include "../Renderer_Base/Text.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	Logger::CreateInstance();
	Profiler::CreateInstance();

	CoInitialize(nullptr);

	//OS::BuildFont(L("../../data/Comic.ttf"), 36, 1024, OS::eFF_ForceAutoHint /*| OS::eFF_Mono*/, L("../../data/"));
	//OS::BuildFont(L("../../data/FFF_Tusj.ttf"), 72, 1024, OS::eFF_ForceAutoHint/* | OS::eFF_Mono*/, L("../../data/"));
	OS::BuildFont(L("../../data/COURBD.TTF"), 24, 1024, OS::eFF_ForceAutoHint/* | OS::eFF_Mono*/, L("../../data/"));
	//OS::BuildFont(L("../../data/COUR.TTF"), 24, 1024, OS::eFF_ForceAutoHint/* | OS::eFF_Mono*/, L("../../data/"));
	OS::BuildFont(L("../../data/HyliaSerifBeta-Regular.otf"), 48, 1024, OS::eFF_ForceAutoHint/* | OS::eFF_Mono*/, L("../../data/"));
						
	Window* pWindow = Window::Create(1024, 1024, false, hInstance);

	//OS::GetDisplayAdapters();
	
	Renderer* pRenderer = Renderer::CreateRenderer(nullptr, pWindow); 
	Texture* pTexture1 = pRenderer->CreateTexture(L("../../data/stuff.gif"));
	Texture* pTexture2 = pRenderer->CreateTexture(L("../../data/papa.jpg"));
	Texture* pTexture3 = pRenderer->CreateTexture(L("../../data/red.png"));

	Font* pFont = pRenderer->LoadFont(L("../../data/Courier New Bold 24pt.font"));
	Font* pFont2 = pRenderer->LoadFont(L("../../data/Hylia Serif Beta Regular 48pt.font"));

	Quad* pQuad = pRenderer->CreateQuad(0.f, 0.f, pTexture2);
	Quad* pQuad2 = pRenderer->CreateQuad(0.5f, 0.5f, pTexture1);
	Quad* pQuad3 = pRenderer->CreateQuad(0.75f, 0.25f, pTexture3);

	//Quad* pQuad2 = pRenderer->CreateQuad(0.f, 0.f, pFont->m_pFontTexture);

	Text* pText = pRenderer->CreateText(0.0f, 0.0f, pFont, L("XXXXX.XX FPS"));
	Text* pText2 = pRenderer->CreateText(0.2f, 0.2f, pFont2, L("ZELDA Breath of the Wild"));


	__int64 lastTime = OS::GetTickCount();

	while(1)
	{
		if (pWindow->ProcessMessages() == false)
			break;
		
		pRenderer->StartFrame();
		pQuad->Draw();
		//pQuad2->Draw();
		pQuad3->Draw();
		pText->Draw();
		//pText2->Draw();
		pRenderer->EndFrame();

		__int64 time = OS::GetTickCount();
		__int64 delta = time - lastTime;
		lastTime = time;

		pText->SetText(Format(L("%.2f FPS"), 1.0 / (double(delta) / OS::GetTickFrequency())));
	}

	delete pText2;
	delete pText;
	delete pQuad;
	delete pQuad2;
	delete pQuad3;
	delete pTexture1;
	delete pTexture2;
	delete pTexture3;
	delete pFont;
	delete pFont2;
	delete pRenderer;
	delete pWindow;

	Profiler::DestroyInstance();
	Logger::DestroyInstance();
}