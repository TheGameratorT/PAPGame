#include "filesystem/path.hpp"

#include "server.hpp"

#include "util/log.hpp"
#include "os/messagebox.hpp"

u32 arcMain(const std::vector<std::string>& args)
{
	Path::setAnnotatedPathPrefix('@', std::string(ARC_PATH_ROOT) + "assets/");
	Path::setAnnotatedPathPrefix('#', std::string(ARC_PATH_ROOT));
	Path::setCurrentWorkingDirectory(Path::getApplicationDirectory());

	try
	{
		if (Server::init())
		{
			Server::run();
			Server::destroy();
		}
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
