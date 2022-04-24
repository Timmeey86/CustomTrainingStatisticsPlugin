#pragma once

#include <map>
#include <IMGUI/imgui.h>

// Extracted from https://github.com/CodeRedModding/CodeRed-ImGui

namespace ItsBranK
{
	enum class ImArgumentIds : uint8_t
	{
		IM_None,
		IM_Interfaces
	};

	enum class TextStyles : uint8_t
	{
		Regular,
		Italic,
		Bold,
		END
	};

	enum class TextColors : uint32_t
	{
		Black,
		Grey,
		White,
		Red,
		Orange,
		Yellow,
		Green,
		Blue,
		Aqua,
		Purple,
		Pink,
		END
	};

	enum class CornerPositions : uint8_t
	{
		Custom,
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight
	};

	static std::map<TextColors, ImVec4> ImColorMap = {
		{ TextColors::Black, ImVec4(0.0f, 0.0f, 0.0f, 1.0f) },							// #000000
		{ TextColors::Grey, ImVec4(0.72549f, 0.72549f, 0.72549f, 1.0f) },				// #B9B9B9
		{ TextColors::White, ImVec4(1.0f, 1.0f, 1.0f, 1.0f) },							// #FFFFFF
		{ TextColors::Red, ImVec4(0.95686f, 0.10196f, 0.18431f, 1.0f) },				// #F41A2F
		{ TextColors::Orange, ImVec4(1.0f, 0.55686f, 0.0f, 1.0f) },						// #FF8E00
		{ TextColors::Yellow, ImVec4(0.99215f, 0.96078f, 0.0f, 1.0f) },					// #FDF500
		{ TextColors::Green, ImVec4(0.16470f, 0.99215f, 0.18039f, 1.0f) },				// #2AFD2E
		{ TextColors::Blue, ImVec4(0.12549f, 0.41568f, 1.0f, 1.0f) },					// #206AFF
		{ TextColors::Aqua, ImVec4(0.09019f, 0.90980f, 1.0f, 1.0f) },					// #17E8FF
		{ TextColors::Purple, ImVec4(0.64705f, 0.12941f, 1.0f, 1.0f) },					// #A521FF
		{ TextColors::Pink, ImVec4(1.0f, 0.12941f, 1.0f, 1.0f) },						// #FF21FF
	};

	static std::map<TextStyles, ImFont*> ImFontMap = {
		{ TextStyles::Regular, nullptr },
		{ TextStyles::Italic, nullptr },
		{ TextStyles::Bold, nullptr }
	};
}