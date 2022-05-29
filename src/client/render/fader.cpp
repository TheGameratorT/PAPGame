#include "fader.hpp"

#include "../game.hpp"

#include "texture.hpp"
#include "renderer.hpp"

enum class FadeState
{
	None = 0,
	WaitingRender,
	InToOut,
	OutToIn
};

namespace Fader
{
	static Texture texture;
	static FadeState fadeState = FadeState::None;
	static float fadeOpacity = 0.0f;
	static float lastFadeOpacity = 0.0f;
	static float fadeDuration = 0.0f;

	void init()
	{
		texture.load("@/fade.png");
	}

	void destroy()
	{
		texture.destroy();
	}

	void update()
	{
		lastFadeOpacity = fadeOpacity;
		if (fadeState == FadeState::InToOut)
		{
			fadeOpacity += fadeDuration * Game::getTickDuration();
			if (fadeOpacity >= 1.0f)
			{
				fadeOpacity = 1.0f;
				fadeState = FadeState::WaitingRender;
			}
		}
		else if (fadeState == FadeState::OutToIn)
		{
			fadeOpacity -= fadeDuration * Game::getTickDuration();
			if (fadeOpacity <= 0.0f)
			{
				fadeOpacity = 0.0f;
				fadeState = FadeState::WaitingRender;
			}
		}
	}

	void render()
	{
		if (fadeState != FadeState::None || fadeOpacity != 0.0f)
		{
			Vec2ui area = Game::getFramebufferSize();
			float lerpedFadeOpacity = Math::lerp(lastFadeOpacity, fadeOpacity, Game::getTickAlpha());
			Renderer::renderTexture(texture, {{0, 0}, area}, {}, lerpedFadeOpacity);
			if (fadeState == FadeState::WaitingRender)
				fadeState = FadeState::None;
		}
	}

	void beginFading(float duration, bool fadeIn)
	{
		fadeDuration = duration;
		if (fadeIn)
		{
			fadeState = FadeState::OutToIn;
			fadeOpacity = 1.0f;
		}
		else
		{
			fadeState = FadeState::InToOut;
			fadeOpacity = 0.0f;
		}
		lastFadeOpacity = fadeOpacity;
	}

	bool hasFadingFinished()
	{
		return fadeState == FadeState::None;
	}

	bool isFullDark()
	{
		return fadeOpacity == 1.0f;
	}
}
