#include <windows.h>
#include <stdio.h>

#include "../Base/Assert.h"
#include "../Base/Logger.h"
#include "../Base/Path.h"

#include "../OS_Base/System.h"
#include "../OS_Base/FontBuilder.h"
#include "../OS_Base/Window.h"

#include "../Renderer_Base/Renderer.h"
#include "../Renderer_Base/Font.h"
#include "../Renderer_Base/Quad.h"
#include "../Renderer_Base/Texture.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Logger::CreateInstance();

	CoInitialize(NULL);
		
	Window* pWindow = Window::Create(1024, 1024, false, hInstance);

	OS::BuildFont(L("Comic Sans MS"), 36.f, 512);

	//OS::GetDisplayAdapters();

	Renderer* pRenderer = Renderer::CreateRenderer(NULL, pWindow); 
	Texture* pTexture1 = pRenderer->CreateTexture(L("../../data/stuff.gif"));
	Texture* pTexture2 = pRenderer->CreateTexture(L("../../data/papa.jpg"));

	Font* pFont = pRenderer->LoadFont(L("Comic Sans MS_36pt.dat"));

	Quad* pQuad = pRenderer->CreateQuad(0.f, 0.f, pTexture2);
	Quad* pQuad2 = pRenderer->CreateQuad(0.5f, 0.5f, pTexture1);

	//Quad* pQuad2 = pRenderer->CreateQuad(0.5f, 0.5f, pFont->m_pFontTexture);
	
	while(1)
	{
		if (pWindow->ProcessMessages() == false)
			break;
		
		pRenderer->StartFrame();
		pQuad->Draw();
		pQuad2->Draw();
		pRenderer->EndFrame();
	} 

	delete pQuad;
	delete pQuad2;
	delete pTexture1;
	delete pTexture2;
	delete pFont;
	delete pRenderer;
	delete pWindow;

	Logger::DestroyInstance();
}