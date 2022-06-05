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
}

void Interface::render()
{
	m_container.render();
}

void Interface::onMouseClick(Key key, KeyState state)
{
	m_container.onClick(key, state);
}

mGUI_END
