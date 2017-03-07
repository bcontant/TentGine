#pragma once

#include <string>

namespace OS
{
	enum EFontFlags
	{
		eFF_Default = 0,
		eFF_Mono = 1 << 0,
		eFF_ForceAutoHint = 1 << 1,
	};

	void BuildFont(const Path& in_FontFile, unsigned int in_uiFontSize, unsigned int in_uiTextureSize, unsigned int in_eFlags = eFF_Default, const Path& in_FontDataFileName = L(""));
}