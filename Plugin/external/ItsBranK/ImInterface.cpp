#include <pch.h>
#include "ImInterface.h"

namespace ItsBranK
{
	ImInterface::ImInterface(const std::string& title, const std::string& name, std::function<void(std::string, bool)> toggleCallback, bool bShowCursor) :
		WindowTitle(title),
		ShowCursor(bShowCursor),
		WindowName(name),
		Attached(false),
		ToggleCallback(toggleCallback),
		Render(false),
		Focused(false)
	{

	}

	ImInterface::~ImInterface()
	{
		OnDetatch();
	}

	const std::string& ImInterface::GetTitle() const
	{
		return WindowTitle;
	}

	const std::string& ImInterface::GetName() const
	{
		return WindowName;
	}

	bool ImInterface::ShouldShowCursor() const
	{
		return ShowCursor;
	}

	bool ImInterface::IsAttached() const
	{
		return Attached;
	}

	bool ImInterface::ShouldRender()
	{
		if (!Render)
		{
			SetShouldRender(false);
		}

		return (IsAttached() && Render);
	}

	bool* ImInterface::ShouldBegin()
	{
		return &Render;
	}

	bool ImInterface::IsFocused() const
	{
		return Focused;
	}

	void ImInterface::SetAttached(bool bAttached)
	{
		Attached = bAttached;
	}

	void ImInterface::SetShouldRender(bool bShouldRender)
	{
		Render = bShouldRender;

		if (ToggleCallback)
		{
			ToggleCallback(WindowName, Render);
		}
	}

	void ImInterface::SetIsFocused(bool bFocused)
	{
		Focused = bFocused;
	}

	void ImInterface::ToggleRender()
	{
		SetShouldRender(!Render);
	}

	void ImInterface::OnAttach()
	{
		SetAttached(true);
	}

	void ImInterface::OnDetatch()
	{
		if (IsAttached())
		{
			SetAttached(false);
		}
	}

	void ImInterface::OnRender() {}
}