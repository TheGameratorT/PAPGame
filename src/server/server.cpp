#include "server.hpp"

#include <utility>

#include "infogui.hpp"
#include "time/timer.hpp"
#include "acceptor.hpp"
#include "network/packet.hpp"
#include "network/packet/pkt_s2c_lobbymsg.hpp"
#include "network/packet/pkt_s2c_startgame.hpp"
#include "util.hpp"

class ScheduledTask
{
public:
	TaskCallback callback;
	float firingTime;
	u32 id;

	ScheduledTask(TaskCallback callback, float firingTime, u32 id) :
		callback(std::move(callback)), firingTime(firingTime), id(id)
	{}
};

namespace Server {

static Acceptor* acceptor = nullptr;
static InfoGUI* gui = nullptr;
static Timer timer;

static bool serverRunning = true;
static bool serverStopping = false;
static bool shouldStopServer = false;

static u32 tickRate = 30;
static float tickDuration = 1.0f / static_cast<float>(tickRate);

static double lastTickTime;
static float tickDelta;
static float tickAlpha;
static int currentTps;

bool gameStarted = false;
MiniGameType currentMinigame;

std::vector<std::unique_ptr<Player>> players;
std::vector<U8String> lobbyChat;
std::vector<std::unique_ptr<ScheduledTask>> scheduledTasks;

bool init()
{
	gui = new InfoGUI();
	if (!gui->init())
	{
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

	timer.start();
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
	double lastTime = getElapsedTime();
	float tickTimer = 0.0f;

	while (serverRunning)
	{
		double timeNow = getElapsedTime();
		auto frameDelta = static_cast<float>(timeNow - lastTime);
		lastTime = timeNow;
		tickTimer += frameDelta;

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

void update(double timeNow)
{
	tickDelta = static_cast<float>(timeNow - lastTickTime);
	lastTickTime = timeNow;

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
}

void render()
{
	if (gui != nullptr)
	{
		gui->update();
		if (gui->closeRequested())
			shouldStopServer = true;
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

double getElapsedTime()
{
	return timer.getElapsedTime(Time::Unit::Seconds);
}

u32 schedule(const TaskCallback& callback, u32 delayMs)
{
	u32 taskID = getNextTaskID();
	auto task = std::make_unique<ScheduledTask>(
		callback,
		float(getElapsedTime() + float(delayMs) / 1000.0f),
		taskID
	);
	scheduledTasks.push_back(std::move(task));
	return taskID;
}

void cancelTask(u32 taskID)
{
	auto it = std::find_if(scheduledTasks.begin(), scheduledTasks.end(), [taskID](const std::unique_ptr<ScheduledTask>& a){
		return a->id == taskID;
	});
	if (it == scheduledTasks.end())
	{
		Log::error("Server", "Tried to cancel non-existent task with ID " + std::to_string(taskID));
		return;
	}
	scheduledTasks.erase(it);
}

void sendChatMessage(const U8String& msg)
{
	lobbyChat.push_back(msg);
	for (auto& player : players)
	{
		player->getClient()->sendPacket<PKT_S2C_LobbyMsg>(msg);
	}
}

void executeCommand(const std::string& cmd)
{
	if (cmd.starts_with("stop"))
	{
		stop();
	}
	else if (cmd.starts_with("say "))
	{
		sendChatMessage(Util::stringAsUtf8(cmd.substr(4)));
	}
}

void startGame()
{
	u32 mgCount = MiniGameType::Count;
	auto mg = MiniGameType(std::rand() % mgCount);
	for (auto& p : players)
		p->getClient()->sendPacket<PKT_S2C_StartGame>(mg);
	currentMinigame = mg;
	gameStarted = true;
}

u32 getNextPlayerID()
{
	static u32 next = 0;
	return ++next;
}

u32 getNextTaskID()
{
	static u32 next = 0;
	return ++next;
}

}
