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
	void onKey(Key key, KeyState state);
	void onKeyChar(KeyChar chr);

	[[nodiscard]] constexpr bool getCtrlDown() { return m_ctrlDown; }

private:
	Container m_container;
	bool m_ctrlDown = false;
};

mGUI_END
