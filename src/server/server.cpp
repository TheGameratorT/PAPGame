#include "server.hpp"

#include "infogui.hpp"
#include "acceptor.hpp"
#include "network/packet.hpp"

namespace Server {

static Acceptor* acceptor = nullptr;
static InfoGUI* gui = nullptr;

static bool serverRunning = true;
static bool serverStopping = false;
static bool shouldStopServer = false;

std::vector<std::unique_ptr<Player>> players;

bool init()
{
	gui = new InfoGUI();
	if (!gui->init())
	{
		Log::error("GUI", "Initialization failed!");
		delete gui;
		gui = nullptr;
	}

	acceptor = new Acceptor();
	acceptor->start(25565);

	acceptor->setOnClientAccepted([](std::unique_ptr<Player> player){
		auto client = player->getClient();
		Player* playerPtr = player.get();

		client->setIncomingPacketListener([playerPtr](Network::PacketID id, const Network::Packet& packet){
			playerPtr->onPacketReceived(id, packet);
		});

		client->setDisconnectHandler([playerPtr](){
			playerPtr->onDisconnect();
			auto it = std::find_if(players.begin(), players.end(), [playerPtr](const std::unique_ptr<Player>& a){
				return a.get() == playerPtr;
			});
			players.erase(it);
		});

		players.push_back(std::move(player));

		playerPtr->onConnect();
	});

	return true;
}

void destroy()
{
	// TODO: Check if needs safe disconnect
	players.clear();

	delete acceptor;

	if (gui != nullptr)
	{
		gui->destroy();
		delete gui;
	}
}

void run()
{
	while (serverRunning)
	{
		if (gui != nullptr)
		{
			gui->update();
			if (gui->closeRequested())
				shouldStopServer = true;
		}

		if (serverStopping)
		{
			if (acceptor->hasStopped())
				serverRunning = false;
		}
		else
		{
			if (shouldStopServer)
			{
				acceptor->stop();
				serverStopping = true;
			}
		}
	}
}

void stop()
{
	shouldStopServer = true;
}

}
