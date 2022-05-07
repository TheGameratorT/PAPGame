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

#include "network/packet.hpp"
#include "network/packetlist.hpp"
#include "network/clientconnection.hpp"
#include "network/packet/pkt_c2s_handshake.hpp"
#include "network/connectedclient.hpp"
#include "network/server.hpp"

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

namespace Game
{
	Window* window;
	Timer timer;
	InputSystem inputSystem;
	ConfigFile config;

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

	static auto packetListC2S = PacketList::create<
	    PKT_C2S_Handshake
	>();

	static auto packetListS2C = PacketList::create<
		PKT_C2S_Handshake
	>();

	PacketListener listener;
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

		connection->setOutgoingPackets(packetListC2S);
		connection->setIncomingPackets(packetListS2C);
		connection->setIncomingPacketListener(listener);
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
		if (!window->create(640, 480, "PAP Game"))
		{
			Log::error("Arclight", "Failed to create window");
			return false;
		}
		window->setCentered();

		Image iconImage = ImageIO::load<Pixel::RGBA8>("@/logo.bmp");
		iconImage.flipY();
		window->setIcon(iconImage);

		window->getRenderCursor();

		Image cursorImage = ImageIO::load<Pixel::RGBA8>("@/cursor.bmp");
		cursorImage.flipY();

		window->setFramebufferResizeFunction([](u32 width, u32 height){
			GLE::Framebuffer::setViewport(0, 0, width, height);
		});

		RenderCursor& windowCursor = window->getRenderCursor();
		windowCursor.set(0, cursorImage);
		windowCursor.setCurrent(0);

		Renderer::init();

		inputSystem.createContext(0);

		InputContext& inputContext = inputSystem.getContext(0);
		inputContext.addAction(0, KeyTrigger({KeyCode::F11}, KeyState::Pressed), false);
		inputContext.addAction(1, KeyTrigger({KeyCode::F10}, KeyState::Pressed), false);
		inputContext.addAction(2, KeyTrigger({KeyCode::F9}, KeyState::Pressed), false);
		inputContext.addAction(3, KeyTrigger({KeyCode::F8}, KeyState::Pressed), false);
		inputContext.registerAction(0, 0);
		inputContext.registerAction(0, 1);
		inputContext.registerAction(0, 2);
		inputContext.registerAction(0, 3);

		InputHandler& inputHandler = inputContext.getHandler();
		inputHandler.setActionListener([](KeyAction action){
			if (action == 0) {
				setFullscreen(!fullscreen);
			} else if (action == 1) {
				window->setCentered();
			} else if (action == 2) {
				ArcDebug() << "Window Coords:" << window->getPosition();
			} else if (action == 3) {
				connerr = 0;
				connection->cancelConnect();
				connection->clearOutgoingPacketQueue();
				connection->connect("localhost", 25565);
				connection->sendPacket<PKT_C2S_Handshake>(GameInfo::handshakeMagic, GameInfo::version);
			}
			return true;
		});

		inputSystem.connect(*window);

		vSync = config.get<bool>("vSync", true);
		vSync ? window->enableVSync() : window->disableVSync();

		setFullscreen(config.get<bool>("fullscreen", false));

		timer.start();
		lastTickTime = 0.0f;

		setTickRate(30);
		setFrameRate(config.get<int>("maxFps", 60));

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

		inputSystem.disconnect();
		Renderer::destroy();
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
			oss << "PAP Game | FPS: " << currentFps << " | TPS: " << currentTps;
			window->setTitle(oss.str());
			fpsTimer = 0.0f;
		}

		if (!currentScene->getCreated())
		{
			currentScene->onCreate();
			currentScene->markCreated();
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
	}

	void render()
	{
		GLE::setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLE::clear(GLE::ClearBuffer::Color);

		if (currentScene->getCreated())
			currentScene->onRender();

		for (const auto& object : objects)
		{
			if (object->getCreated())
				object->onRender();
		}
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
}
