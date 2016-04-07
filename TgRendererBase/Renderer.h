#pragma once

#include <string>

class BackBuffer;
class DisplayAdapter;
class Texture;
class Quad;
class Font;
class Window;

class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	virtual Texture* CreateTexture(std::wstring filename) = 0;
	virtual Texture* CreateTexture(unsigned int width, unsigned int height, const unsigned char* pBits) = 0;
	virtual Quad* CreateQuad(float posX, float posY, Texture* texture) = 0;
	
	BackBuffer* GetBackBuffer() { return m_pBackBuffer; }

	Font* LoadFont(std::wstring filename);

	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

	BackBuffer* m_pBackBuffer;

public:
	static Renderer* CreateRenderer(DisplayAdapter* adapter, Window* window);
};