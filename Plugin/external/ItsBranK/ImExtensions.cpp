#include <pch.h>
#include "ImExtensions.h"

namespace ItsBranK::ImExtensions
{
	int32_t Stricmp(const char* s1, const char* s2)
	{
		int32_t d;

		while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
		{
			s1++;
			s2++;
		}

		return d;
	}

	int32_t Strnicmp(const char* s1, const char* s2, int32_t n)
	{
		int32_t d = 0;

		while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
		{
			s1++;
			s2++;
			n--;
		}

		return d;
	}

	char* StrCpy(const char* s)
	{
		IM_ASSERT(s);
		size_t length = strlen(s) + 1;
		void* buffer = malloc(length);
		IM_ASSERT(buffer);
		memcpy_s(buffer, length, s, length);
		return reinterpret_cast<char*>(buffer);
	}

	void Strtrim(char* s)
	{
		char* strEnd = s + strlen(s);

		while (strEnd > s && strEnd[-1] == ' ')
		{
			strEnd--;
			*strEnd = 0;
		}
	}

	void TextColored(const char* text, const ImVec4& textColor)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::TextEx(text);
		ImGui::PopStyleColor();
	}

	void TextStyled(const char* text, const ImVec4& textColor, ImFont* textFont)
	{
		if (textFont) { ImGui::PushFont(textFont); }
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::TextEx(text);
		ImGui::PopStyleColor();
		if (textFont) { ImGui::PopFont(); }
	}

	void HelpMarker(const char* desc, float posScale)
	{
		ImGui::TextDisabled("(?)");

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * posScale);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}