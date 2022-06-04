#pragma once

#include <functional>

#include "common.hpp"
#include "math/vector.hpp"
#include "math/rectangle.hpp"
#include "input/keydefs.hpp"

mGUI_BEGIN

class Widget
{
public:
	Widget();

	virtual void update();
	virtual void render();
	virtual void onClick();

	constexpr void setParent(Widget* parent) { m_parent = parent; }
	[[nodiscard]] constexpr Widget* getParent() const { return m_parent; }

	constexpr void setVisible(bool visible) { m_visible = visible; }
	[[nodiscard]] constexpr bool getVisible() const { return m_visible; }

	constexpr void setRenderPriority(u32 priority) { m_renderPriority = priority; }
	[[nodiscard]] constexpr u32 getRenderPriority() const { return m_renderPriority; }

	RectI bounds = {0, 0, 0, 0};
	RectI clickBounds = {0, 0, 0, 0};
	bool clickable = true;
	bool hoverable = true;
	bool beingHovered = false;
	bool beingHeld = false;

private:
	Widget* m_parent;
	bool m_visible;
	u32 m_renderPriority = 1000;
};

mGUI_END
