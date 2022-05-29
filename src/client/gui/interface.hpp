#pragma once

#include <vector>

#include "common.hpp"
#include "iwidget.hpp"
#include "input/keydefs.hpp"
#include "input/keyhandle.hpp"

mGUI_BEGIN

class Interface
{
public:
	void init();
	void destroy();

	void addWidget(IWidget& widget);
	void removeWidget(IWidget& widget);
	void update();
	void render();
	void onMouseClick(Key key, KeyState state);

private:
	std::vector<IWidget*> m_widgets;
	KeyHandle m_leftClick;
	KeyHandle m_rightClick;
};

mGUI_END
