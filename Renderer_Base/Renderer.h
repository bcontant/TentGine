#pragma once

#include <string>
#include "../Base/Path.h"

class DisplayAdapter;
class Texture;
class Quad;
class Font;
class Text;
class Window;
class BitmapData;

//--------------------------------------------------------------------------------
class Renderer
{
public:
	Renderer(Window* in_pWindow);
	virtual ~Renderer();

	virtual Texture* CreateTexture(const Path& filename) = 0;
	virtual Texture* CreateTexture(const BitmapData* in_pData) = 0;
	virtual Quad* CreateQuad(float posX, float posY, Texture* texture) = 0;
	virtual Text* CreateText(float poxX, float posY, Font* in_pFont, const StdString& in_strText) = 0;
	
	Font* LoadFont(const Path& filename);

	virtual unsigned int GetBackBufferWidth() const = 0;
	virtual unsigned int GetBackBufferHeight() const = 0;

	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

protected:
	Window* m_pWindow;

public:
	static Renderer* CreateRenderer(DisplayAdapter* in_pAdapter, Window* in_pWindow);
};