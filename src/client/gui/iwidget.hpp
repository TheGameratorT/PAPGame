#pragma once

#include <functional>

#include "common.hpp"
#include "math/vector.hpp"
#include "math/rectangle.hpp"
#include "input/keydefs.hpp"

mGUI_BEGIN

class IWidget
{
public:
	virtual void update() = 0;
	virtual void render() = 0;
	virtual bool onClick(Key key, KeyState state) = 0; // Returns whether to propagate or not

	RectI bounds = {0, 0, 0, 0};
	bool clickable = false;
	u32 renderPriority = 0;
};

mGUI_END
