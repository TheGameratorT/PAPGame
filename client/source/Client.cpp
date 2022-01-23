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

int main()
{
	using namespace std::chrono_literals;

	std::ios_base::sync_with_stdio(false);

	try {
		AudioDevice audioDevice = AudioDevice::open(nullptr);
		AudioContext audioContext = AudioContext::create(audioDevice);
		audioContext.makeCurrent();
		AudioSource audioSource = AudioSource::generate();

		std::ifstream ifs1("assets/sample.ogg", std::ios_base::binary);
		if (!ifs1.is_open()) {
			throw std::runtime_error("error opening audio file");
		}

		AudioData audioData = OGGAudio::readAll(ifs1);

		AudioDataSupplier audioSupplier1 = audioData.makeSupplier();
		OGGAudioStreamSupplier audioSupplier2 = OGGAudio::makeStreamSupplier(ifs1);

		audioSupplier2.attach(audioSource);
		audioSupplier2.setup();

		audioSource.play();

		while (audioSource.getState() == AudioSource::State::Playing)
		{
			audioSupplier2.supply();
			std::this_thread::sleep_for(1ms);
		}

		audioSupplier2.destroy();

		ifs1.close();

		audioSource.destroy();
		AudioContext::clearCurrent();
		audioContext.destroy();
		audioDevice.close();
	} catch (const std::exception& ex) {
		std::cout << "You suck at coding because:\n" << ex.what() << std::endl;
	}

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
