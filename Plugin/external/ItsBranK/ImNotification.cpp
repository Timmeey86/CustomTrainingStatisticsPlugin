#include <pch.h>
#include "ImNotification.h"
#include "ImExtensions.h"

namespace ItsBranK
{
	ImNotification::ImNotification(const std::string& title, const std::string& name, std::function<void(std::string, bool)> toggleCallback) : ImInterface(title, name, toggleCallback, false) {}

	ImNotification::~ImNotification() { OnDetatch(); }

	void ImNotification::OnAttach()
	{
		WindowPadding = 10.0f;
		WindowOffset = 0.0f;
		Corner = CornerPositions::TopLeft;
		WindowFlags = (ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		DescriptionColor = ImColorMap[TextColors::White];
		DeltaTime = 0.0f;
		CutoffTime = 300.0f;
		BaseAlpha = 0.85f;
		LastAlpha = BaseAlpha;
		FadeOut = false;
		SetAttached(true);
	}

	void ImNotification::OnDetatch()
	{
		if (IsAttached())
		{
			SetAttached(false);
		}
	}

	void ImNotification::OnRender()
	{
		if (ShouldRender())
		{
			if (!FadeOut && (DeltaTime > CutoffTime))
			{
				DeltaTime = 0.0f;
				LastAlpha = BaseAlpha;
				TitleColor.w = LastAlpha;
				DescriptionColor.w = LastAlpha;
				FadeOut = true;
			}
			else if (FadeOut && (LastAlpha < 0.0f))
			{
				DeltaTime = 0.0f;
				LastAlpha = BaseAlpha;
				TitleColor.w = LastAlpha;
				DescriptionColor.w = LastAlpha;
				FadeOut = false;
				SetShouldRender(false);
				return;
			}

			//ImGuiViewport* viewport = ImGui::GetMainViewport();
			//ImVec2 work_pos = viewport->WorkPos;
			//ImVec2 work_size = viewport->WorkSize;
			ImVec2 work_pos = { .0f, .0f };
			ImVec2 work_size = { 1920.0f, 1080.0f }; // TODO Retrieve from game wrapper
			ImVec2 window_pos;
			ImVec2 window_pos_pivot;

			window_pos.x = (Corner == CornerPositions::TopRight || Corner == CornerPositions::BottomRight) ? (work_pos.x + work_size.x - WindowPadding) : (work_pos.x + WindowPadding);
			window_pos.y = (Corner == CornerPositions::BottomRight || Corner == CornerPositions::BottomLeft) ? (work_pos.y + work_size.y - WindowPadding) : (work_pos.y + WindowPadding);
			window_pos_pivot.x = (Corner == CornerPositions::TopRight || Corner == CornerPositions::BottomRight) ? 1.0f : 0.0f;
			window_pos_pivot.y = (Corner == CornerPositions::BottomRight || Corner == CornerPositions::BottomLeft) ? 1.0f : 0.0f;
			window_pos.y += WindowOffset;

			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowSize(ImVec2(325.0f, 75.0f), ImGuiCond_Always);

			if (FadeOut)
			{
				LastAlpha -= (GetRenderTime() / 100.0f);
				ImGui::SetNextWindowBgAlpha(LastAlpha);
			}
			else
			{
				ImGui::SetNextWindowBgAlpha(BaseAlpha);
			}

			if (ImGui::Begin(GetTitle().c_str(), ShouldBegin(), WindowFlags))
			{
				SetIsFocused(ImGui::IsWindowFocused());

				if (ImGui::BeginChild(std::string("###" + GetName() + "_Notification_ScrollRegion").c_str()))
				{
					TitleColor.w = LastAlpha;
					DescriptionColor.w = LastAlpha;
										
					ImExtensions::TextStyled(Title.c_str(), TitleColor, ImFontMap[TextStyles::Bold]);
					ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - 1.0f);
					ImExtensions::TextStyled(Description.c_str(), DescriptionColor, ImFontMap[TextStyles::Regular]);
					ImGui::PopTextWrapPos();
				}
				ImGui::EndChild();

				DeltaTime += GetRenderTime();
			}
			ImGui::End();
		}
	}

	float ImNotification::GetRenderTime() const
	{
		float framerate = ImGui::GetIO().Framerate;
		if (framerate <= 15) { framerate = 4.0f; }
		if (framerate <= 30) { framerate = 2.0f; }
		else if (framerate <= 60) { framerate = 1.0f; }
		else if (framerate <= 120) { framerate = 0.5f; }
		else if (framerate <= 144) { framerate = 0.4167f; }
		else if (framerate <= 240) { framerate = 0.25f; }
		else if (framerate <= 360) { framerate = 0.1667f; }
		else if (framerate <= 480) { framerate = 0.125f; }
		else if (framerate > 480) { framerate = 0.1f; }
		return framerate;
	}

	ImNotification* ImNotification::SetInformation(const std::string& title, const std::string& description, TextColors color, CornerPositions corner)
	{
		SetTitle(title);
		SetDescription(description);
		SetColor(color);
		SetPosition(corner);
		return this;
	}

	ImNotification* ImNotification::SetTitle(const std::string& title)
	{
		Title = title;
		return this;
	}

	ImNotification* ImNotification::SetDescription(const std::string& description)
	{
		Description = description;
		return this;
	}

	ImNotification* ImNotification::SetColor(TextColors color)
	{
		TitleColor = ImColorMap[color];
		TitleColor.w = LastAlpha;
		return this;
	}

	ImNotification* ImNotification::SetPosition(CornerPositions corner)
	{
		Corner = corner;
		return this;
	}

	ImNotification* ImNotification::SetOffset(float offset)
	{
		WindowOffset = offset;
		return this;
	}
}