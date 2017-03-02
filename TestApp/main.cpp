#include <windows.h>
#include <stdio.h>
#include <algorithm>

#include "../Base/Assert.h"
#include "../Base/Logger.h"
#include "../Base/Profiler.h"
#include "../Base/Path.h"
#include "../Base/BitmapData.h"
#include "../Base/TGAFile.h"
#include "../Base/GraphicUtils.h"

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
	Profiler::CreateInstance();

	CoInitialize(NULL);

	//Init the string we'll use for creating the FontFileTexture
	wchar_t wszTextureText[512] = {};
	for (int i = 1; i <= 511; i++)
		wszTextureText[i - 1] = i;
	UINT32 cTextureTextLength = (UINT32)wcslen(wszTextureText);

	FT_Library library;
	FT_Error error;
	error = FT_Init_FreeType(&library);

	FT_Face face;
	//error = FT_New_Face(library, "../../data/FFF_Tusj.ttf", 0, &face);
	error = FT_New_Face(library, "../../data/Comic.ttf", 0, &face);
	error = FT_Set_Char_Size(face, 0, 48 * 64, 96, 96);  //This is correct on my (96DPI) display.  Glyph are the proper size (compared with MS Word)

	struct Glyph
	{
		int left;
		int top;
		int advance;
		BitmapData* m_pData;
	};

	struct Character
	{
		wchar_t m_CharCode;
		Glyph* m_pGlyph;
	};

	std::map<unsigned int, Glyph*> mGlyphs;
	std::vector<Character> vChars;
	
	for (int i = 0; i < cTextureTextLength; i++)
	{
		unsigned int glyphIndex = FT_Get_Char_Index(face, wszTextureText[i]);
		error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_FORCE_AUTOHINT);
		error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

		if (error == 0)
		{
			if (mGlyphs.find(glyphIndex) == mGlyphs.end())
			{
				Glyph* newGlyph = new Glyph;
				newGlyph->left = face->glyph->bitmap_left;
				newGlyph->top = face->glyph->bitmap_top;
				newGlyph->advance = face->glyph->advance.x / 64;
				newGlyph->m_pData = new BitmapData(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer, BitmapData::eBF_A_U8);
				mGlyphs[glyphIndex] = newGlyph;
			}

			Character newChar;
			newChar.m_CharCode = wszTextureText[i];
			newChar.m_pGlyph = mGlyphs[glyphIndex];
			vChars.push_back(newChar);
		}
	}

	std::vector<PackRect> vUnpackRects;
	for (auto it = mGlyphs.begin(); it != mGlyphs.end(); it++)
	{
		PackRect rc = { 0, 0, it->second->m_pData->GetWidth(), it->second->m_pData->GetHeight(), it->second };
		vUnpackRects.push_back(rc);
	}

#define PACK_SIZE 512
	auto vPackRects = PackTexture(vUnpackRects, PACK_SIZE, PACK_SIZE);

	BitmapData* pRects = new BitmapData(PACK_SIZE, PACK_SIZE, BitmapData::eBF_A_U8);
	for (int i = 0; i < vPackRects.size(); i++)
	{
		pRects->Blit(vPackRects[i].x, vPackRects[i].y,  ((Glyph*)vPackRects[i].user_data)->m_pData  );
	}
	SaveTGA("f:\\fontrect.tga", pRects);
				
	Window* pWindow = Window::Create(1024, 1024, false, hInstance);

	OS::BuildFont(L("Comic Sans MS"), 48.f, 1024);

	File fontfile;
	fontfile.Open("f:\\fonttest.dat", File::fmWriteOnly);
	unsigned int charCount = unsigned int(vChars.size());
	fontfile.Write(&charCount, sizeof(unsigned int));
	for (int i = 0; i < vChars.size(); i++)
	{
		fontfile.Write(&vChars[i].m_CharCode, sizeof(wchar_t));

		Glyph* pCurrentGlyph = vChars[i].m_pGlyph;
		auto it = std::find_if(vPackRects.begin(), vPackRects.end(), [pCurrentGlyph](const PackRect& a) { return a.user_data == pCurrentGlyph; });

		//UVs
		fontfile.Write(&it->x, sizeof(unsigned int));
		fontfile.Write(&it->y, sizeof(unsigned int));
		fontfile.Write(&it->size_x, sizeof(unsigned int));
		fontfile.Write(&it->size_y, sizeof(unsigned int));

		//Quad top left corner offset from baseline
		fontfile.Write(&pCurrentGlyph->left, sizeof(int));
		fontfile.Write(&pCurrentGlyph->top, sizeof(int));

		//Next character X offset (from previous advance position)
		fontfile.Write(&pCurrentGlyph->advance, sizeof(int));
	}
	fontfile.Close();
	
	//OS::GetDisplayAdapters();


	Renderer* pRenderer = Renderer::CreateRenderer(NULL, pWindow); 
	Texture* pTexture1 = pRenderer->CreateTexture(L("../../data/stuff.gif"));
	Texture* pTexture2 = pRenderer->CreateTexture(L("../../data/papa.jpg"));

	Texture* pTexture3 = pRenderer->CreateTexture(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
	
	Font* pFont = pRenderer->LoadFont(L("Comic Sans MS_48pt.dat"));

	Quad* pQuad = pRenderer->CreateQuad(0.f, 0.f, pTexture2);
	//Quad* pQuad2 = pRenderer->CreateQuad(0.5f, 0.5f, pTexture1);
	Quad* pQuad3 = pRenderer->CreateQuad(0.75f, 0.25f, pTexture3);

	Quad* pQuad2 = pRenderer->CreateQuad(0.f, 0.f, pFont->m_pFontTexture);
	
	int i = 0;
	while(1)
	{
		if (pWindow->ProcessMessages() == false)
			break;
		
		pRenderer->StartFrame();
		pQuad->Draw();
		pQuad2->Draw();
		//pQuad3->Draw();
		pRenderer->EndFrame();

		i++;
	}

	Logger::GetInstance()->Log(eLC_DebugInfo, eLS_Message, eLT_FileAndDebug, "Frames : %d", i);

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

	Profiler::DestroyInstance();
	Logger::DestroyInstance();
}