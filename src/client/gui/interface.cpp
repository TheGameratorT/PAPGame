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

void Interface::addWidget(IWidget& widget)
{
	m_widgets.push_back(&widget);
}

void Interface::removeWidget(IWidget& widget)
{
	auto it = std::find(m_widgets.begin(), m_widgets.end(), &widget);
	m_widgets.erase(it);
}

void Interface::update()
{
	for (IWidget* widget : m_widgets)
		widget->update();
}

void Interface::render()
{
	std::vector<IWidget*> sorted = m_widgets;
	std::sort(sorted.begin(), sorted.end(), [](IWidget* a, IWidget* b){
		return a->renderPriority < b->renderPriority;
	});

	for (IWidget* widget : sorted)
		widget->render();
}

void Interface::onMouseClick(Key key, KeyState state)
{
	Vec2d cursorPos = Game::getCursorPosition();
	for (IWidget* widget : m_widgets)
	{
		if (widget->clickable && widget->bounds.contains(cursorPos))
		{
			if (!widget->onClick(key, state))
				break;
		}
	}
}

mGUI_END
