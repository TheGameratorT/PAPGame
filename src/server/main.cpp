#include "filesystem/path.hpp"

#include <utility>

#include "netplayer.hpp"
#include "connectionmanager.hpp"

#include "util/log.hpp"
#include "os/messagebox.hpp"

#include "window/window.hpp"
#include "render/gle/render.hpp"
#include "render/gui/imgui/window.hpp"
#include "render/gui/imgui/label.hpp"
#include "image/imageio.hpp"

static std::vector<std::unique_ptr<NetPlayer>> m_players;

static std::string utf8_to_str(const U8String& str)
{
	std::string out;
	out = reinterpret_cast<const char*>(str.data());
	return out;
}

u32 arcMain(const std::vector<std::string>& args)
{
	using namespace std::chrono_literals;

	Path::setAnnotatedPathPrefix('@', std::string(ARC_PATH_ROOT) + "assets/");
	Path::setAnnotatedPathPrefix('#', std::string(ARC_PATH_ROOT));
	Path::setCurrentWorkingDirectory(Path::getApplicationDirectory());

	auto* window = new Window();
	if (!window->create(640, 480, "Hypestayo - Server"))
	{
		Log::error("App", "Failed to create window.");
		return 1;
	}
	window->enableVSync();

	Image iconImage = ImageIO::load<Pixel::RGBA8>("@/logo.bmp");
	window->setIcon(iconImage);

	GUI::initialize(*window);

	auto* guiWindow = new GUI::Window();
	guiWindow->create("Connected Clients");

	auto* chatWindow = new GUI::Window();
	chatWindow->create("Game Chat");

	try
	{
		ConnectionManager connMgr;
		connMgr.startServer(25565);

		connMgr.setConnectionListener([guiWindow](const ConnectedClientHandle& handle, const U8String& playerName){
			Network::ConnectedClientPtr connection = handle.getConnection();

			ArcDebug() << playerName;

			auto* netPlayer = new NetPlayer();
			m_players.emplace_back(netPlayer);

			netPlayer->m_connHandle = &handle;
			netPlayer->m_guiLabel = std::make_unique<GUI::Label>();
			netPlayer->m_guiLabel->create(
				String::format("%d: %s | %s", handle.getID(), connection->getAddress(), utf8_to_str(playerName))
			);
			netPlayer->m_guiLabelHandle = guiWindow->addControl(*netPlayer->m_guiLabel);

			Log::info("Server", "Got connection from address: " + connection->getAddress());
		});

		connMgr.setDisconnectionListener([guiWindow](const ConnectedClientHandle& handle){
			Log::info("Server", "Client with ID " + std::to_string(handle.getID()) + " disconnected");

			auto it = std::find_if(
				m_players.begin(),
				m_players.end(),
				[&](const std::unique_ptr<NetPlayer>& e){
					return &e->getConnectionHandle() == &handle;
				}
			);

			guiWindow->removeControl(it->get()->m_guiLabelHandle);
			m_players.erase(it);
		});

		while (!window->closeRequested())
		{
			Window::pollEvents();

			GLE::setClearColor(0.0, 0.0, 0.0, 1.0);
			GLE::clear(GLE::ClearBuffer::Color);

			GUI::prepareRender();
			guiWindow->update();
			chatWindow->update();
			GUI::render();

			window->swapBuffers();
		}

		connMgr.stopServer();
	}
	catch (const std::exception& ex)
	{
		const char* exMsg = ex.what();

		Log::error("Server", "======== EXCEPTION ========");
		Log::error("Server", exMsg);
		Log::error("Server", "Aborting exection...");
		Log::error("Server", "===========================");

		MessageBox msgBox;
		msgBox.setTitle("Server Runtime Error");
		msgBox.setText(exMsg);
		msgBox.setIcon(MessageBox::Icon::Error);
		msgBox.show();
	}

	return 0;
}
