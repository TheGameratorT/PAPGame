#include "filesystem/path.hpp"

#include "game.hpp"

#include "os/messagebox.hpp"

u32 arcMain(const std::vector<std::string>& args)
{
	Path::setAnnotatedPathPrefix('@', std::string(ARC_PATH_ROOT) + "assets/");
	Path::setAnnotatedPathPrefix('#', std::string(ARC_PATH_ROOT));
	Path::setCurrentWorkingDirectory(Path::getApplicationDirectory());

	try
	{
		if (!Game::init())
		{
			Log::error("Game", "Initialization failed.");
			return 1;
		}
		Game::run();
		Game::destroy();
	}
	catch (const std::exception& ex)
	{
		const char* exMsg = ex.what();

		Log::error("Game", "======== EXCEPTION ========");
		Log::error("Game", exMsg);
		Log::error("Game", "Aborting execution...");
		Log::error("Game", "===========================");

		MessageBox msgBox;
		msgBox.setTitle("Game Runtime Error");
		msgBox.setText(exMsg);
		msgBox.setIcon(MessageBox::Icon::Error);
		msgBox.show();
	}

	return 0;
}
