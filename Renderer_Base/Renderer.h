#pragma once

#include <string>

class DisplayAdapter;
class Texture;
class Quad;
class Font;
class Window;

class Renderer
{
public:
	Renderer(Window* in_pWindow);
	virtual ~Renderer();

	virtual Texture* CreateTexture(std::wstring filename) = 0;
	virtual Texture* CreateTexture(unsigned int width, unsigned int height, const unsigned char* pBits) = 0;
	virtual Quad* CreateQuad(float posX, float posY, Texture* texture) = 0;
	
	Font* LoadFont(std::wstring filename);

	virtual unsigned int GetBackBufferWidth() const = 0;
	virtual unsigned int GetBackBufferHeight() const = 0;

	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

protected:
	Window* m_pWindow;

public:
	static Renderer* CreateRenderer(DisplayAdapter* in_pAdapter, Window* in_pWindow);
};