#pragma once

#include <string>

namespace OS
{
	//--------------------------------------------------------------------------------
	enum EFontFlags
	{
		eFF_Default = 0,
		eFF_Mono = 1 << 0,
		eFF_ForceAutoHint = 1 << 1,
	};

	//--------------------------------------------------------------------------------
	bool BuildFont(const Path& in_FontFile, u32 in_uiFontSize, u32 in_uiTextureSize, u32 in_eFlags = eFF_Default, const Path& in_FontDataFileName = L(""));
}