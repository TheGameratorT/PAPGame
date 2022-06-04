#pragma once

#include <vector>

#include "widget.hpp"

mGUI_BEGIN

class Container : public Widget
{
public:
	void addWidget(Widget& widget);
	void removeWidget(Widget& widget);
	void clear();

	void update() override;
	void render() override;
	void onClick() override;

private:
	std::vector<Widget*> m_children;
};

mGUI_END
