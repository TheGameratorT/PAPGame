#pragma once

#include "window/window.hpp"
#include "render/gui/imgui/window.hpp"
#include "render/gui/imgui/inputtext.hpp"

class InfoGUI
{
public:
	bool init();
	void destroy();

	void update();

	[[nodiscard]] constexpr bool closeRequested() const { return m_closeRequested; }

private:
	bool m_closeRequested = false;
	Window* m_window = nullptr;
	GUI::Window m_conClientsWnd;
	GUI::Window m_lobbyChatWnd;
	GUI::Window m_miscWnd;
	GUI::InputText m_inputText;

	void drawPlayerNames();
	void drawLobbyChat();
};
