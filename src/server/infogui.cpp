#include "infogui.hpp"

#include "image/image.hpp"
#include "image/imageio.hpp"
#include "render/gle/render.hpp"
#include "render/gui/imgui/imgui_impl.hpp"

bool InfoGUI::init()
{
	m_window = new Window();

	if (!m_window->create(640, 480, "Hypestayo - Server"))
	{
		Log::error("App", "Failed to create window.");
		delete m_window;
		m_window = nullptr;
		return false;
	}
	m_window->enableVSync();

	Image iconImage = ImageIO::load<Pixel::RGBA8>("@/logo_server.bmp");
	m_window->setIcon(iconImage);

	GUI::initialize(*m_window);

	m_conClientsWnd.create("Connected Clients");
	m_lobbyChatWnd.create("Game Chat");

	return true;
}

void InfoGUI::destroy()
{
	delete m_window;
}

void InfoGUI::update()
{
	Window::pollEvents();

	GLE::setClearColor(0.0, 0.0, 0.0, 1.0);
	GLE::clear(GLE::ClearBuffer::Color);

	GUI::prepareRender();
	m_conClientsWnd.update();
	m_lobbyChatWnd.update();
	GUI::render();

	m_window->swapBuffers();

	m_closeRequested = m_window->closeRequested();
}
