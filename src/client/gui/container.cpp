#include "container.hpp"

#include "game.hpp"

mGUI_BEGIN

void Container::addWidget(Widget& widget)
{
	widget.setParent(this);
	m_children.push_back(&widget);
}

void Container::removeWidget(Widget& widget)
{
	widget.setParent(nullptr);
	auto it = std::find(m_children.begin(), m_children.end(), &widget);
	m_children.erase(it);
}

void Container::clear()
{
	for (Widget* widget : m_children)
		widget->setParent(nullptr);
	m_children.clear();
}

void Container::update()
{
	if (!getVisible())
		return;
	for (Widget* widget : m_children)
	{
		if (widget->getVisible())
			widget->update();
	}
}

void Container::render()
{
	if (!getVisible())
		return;
	std::vector<Widget*> sorted = m_children;
	std::sort(sorted.begin(), sorted.end(), [](Widget* a, Widget* b){
		return a->getZIndex() < b->getZIndex();
	});
	for (Widget* widget : sorted)
	{
		if (widget->getVisible())
			widget->render();
	}
}

void Container::onClick(Key key, KeyState state)
{
	for (Widget* widget : getChildren())
	{
		if (widget->getVisible())
		{
			widget->onClick(key, state);
		}
	}
}

mGUI_END
