#include "interface.hpp"

#include "game.hpp"

mGUI_BEGIN

void Interface::init()
{
	m_leftClick = Game::bindKey(MouseCode::Left, [this](KeyState state){onMouseClick(MouseCode::Left, state);});
	m_rightClick = Game::bindKey(MouseCode::Right, [this](KeyState state){onMouseClick(MouseCode::Right, state);});
}

void Interface::destroy()
{
	Game::unbindKey(m_leftClick);
	Game::unbindKey(m_rightClick);
}

void Interface::update()
{
	m_container.update();

	/*Vec2d cursorPos = Game::getCursorPosition();
	for (Widget* widget : m_widgets)
	{
		bool isContained = widget->clickBounds.contains(cursorPos);
		if (widget->beingHovered && !isContained)
		{
			widget->beingHovered = false;
		}
		else if (isContained)
		{
			widget->beingHovered = true;
		}
		widget->update();
	}*/
}

void Interface::render()
{
	m_container.render();
}

void Interface::onMouseClick(Key key, KeyState state)
{
	/*Vec2d cursorPos = Game::getCursorPosition();
	for (Widget* widget : m_widgets)
	{
		if (widget->clickable)
		{
			bool isContained = widget->clickBounds.contains(cursorPos);
			if (widget->beingHeld)
			{
				if (state == KeyState::Released)
				{
					widget->beingHeld = false;
					if (isContained)
					{
						widget->onClick();
						break;
					}
				}
			}
			else
			{
				if (state == KeyState::Pressed && isContained)
					widget->beingHeld = true;
			}
		}
	}*/
}

mGUI_END
