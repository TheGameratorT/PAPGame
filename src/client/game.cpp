#include "game.hpp"

#include <sstream>
#include <memory>
#include <utility>

#include "gameinfo.hpp"

#include "render/gle/gle.hpp"

#include "image/imageio.hpp"
#include "input/inputsystem.hpp"

#include "configfile.hpp"
#include "objects.hpp"
#include "scene/splashscene.hpp"
#include "scene/titlescene.hpp"
#include "scene/lobbyscene.hpp"
#include "render/renderer.hpp"
#include "loadingscreen.hpp"
#include "render/fader.hpp"
#include "font/truetype/loader.hpp"
#include "network/gamenet.hpp"

class KeyHandle_impl
{
public:
	explicit KeyHandle_impl(u32 id, Key key) :
		id(id), key(key)
	{}

	u32 id;
	Key key;
};

struct ScheduledTask
{
	Game::TaskCallback callback;
	float firingTime;
};

namespace Game
{
	Window* window;
	Timer timer;
	InputSystem inputSystem;
	ConfigFile config;
	mGUI::Interface gui;

	Vec2ui framebufferSize;
	Vec2d cursorPosition;

	bool vSync;
	bool fullscreen;
	bool shouldQuitGame;

	u32 tickRate;
	float tickDuration;
	u32 frameRate;
	float frameDuration;

	double lastTickTime;

	float tickDelta;
	float frameDelta;
	float tickAlpha;
	float fpsTimer;
	int currentFps;
	int currentTps;

	int nextSceneID;
	Scene* currentScene;
	std::vector<std::unique_ptr<Object>> objects;
	std::vector<KeyCallback> keyCallbacks;
	std::vector<std::unique_ptr<KeyHandle_impl>> keyHandles;
	std::unordered_map<std::string, std::unique_ptr<TrueType::Font>> loadedFonts;
	std::vector<std::unique_ptr<ScheduledTask>> scheduledTasks;

	U8String playerName;

	bool init()
	{
		GameNet::init();

		config.load("#/config.txt");

		window = new Window();

		WindowConfig windowConfig;
		windowConfig.setResizable(true);

		window->setWindowConfig(windowConfig);
		if (!window->create(640, 480, "Hypestayo | Initializing..."))
		{
			Log::error("Arclight", "Failed to create window");
			return false;
		}
		window->setCentered();

		Image iconImage = ImageIO::load<Pixel::RGBA8>("@/logo.bmp");
		window->setIcon(iconImage);

		Image cursorImage = ImageIO::load<Pixel::RGBA8>("@/cursor.bmp");

		RenderCursor& windowCursor = window->getRenderCursor();
		windowCursor.set(0, cursorImage);
		windowCursor.setCurrent(0);

		window->setFramebufferResizeFunction([](u32 width, u32 height){
			GLE::Framebuffer::setViewport(0, 0, width, height);
			framebufferSize = { width, height }; // save on glfw calls
			Renderer::updateProjection();
		});

		framebufferSize = window->getFramebufferSize();

		Renderer::init();

		inputSystem.createContext(0);
		InputContext& inputContext = inputSystem.getContext(0);
		InputHandler& inputHandler = inputContext.getHandler();
		inputHandler.setKeyListener([](Key key, KeyState state){
			for (auto& keyHandle : keyHandles)
			{
				if (keyHandle->key == key)
					keyCallbacks[keyHandle->id](state);
			}
			gui.onKey(key, state);
			return true;
		});
		inputHandler.setCharListener([](KeyChar chr){
			gui.onKeyChar(chr);
			return true;
		});
		inputHandler.setCursorListener([](double x, double y){
			cursorPosition = { x, y };
			return true;
		});
		inputSystem.enableHeldEvent();
		inputSystem.connect(*window);

		loadFont("arial", "@/arial.ttf");
		loadFont("pixels", "@/PixeloidSans-nR3g1.ttf");
		loadFont("smooth", "@/UDDigiKyokashoN-R.ttf");

		gui.init();
		Fader::init();
		LoadingScreen::init();

		vSync = config.get<bool>("vSync", true);
		vSync ? window->enableVSync() : window->disableVSync();

		setFullscreen(config.get<bool>("fullscreen", false));

		timer.start();
		lastTickTime = 0.0f;

		setTickRate(30);
		setFrameRate(config.get<int>("maxFps", 60));

		bindKey(KeyCode::F10, [](KeyState state){
			if (state == KeyState::Pressed)
			{
				window->setSize(640, 480);
				window->setCentered();
			}
		});

		bindKey(KeyCode::F11, [](KeyState state){
			if (state == KeyState::Pressed)
				setFullscreen(!fullscreen);
		});

		bindKey(KeyCode::F12, [](KeyState state){
			if (state == KeyState::Pressed)
				Game::reload();
		});

		config.save();

		createScene<SplashScene>();

		return true;
	}

	void run()
	{
		double lastTime = getElapsedTime();
		float tickTimer = 0.0f;

		while (!window->closeRequested() && !shouldQuitGame)
		{
			double timeNow = getElapsedTime();
			frameDelta = static_cast<float>(timeNow - lastTime);
			lastTime = timeNow;
			tickTimer += frameDelta;

			Window::pollEvents();

			if (tickRate == 0)
			{
				tickDuration = frameDelta;
				update(timeNow);
				tickAlpha = 1.0f;
				tickTimer = 0.0f;
			}
			else
			{
				while (tickTimer > tickDuration)
				{
					update(timeNow);
					if (tickTimer > (tickDuration * 8)) // max of 8 updates in a run
					{
						std::ostringstream out;
						out << "The game ticker is lagging behind! "
							   "Discarding " << tickTimer << " seconds worth of updates.";
						Log::warn("Ticker", out.str());
						tickTimer = 0.0f;
						break;
					}
					else
					{
						tickTimer -= tickDuration;
					}
				}

				tickAlpha = tickTimer / tickDuration;
			}

			render();

			window->swapBuffers();

			if (!vSync && frameRate != 0)
			{
				double nextFrameTime = timeNow + frameDuration;
				while(getElapsedTime() < nextFrameTime);
			}
		}
	}

	void destroy()
	{
		GameNet::destroy();
		LoadingScreen::destroy();
		Fader::destroy();
		gui.destroy();
		inputSystem.disconnect();
		Renderer::destroy();
		keyCallbacks.clear();
		keyHandles.clear();
		loadedFonts.clear();
		scheduledTasks.clear();
		window->close();
		delete window;
	}

	void reload()
	{
		GameNet::disconnect();
		if (currentScene != nullptr)
		{
			currentScene->onDestroy();
			delete currentScene;
		}
		LoadingScreen::close();
		createScene<TitleScene>();
	}

	void update(double timeNow)
	{
		tickDelta = static_cast<float>(timeNow - lastTickTime);
		lastTickTime = timeNow;

		fpsTimer += tickDuration;
		if (fpsTimer >= (1.0f / 4.0f))
		{
			currentFps = std::lround(1.0f / frameDelta);
			currentTps = std::lround(1.0f / tickDelta);
			window->setTitle(String::format("Hypestayo | FPS: %d | TPS: %d", currentFps, currentTps));
			fpsTimer = 0.0f;
		}

		if (!currentScene->getCreated())
		{
			Fader::beginFading(1.0f, true);
			currentScene->onCreate();
			currentScene->markCreated();
		}

		LoadingScreen::update();

		SizeT scheduledTaskCount = scheduledTasks.size();
		for (SizeT i = scheduledTaskCount - 1; i <= 0; i--)
		{
			ScheduledTask& scheduledTask = *scheduledTasks[i];
			if (getElapsedTime() > scheduledTask.firingTime)
			{
				scheduledTask.callback();
				scheduledTasks.erase(scheduledTasks.begin() + i);
			}
		}

		currentScene->onUpdate();

		for (std::size_t i = 0; i < objects.size(); i++)
		{
			const auto& object = objects[i];
			if (!object->getCreated())
			{
				object->onCreate();
				object->markCreated();
			}
			object->onUpdate();
			if (object->getPendingDestroy())
				objects.erase(objects.begin() + static_cast<int>(i));
		}

		if (currentScene->getPendingDestroy())
		{
			currentScene->onDestroy();
			delete currentScene;
			createScene(nextSceneID);
			nextSceneID = 0;
		}

		gui.update();
		Fader::update();
	}

	void render()
	{
		GLE::setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLE::clear(GLE::ClearBuffer::Color);

		LoadingScreen::render();

		if (currentScene->getCreated())
			currentScene->onRender();

		for (const auto& object : objects)
		{
			if (object->getCreated())
				object->onRender();
		}

		gui.render();
		Fader::render();
	}

	void setTickRate(u32 newTickRate)
	{
		tickRate = newTickRate;
		if (newTickRate != 0) {
			tickDuration = 1.0f / float(newTickRate);
		} else {
			tickDuration = 0.0f;
		}
	}

	void setFrameRate(u32 newFrameRate)
	{
		frameRate = newFrameRate;
		if (newFrameRate != 0) {
			frameDuration = 1.0f / float(newFrameRate);
		} else {
			frameDuration = 0.0f;
		}
	}

	double getElapsedTime()
	{
		return timer.getElapsedTime(Time::Unit::Seconds);
	}

	U8String getClipboard()
	{
		std::string str = window->getClipboardString();
		if (str.empty())
			return {};
		SizeT strSize = str.size();
		Log::info("A", str);
		U8String out;
		out.resizeBytes(strSize);
		std::memcpy(out.data(), str.data(), strSize);
		return out;
	}

	Object* createObject(const ObjectProfile* profile)
	{
		Object* newObj = profile->ctor();
		newObj->onCreate();
		if (newObj->getPendingDestroy())
			return nullptr;
		objects.push_back(std::unique_ptr<Object>(newObj));
		return newObj;
	}

	void switchScene(const ObjectProfile* profile)
	{
		nextSceneID = profile->id;
		if (currentScene != nullptr)
			currentScene->onDestroyRequest();
		Fader::beginFading(1.0f, false);
		Log::info("SceneMgr", std::string("Requested scene change: ") + profile->name);
	}

	Scene* createScene(int sceneID)
	{
		const ObjectProfile* profile = Objects::getProfileByID(sceneID);
		currentScene = Scene::cast(profile->ctor());
		Log::info("SceneMgr", std::string("Created scene: ") + profile->name);
		return currentScene;
	}

	void setFullscreen(bool newValue)
	{
		if (fullscreen == newValue)
			return;

		newValue ? window->setFullscreen(window->getCurrentMonitorID()) : window->setWindowed();
		fullscreen = newValue;

		config.set<bool>("fullscreen", newValue);
		config.save();
	}

	KeyHandle bindKey(Key key, const KeyCallback& callback)
	{
		u32 i = keyCallbacks.size();
		keyCallbacks.push_back(callback);
		auto handle = std::make_unique<KeyHandle_impl>(i, key);
		KeyHandle ptr = handle.get();
		keyHandles.push_back(std::move(handle));
		return ptr;
	}

	void unbindKey(KeyHandle handle)
	{
		u32 i = handle->id;
		keyCallbacks.erase(keyCallbacks.begin() + i);
		keyHandles.erase(keyHandles.begin() + i);
		for (; i < keyCallbacks.size(); i++)
			keyHandles[i]->id = i;
	}

	void loadFont(const std::string& fontName, const Path& fontPath)
	{
		std::vector<u8> fontData;

		File fontFile(fontPath);
		fontFile.open();
		fontData = fontFile.readAll();
		fontFile.close();

		auto font = new TrueType::Font(TrueType::loadFont(fontData));
		loadedFonts.insert({fontName, std::unique_ptr<TrueType::Font>(font)});
	}

	const TrueType::Font& getFont(const std::string& fontName)
	{
		return *loadedFonts.at(fontName);
	}

	void connect(const std::string& address, u16 port)
	{
		GameNet::connect(address, port, [](bool success){
			if (success)
			{
				switchScene<LobbyScene>();
			}
			else
			{
				if (currentScene)
					currentScene->onConnectionLost();
			}
		});
	}

	void schedule(const TaskCallback& callback, u32 delayMs)
	{
		ScheduledTask task;
		task.callback = callback;
		task.firingTime = float(getElapsedTime() + float(delayMs) / 1000.0f);
		scheduledTasks.push_back(std::make_unique<ScheduledTask>(task));
	}

	const U8String& getPlayerName()
	{
		return playerName;
	}

	void setPlayerName(const U8String& name)
	{
		playerName = name;
	}
}
