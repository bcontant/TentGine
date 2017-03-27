#pragma once

#include <string>
#include "../Base/Path.h"

#include "Texture.h"

class DisplayAdapter;
class Quad;
class Font;
class Text;
class Window;
class BitmapData;
class VertexBuffer;
class Shader;

enum class EPrimitiveType;

//--------------------------------------------------------------------------------
enum EUsageType
{
	eUT_Constant = 0,
	eUT_CPU_Readable = 1 << 0,
	eUT_CPU_Writable = 1 << 1,
	eUT_GPU_Writable = 1 << 2,
};

//--------------------------------------------------------------------------------
class Renderer
{
public:
	Renderer(Window* in_pWindow);
	virtual ~Renderer();

	virtual void Initialize(DisplayAdapter* in_pAdapter);

	virtual Texture* CreateTexture(const Path& filename, EAddressingMode in_eMode = EAddressingMode::eWrap, EFilteringMode in_eMinFilter = EFilteringMode::eLinear, EFilteringMode in_eMagFilter = EFilteringMode::eLinear, EFilteringMode in_eMipMapFilter = EFilteringMode::eLinear) = 0;
	virtual Texture* CreateTexture(const BitmapData* in_pData, EAddressingMode in_eMode = EAddressingMode::eWrap, EFilteringMode in_eMinFilter = EFilteringMode::eLinear, EFilteringMode in_eMagFilter = EFilteringMode::eLinear, EFilteringMode in_eMipMapFilter = EFilteringMode::eLinear) = 0;
	virtual Quad* CreateQuad(float posX, float posY, Texture* texture) = 0;
	virtual Text* CreateText(float poxX, float posY, Font* in_pFont, const std_string& in_strText) = 0;
	virtual Text* CreateText(const rect_f& rc, Font* in_pFont, const std_string& in_strText) = 0;
	
	virtual VertexBuffer* CreateVertexBuffer(u32 in_uiVertexCount, u32 in_uiVertexMask, EPrimitiveType in_ePrimitiveType, void* in_pVBData, u32 in_eBufferType) = 0;
	virtual Shader* CreateShader(const Path& in_shaderFile) = 0;
	
	Font* LoadFont(const Path& filename);

	virtual u32 GetBackBufferWidth() const = 0;
	virtual u32 GetBackBufferHeight() const = 0;

	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

	Shader* GetShader(const std_string& in_ShaderName);

protected:
	void LoadShaders();
	void UnloadShaders();

	Window* m_pWindow;

	std::vector<Shader*> m_vShaders;

public:
	static Renderer* CreateRenderer(DisplayAdapter* in_pAdapter, Window* in_pWindow);
};