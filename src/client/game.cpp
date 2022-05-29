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
#include "render/renderer.hpp"
#include "loadingscreen.hpp"
#include "render/fader.hpp"

#include "network/packet.hpp"
#include "network/packetlist.hpp"
#include "network/clientconnection.hpp"
#include "network/packet/pkt_c2s_handshake.hpp"
#include "network/connectedclient.hpp"
#include "network/server.hpp"
#include "network/packettable.hpp"

#include <GLFW/glfw3.h>
using namespace Network;

#define PKT_LSTNR_JUMP(id, type, func) case id: func(static_cast<const type&>(packet)); break

class PacketListener
{
public:
	void operator()(PacketID id, const Packet& packet)
	{
		switch(id)
		{
		PKT_LSTNR_JUMP(0, PKT_C2S_Handshake, onHandshake);
		}
	}

	void onHandshake(const PKT_C2S_Handshake& packet)
	{
	}
};

class KeyHandle_impl
{
public:
	explicit KeyHandle_impl(u32 id, Key key) :
		id(id), key(key)
	{}

	u32 id;
	Key key;
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

	PacketListener pktListener;
	ClientConnection* connection;
	int connerr;

	ClientConnection* getConn()
	{
		return connection;
	}

	int gotConnErr()
	{
		return connerr;
	}

	bool init()
	{
		connerr = 0;

		connection = new ClientConnection();

		connection->setOutgoingPackets(PacketTable::packetsC2S);
		connection->setIncomingPackets(PacketTable::packetsS2C);
		connection->setIncomingPacketListener(pktListener);
		connection->setErrorHandler([&](const ConnectionError& error){
			std::ostringstream out;
			switch (error.value())
			{
			case ConnectionError::ConnectionFailed: {
				if (ClientConnection::checkConnectCanceled(error.getCode())) {
					out << "Error connecting to server. (Reason: Cancelled by user)";
				} else {
					out << "Error connecting to server. (Code: " << error.getCode() << ")";
				}
			} break;
			case ConnectionError::SendPacketUnlisted: {
				out << "Error trying to send packet, " <<
				error.arg<PacketProfile>()->name << " was not registered.";
			} break;
			case ConnectionError::SendPacketWrite: {
				out << "Error writing outgoing packet. (Code: " << error.getCode() << ")";
			} break;
			case ConnectionError::ReadPacketHeader: {
				out << "Error reading incoming packet header. (Code: " << error.getCode() << ")";
			} break;
			case ConnectionError::ReadPacketBody: {
				out << "Error reading incoming packet body. (Code: " << error.getCode() << ")";
			} break;
			case ConnectionError::ReadPacketInvalid: {
				out << "Error identifying incoming packet. (Packet ID: " << *error.arg<u32>() << ")";
			} break;
			case ConnectionError::ReadPacketBufferExceeded: {
				auto* info = error.arg<PacketReceiveBufExceedInfo>();
				out << "Error reading incoming packet body, "
					   "packet exceeds buffer size. (ID: "
					<< info->pktID << ", Size: " << info->pktSize << ")";
			} break;
			}
			/*if (error.category() == NetworkConnectionError::Category::ReadPacket)
				connection.sendPacket<PKT_S2C_Disconnect>(out.str());*/
			Log::error("GameNetwork", out.str());
			connection->close();
			connerr = 1;
		});
		connection->connect("localhost", 25565);
		connection->setConnectHandler([](){
			Log::info("NetworkConnection", "Connection estabilished.");
		});
		connection->setDisconnectHandler([](){
			Log::info("NetworkConnection", "Connection lost.");
			connerr = 2;
		});
		connection->sendPacket<PKT_C2S_Handshake>(GameInfo::handshakeMagic, GameInfo::version);

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
			return true;
		});
		inputHandler.setCursorListener([](double x, double y){
			cursorPosition = { x, y };
			return true;
		});
		inputSystem.connect(*window);

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
		connection->close();
		delete connection;

		LoadingScreen::destroy();
		Fader::destroy();
		gui.destroy();
		inputSystem.disconnect();
		Renderer::destroy();
		keyCallbacks.clear();
		keyHandles.clear();
		window->close();
		delete window;
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
			std::ostringstream oss;
			oss << "Hypestayo | FPS: " << currentFps << " | TPS: " << currentTps;
			window->setTitle(oss.str());
			fpsTimer = 0.0f;
		}

		if (!currentScene->getCreated())
		{
			Fader::beginFading(1.0f, true);
			currentScene->onCreate();
			currentScene->markCreated();
		}

		LoadingScreen::update();

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
}