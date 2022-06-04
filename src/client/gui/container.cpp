#include "container.hpp"

mGUI_BEGIN

void Container::addWidget(Widget& widget)
{
	m_children.push_back(&widget);
}

void Container::removeWidget(Widget& widget)
{
	auto it = std::find(m_children.begin(), m_children.end(), &widget);
	m_children.erase(it);
}

void Container::clear()
{
	m_children.clear();
}

void Container::update()
{
	if (!getVisible())
		return;
	for (Widget* widget : m_children)
		widget->update();
}

void Container::render()
{
	if (!getVisible())
		return;
	std::vector<Widget*> sorted = m_children;
	std::sort(sorted.begin(), sorted.end(), [](Widget* a, Widget* b){
		return a->getRenderPriority() < b->getRenderPriority();
	});
	for (Widget* widget : sorted)
		widget->render();
}

void Container::onClick()
{

}

mGUI_END
