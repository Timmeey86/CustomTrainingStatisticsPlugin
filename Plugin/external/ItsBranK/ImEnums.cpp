#include <pch.h>
#include "ImEnums.h"

namespace ItsBranK
{
	std::map<TextStyles, ImFont*> Test::ImFontMap = {
			   { TextStyles::Regular, nullptr },
			   { TextStyles::Italic, nullptr },
			   { TextStyles::Bold, nullptr }
	};
}