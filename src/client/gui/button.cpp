#include "button.hpp"

#include "game.hpp"

mGUI_BEGIN

void Button::update()
{
	Vec2d cursorPos = Game::getCursorPosition();
	if (m_enabled)
	{
		bool isContained = getBounds().contains(cursorPos);
		if (m_beingHovered && !isContained)
		{
			m_beingHovered = false;
		}
		else if (isContained)
		{
			m_beingHovered = true;
		}
	}
	else
	{
		m_beingHeld = false;
		m_beingHovered = false;
	}

	Texture* texture;
	if (m_heldTexture && m_beingHeld)
		texture = m_heldTexture;
	else if (m_hoverTexture && m_beingHovered)
		texture = m_hoverTexture;
	else
		texture = m_texture;
	m_image.setTexture(texture);
}

void Button::render()
{
	m_image.setBounds(getBounds());
	m_image.render();
}

void Button::onKey(Key key, KeyState state)
{
	if (!m_enabled)
		return;

	if (key != MouseCode::Left)
		return;

	Vec2d cursorPos = Game::getCursorPosition();

	bool isContained = getBounds().contains(cursorPos);
	if (m_beingHeld)
	{
		if (state == KeyState::Released)
		{
			m_beingHeld = false;
			if (isContained)
			{
				if (m_mouseClickCallback)
					m_mouseClickCallback();
				return;
			}
		}
	}
	else
	{
		if (state == KeyState::Pressed && isContained)
		{
			Game::getGUI().setFocusedWidget(this);
			m_beingHeld = true;
		}
	}
}

mGUI_END
