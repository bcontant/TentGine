#pragma once

#include <string>

namespace OS
{
	//Create Font from a system installed Font
	void BuildFont(const StdString& in_FontName, float in_fontSize, unsigned int textureSize);
}