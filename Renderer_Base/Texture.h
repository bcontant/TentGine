#pragma once

#include <string>
#include "RendererObject.h"
#include "../Base/Path.h"

class BitmapData;

enum class EAddressingMode
{
	eWrap,
	eMirror,
	eClamp,
	eBorder
};

enum class EFilteringMode
{
	ePoint,
	eLinear
};

//--------------------------------------------------------------------------------
class Texture : public RendererObject
{
public:
	Texture(Renderer* pOwner) : RendererObject(pOwner) {}
	virtual ~Texture() {};

	virtual void Initialize(const Path& filename) = 0;
	virtual void Initialize(const BitmapData* in_pData) = 0;

	virtual void SetAddressingMode(EAddressingMode in_eMode) { m_eAddressingMode = in_eMode; }
	virtual void SetMinFilter(EFilteringMode in_eMode) { m_eMinFilter = in_eMode; }
	virtual void SetMagFilter(EFilteringMode in_eMode) { m_eMagFilter = in_eMode; }
	virtual void SetMipMapFilter(EFilteringMode in_eMode) { m_eMipFilter = in_eMode; }

	virtual void Bind(u32 startSlot) const = 0;

	virtual u32 GetWidth() const = 0;
	virtual u32 GetHeight() const = 0;

protected:
	EAddressingMode m_eAddressingMode = EAddressingMode::eWrap;
	EFilteringMode m_eMinFilter = EFilteringMode::eLinear;
	EFilteringMode m_eMagFilter = EFilteringMode::eLinear;
	EFilteringMode m_eMipFilter = EFilteringMode::eLinear;
};
