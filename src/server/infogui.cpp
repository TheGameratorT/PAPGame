#include "infogui.hpp"

#include "server.hpp"

#include "image/image.hpp"
#include "image/imageio.hpp"
#include "render/gle/render.hpp"
#include "render/gui/imgui/imgui_impl.hpp"
#include "render/gui/imgui/label.hpp"
#include "util.hpp"

bool InfoGUI::init()
{
	m_window = new Window();

	if (!m_window->create(640, 480, "Hypestayo - Server"))
	{
		Log::error("InfoGUI", "Failed to create window.");
		delete m_window;
		m_window = nullptr;
		return false;
	}
	m_window->enableVSync();

	Image iconImage = ImageIO::load<Pixel::RGBA8>("@/logo_server.bmp");
	m_window->setIcon(iconImage);

	GUI::initialize(*m_window);

	m_conClientsWnd.create("Connected Clients");
	m_lobbyChatWnd.create("Lobby Chat");
	m_miscWnd.create("Miscellaneous");

	m_inputText.setLabel("Command Input");
	m_inputText.setOnEnterAction([this](){
		std::string command = m_inputText.getText();
		m_inputText.clear();
		Server::executeCommand(command);
	});
	m_miscWnd.addControl(m_inputText);

	m_tpsLabel.setText("Current TPS: 0");
	m_miscWnd.addControl(m_tpsLabel);

	return true;
}

void InfoGUI::destroy()
{
	delete m_window;
	GUI::destroy();
}

void InfoGUI::update()
{
	Window::pollEvents();

	GLE::setClearColor(0.0, 0.0, 0.0, 1.0);
	GLE::clear(GLE::ClearBuffer::Color);

	GUI::prepareRender();
	drawPlayerNames();
	drawLobbyChat();
	m_tpsLabel.setText("Current TPS: " + std::to_string(Server::getCurrentTPS()));
	m_miscWnd.update();
	GUI::render();

	m_window->swapBuffers();

	m_closeRequested = m_window->closeRequested();
}

void InfoGUI::drawPlayerNames()
{
	auto& players = Server::getPlayers();
	SizeT playerCount = players.size();
	std::vector<GUI::Label> playerLabels(playerCount);
	std::vector<HandleT> playerLabelHandles(playerCount);
	for (SizeT i = 0; i < playerCount; i++)
	{
		auto& player = players[i];
		std::string str = Util::utf8AsString(player->getName()) + " (" + player->getClient()->getAddress() + ")";
		GUI::Label& label = playerLabels[i];
		label.setText(str);
		playerLabelHandles[i] = m_conClientsWnd.addControl(label);
	}
	m_conClientsWnd.update();
	for (SizeT i = 0; i < playerCount; i++)
		m_conClientsWnd.removeControl(playerLabelHandles[i]);
}

void InfoGUI::drawLobbyChat()
{
	auto& msgs = Server::getLobbyChat();
	SizeT msgCount = msgs.size();
	std::vector<GUI::Label> msgLabels(msgCount);
	std::vector<HandleT> msgLabelHandles(msgCount);
	for (SizeT i = 0; i < msgCount; i++)
	{
		auto& msg = msgs[i];
		std::string str = Util::utf8AsString(msg);
		GUI::Label& label = msgLabels[i];
		label.setText(str);
		msgLabelHandles[i] = m_lobbyChatWnd.addControl(label);
	}
	m_lobbyChatWnd.update();
	for (SizeT i = 0; i < msgCount; i++)
		m_lobbyChatWnd.removeControl(msgLabelHandles[i]);
}
