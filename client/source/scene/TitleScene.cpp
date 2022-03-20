#include "TitleScene.hpp"

#include "../Game.hpp"

IMPL_OBJECT_PROFILE(TitleScene);

TitleScene::TitleScene() = default;
TitleScene::~TitleScene() = default;

void TitleScene::create()
{
	GameAudio& audio = Game::getInstance().getAudio();
	audio.playMusic(audio.getMainSource(), "title");
	audio.setLooping(audio.getMainSource(), true);
}

void TitleScene::update()
{

}

void TitleScene::render()
{

}

void TitleScene::destroy()
{

}
