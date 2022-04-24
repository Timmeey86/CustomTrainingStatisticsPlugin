#pragma once

// Extracted from https://github.com/CodeRedModding/CodeRed-ImGui

#include <string>
#include <functional>

namespace ItsBranK
{
	// Base class for all ImGui interfaces, controls attaching/detaching as well as rendering.
	class ImInterface
	{
	private:
		std::string WindowTitle; // Text to display inside its ImGui window title.
		std::string WindowName;	// Name used internally to call or manage in other places, must be unique.
		bool ShowCursor; // If you should forcefully show the cursor upon opening the window or not, true by default, should be managed by your WndProc hook.
		bool Attached; // If the interface is attached/created or not.
		bool Render; // If the window should be rendered or not inside ImGui.
		bool Focused; // If the window is focused or not inside ImGui.
		std::function<void(std::string, bool)> ToggleCallback; // Callback function that is called when the render status has changed (open/closing).

	public:
		ImInterface(const std::string& title, const std::string& name, std::function<void(std::string, bool)> toggleCallback, bool bShowCursor);
		virtual ~ImInterface();

		const std::string& GetTitle() const;
		const std::string& GetName() const;
		bool ShouldShowCursor() const;
		bool IsAttached() const;
		bool ShouldRender();
		bool* ShouldBegin();
		bool IsFocused() const;
		void SetAttached(bool bAttached);
		void SetShouldRender(bool bShouldRender);
		void SetIsFocused(bool bFocused);
		void ToggleRender();

		virtual void OnAttach(); // Should be called when the interface is first created and hooked to ImGui, do any initializing here like memset or assigning default values.
		virtual void OnDetatch(); // Can be called by the deconstructor or manually, if you have any pointers allocated you should free them here.
		virtual void OnRender(); // Called each render tick, depends on what version of DirectX you're using.
	};
}