#pragma once

#include <vector>

#include "common.hpp"
#include "widget.hpp"
#include "container.hpp"
#include "input/keydefs.hpp"
#include "input/keyhandle.hpp"
#include "render/texture.hpp"

mGUI_BEGIN

class Interface
{
public:
	void init();
	void destroy();

	void update();
	void render();
	void onKey(Key key, KeyState state);
	void onKeyChar(KeyChar chr);

	[[nodiscard]] constexpr Container& getContainer() { return m_container; }

	[[nodiscard]] constexpr Texture& getCaretTexture() { return m_caretTexture; }

	constexpr void setFocusedWidget(Widget* widget) { m_focusedWidget = widget; }
	[[nodiscard]] constexpr Widget* getFocusedWidget() const { return m_focusedWidget; }

	bool isWidgetFocused(Widget* widget);

	[[nodiscard]] constexpr bool getCtrlDown() const { return m_ctrlDown; }

private:
	Texture m_caretTexture;
	Container m_container;
	Widget* m_focusedWidget = nullptr;
	bool m_ctrlDown = false;
};

mGUI_END
