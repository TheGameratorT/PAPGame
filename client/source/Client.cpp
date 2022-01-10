#include <iostream>
#include <thread>
#include <fstream>

#include <windows.h>

#include "core/GLFW.hpp"
#include "graphics/Window.hpp"

#include "audio/AudioContext.hpp"
#include "audio/AudioDevice.hpp"
#include "audio/AudioSource.hpp"
#include "audio/OGGAudioStream.hpp"
#include "audio/WAVAudioStream.hpp"

int main()
{
	using namespace std::chrono_literals;

	std::ios_base::sync_with_stdio(false);

	AudioDevice audioDevice(nullptr);
	AudioContext audioContext(audioDevice);
	audioContext.makeCurrent();
	AudioSource audioSource = AudioSource::create();

	std::ifstream file("assets/sample.ogg", std::ios_base::binary);
	if (!file.is_open())
	{
		std::cout << "Failed to open file" << std::endl;
		return 1;
	}

	OGGAudioStream audioStream(file);
	audioSource.setStream(audioStream);
	audioSource.play();

	while (audioSource.getState() == AudioSource::State::Playing)
	{
		audioStream.update(audioSource);
		std::this_thread::sleep_for(1s);
		if (audioSource.getState() != AudioSource::State::Playing)
			audioSource.play();
	}

	file.close();
	audioDevice.close();

	/*if (!GLFW::init())
		return 1;

	Window window = Window::create(640, 480, "My Title");

	window.makeContextCurrent();

	while (!window.shouldClose())
	{
		GLFW::pollEvents();
		window.swapBuffers();
	}*/

	/*auto* game = new Game();
	game->run();
	delete game;*/

	//GLFW::terminate();

	return 0;
}
