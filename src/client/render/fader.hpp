#pragma once

namespace Fader
{
	void init();
	void destroy();
	void update();
	void render();
	void beginFading(float duration, bool fadeIn);
	bool hasFadingFinished();
	bool isFullDark();
}
