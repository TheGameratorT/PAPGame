#include "interface.hpp"

#include "game.hpp"

mGUI_BEGIN

void Interface::init()
{
	m_caretTexture.load("@/caret.png");
}

void Interface::destroy()
{
	m_caretTexture.destroy();
}

void Interface::update()
{
	m_container.update();
}

void Interface::render()
{
	m_container.render();
}

void Interface::onKey(Key key, KeyState state)
{
	m_container.onKey(key, state);
	if (key == KeyCode::LeftControl)
	{
		if (state == KeyState::Pressed)
			m_ctrlDown = true;
		else if (state == KeyState::Released)
			m_ctrlDown = false;
	}
}

void Interface::onKeyChar(KeyChar chr)
{
	m_container.onKeyChar(chr);
}

bool Interface::isWidgetFocused(Widget* widget)
{
	return widget == m_focusedWidget;
}

mGUI_END
