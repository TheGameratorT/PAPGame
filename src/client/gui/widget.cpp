#include "widget.hpp"

mGUI_BEGIN

Widget::Widget() :
	m_parent(nullptr),
	m_visible(true)
{}

void Widget::update() {}
void Widget::render() {}
void Widget::onClick() {}

mGUI_END
