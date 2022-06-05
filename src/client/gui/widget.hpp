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
	virtual void update();
	virtual void render();

	virtual void onClick(Key key, KeyState state);

	constexpr void setParent(Widget* parent) { m_parent = parent; }
	[[nodiscard]] constexpr Widget* getParent() const { return m_parent; }

	constexpr void setVisible(bool visible) { m_visible = visible; }
	[[nodiscard]] constexpr bool getVisible() const { return m_visible; }

	constexpr void setZIndex(u32 zindex) { m_zpos = zindex; }
	[[nodiscard]] constexpr u32 getZIndex() const { return m_zpos; }

	constexpr void setBounds(const RectI& bounds) { m_bounds = bounds; }
	[[nodiscard]] constexpr const RectI& getBounds() const { return m_bounds; }

private:
	Widget* m_parent = nullptr;
	bool m_visible = true;
	u32 m_zpos = 1000;
	RectI m_bounds = { 0, 0, 0, 0 };
};

mGUI_END
