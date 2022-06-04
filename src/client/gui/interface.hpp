#pragma once

#include <vector>

#include "common.hpp"
#include "widget.hpp"
#include "container.hpp"
#include "input/keydefs.hpp"
#include "input/keyhandle.hpp"

mGUI_BEGIN

class Interface
{
public:
	void init();
	void destroy();

	[[nodiscard]] constexpr Container& getContainer() { return m_container; }

	void update();
	void render();
	void onMouseClick(Key key, KeyState state);

private:
	Container m_container;
	KeyHandle m_leftClick;
	KeyHandle m_rightClick;
};

mGUI_END
