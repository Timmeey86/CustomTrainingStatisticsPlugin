#pragma once

#include "ImInterface.h"
#include "ImEnums.h"

#include <IMGUI/imgui.h>

#include <string>

namespace ItsBranK
{

	// Extracted from https://github.com/CodeRedModding/CodeRed-ImGui

	class ImNotification : public ImInterface
	{
	private:
		float WindowPadding;
		float WindowOffset;
		ImGuiWindowFlags WindowFlags;

		CornerPositions Corner;
		std::string Title;
		std::string Description;
		ImVec4 TitleColor;
		ImVec4 DescriptionColor;

		float DeltaTime;
		float CutoffTime;
		float BaseAlpha;
		float LastAlpha;
		bool FadeOut;

	public:
		ImNotification(const std::string& title, const std::string& name, std::function<void(std::string, bool)> toggleCallbac);
		~ImNotification() override;

		void OnAttach() override;
		void OnDetatch() override;
		void OnRender() override;

		float GetRenderTime() const;
		ImNotification* SetInformation(const std::string& title, const std::string& description, TextColors color, CornerPositions corner = CornerPositions::TopLeft);
		ImNotification* SetTitle(const std::string& title);
		ImNotification* SetDescription(const std::string& description);
		ImNotification* SetColor(TextColors color);
		ImNotification* SetPosition(CornerPositions corner);
		ImNotification* SetOffset(float offset);
	};
}