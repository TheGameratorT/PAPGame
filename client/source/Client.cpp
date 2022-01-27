#include <iostream>
#include <thread>
#include <fstream>

#include <windows.h>

#include "core/GLFW.hpp"
#include "graphics/Window.hpp"

#include "audio/AudioContext.hpp"
#include "audio/AudioDevice.hpp"
#include "audio/AudioSource.hpp"
#include "audio/AudioSupplier.hpp"
#include "audio/AudioDataSupplier.hpp"
#include "audio/ogg/OGGAudio.hpp"
#include "audio/wav/WAVAudio.hpp"
#include "audio/GameAudio.hpp"

int main()
{
	using namespace std::chrono_literals;

	std::ios_base::sync_with_stdio(false);

	try {
		AudioDevice audioDevice = AudioDevice::open(nullptr);
		AudioContext audioContext = AudioContext::create(audioDevice);
		audioContext.makeCurrent();
		AudioSource audioSource = AudioSource::generate();
		audioSource.setVolume(0.25f);
		GameAudio audio;

		audio.loadMusic("sample1", "assets/sample.ogg", AudioContainer::OGG);
		audio.loadSound("sample2", "assets/susdogdoinbruh.ogg", AudioContainer::OGG);
		audio.playMusic(audioSource, "sample1");

		bool prevCtrlHeld = false;
		bool prevShiftHeld = false;

		while (true)
		{
			bool ctrlHeld = GetKeyState(VK_CONTROL) & 0x8000;
			bool ctrlDown = !prevCtrlHeld && ctrlHeld;
			prevCtrlHeld = ctrlHeld;

			bool shiftHeld = GetKeyState(VK_SHIFT) & 0x8000;
			bool shiftDown = !prevShiftHeld && shiftHeld;
			prevShiftHeld = shiftHeld;

			audio.update();

			if (ctrlDown)
			{
				audio.playMusic(audioSource, "sample1");
				audio.setLooping(audioSource, true);
			}
			else if (shiftDown)
			{
				audio.playSound(audioSource, "sample2");
				audio.setLooping(audioSource, true);
			}

			if (GetKeyState('H') & 0x8000)
				break;

			std::this_thread::sleep_for(1ms);
		}

		audio.destroy();
		audioSource.destroy();
		AudioContext::clearCurrent();
		audioContext.destroy();
		audioDevice.close();
	} catch (const std::exception& ex) {
		std::cout << "You suck at coding because:\n" << ex.what() << std::endl;
	}

	if (!GLFW::init())
		return 1;

	Window window = Window::create(640, 480, "My Title");

	window.makeContextCurrent();

	while (!window.shouldClose())
	{
		GLFW::pollEvents();
		window.swapBuffers();
	}

	/*auto* game = new Game();
	game->run();
	delete game;*/

	GLFW::terminate();

	return 0;
}
