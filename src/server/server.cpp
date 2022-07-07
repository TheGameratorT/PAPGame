#include "server.hpp"

#include <utility>

#include "minigame/mgpong.hpp"
#include "minigame/mgtypewriter.hpp"

#include "network/packet.hpp"
#include "network/packet/pkt_s2c_lobbymsg.hpp"
#include "network/packet/pkt_s2c_startgame.hpp"
#include "network/packet/pkt_s2c_gameend.hpp"

#include "infogui.hpp"
#include "time/timer.hpp"
#include "acceptor.hpp"
#include "random/mersennetwister.hpp"
#include "util.hpp"

constexpr SizeT MinigamesToPlay = 2;
constexpr u32 LobbyTickRate = 30;

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

static Mt19937* random;
static Acceptor* acceptor;
static InfoGUI* gui;
static Timer timer;

bool restartFlag;
static bool serverRunning;
static bool serverStopping;
static bool shouldStopServer;

static u32 tickRate;
static float tickDuration;

static double lastTickTime;
static float tickDelta;
static float tickAlpha;
int currentTps;

bool gameStarted;
MiniGame* currentMinigame;
MiniGameType currentMinigameType;
MiniGameType forcedMinigameType;
SizeT playedMinigameCount;

std::vector<std::unique_ptr<Player>> players;
std::vector<U8String> lobbyChat;
std::vector<std::unique_ptr<ScheduledTask>> scheduledTasks;

static bool playedMinigameList[MiniGameType::Count] = {};

bool init()
{
	random = new Mt19937(Seed32::fromEntropySource());

	restartFlag = false;
	serverRunning = true;
	serverStopping = false;
	shouldStopServer = false;
	gameStarted = false;
	currentMinigame = nullptr;
	forcedMinigameType = MiniGameType::Count;
	playedMinigameCount = 0;

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

	resetPlayedMinigames();

	setTickRate(LobbyTickRate);

	timer.start();
	return true;
}

void destroy()
{
	// TODO: Check if needs safe disconnect
	players.clear();
	lobbyChat.clear();
	scheduledTasks.clear();

	delete acceptor;

	if (gui != nullptr)
	{
		gui->destroy();
		delete gui;
	}

	delete random;
}

void run()
{
	Log::info("Server", "Server is running.");

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

void restart()
{
	Log::info("Server", "Restarting the server...");
	restartFlag = true;
	stop();
}

void update(double timeNow)
{
	tickDelta = static_cast<float>(timeNow - lastTickTime);
	lastTickTime = timeNow;
	currentTps = std::lround(1.0f / tickDelta);

	if (serverStopping)
		return;

	if (currentMinigame && players.empty()) // if not in lobby
	{
		restart();
		return;
	}

	SizeT scheduledTaskCount = scheduledTasks.size();
	for (SizeT i = scheduledTaskCount - 1; i <= 0; i--)
	{
		ScheduledTask& scheduledTask = *scheduledTasks[i];
		if (getElapsedTime() > scheduledTask.firingTime)
		{
			scheduledTask.callback();
			scheduledTasks.erase(scheduledTasks.begin() + i32(i));
		}
	}

	if (currentMinigame)
		currentMinigame->onUpdate();
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
	else if (cmd.starts_with("restart"))
	{
		restart();
	}
	else if (cmd.starts_with("say "))
	{
		sendChatMessage(Util::stringAsUtf8(cmd.substr(4)));
	}
	else if (cmd.starts_with("forcemg "))
	{
		u32 id = std::stoi(cmd.substr(8));
		if (id < MiniGameType::Count)
		{
			sendChatMessage(Util::stringAsUtf8("Server: Forced minigame " + std::to_string(id)));
			forcedMinigameType = static_cast<MiniGameType>(id);
		}
		else if (id == MiniGameType::Count)
		{
			sendChatMessage(u8"Server: Disabled forced minigame");
			forcedMinigameType = MiniGameType::Count;
		}
	}
}

// Tells the current minigame that all the clients are ready
void notifyClientsReady()
{
	resetClientsReady();
	if (currentMinigame)
		currentMinigame->onClientsReady();
}

void resetClientsReady()
{
	for (auto& p : players)
		p->setGameReady(false);
}

void resetPlayedMinigames()
{
	for (bool& b : playedMinigameList)
		b = false;
}

bool playedAllMinigames()
{
	bool playedAll = true;
	for (bool b : playedMinigameList)
	{
		if (!b)
			playedAll = false;
	}
	return playedAll;
}

MiniGameType getNextRandomMinigame()
{
	if (playedAllMinigames())
		resetPlayedMinigames();
	u32 mgCount = MiniGameType::Count;
	u32 mgID;
	do {
		mgID = getRandom(mgCount);
	} while (playedMinigameList[mgID]);
	return MiniGameType(mgID);
}

void startRandomMinigame()
{
	for (auto& p : players) // sum points from round
	{
		p->setGamePoints(p->getGamePoints() + p->getPoints());
		p->setPoints(0);
	}

	if (MinigamesToPlay == playedMinigameCount)
	{
		endGame();
		return;
	}

	MiniGameType mgType;
	if (forcedMinigameType != MiniGameType::Count)
		mgType = forcedMinigameType;
	else
		mgType = getNextRandomMinigame();
	startMinigame(mgType);
	gameStarted = true;
}

void startMinigame(MiniGameType mgType)
{
	currentMinigameType = mgType;
	playedMinigameList[mgType] = true;
	playedMinigameCount++;

	for (auto& p : players)
		p->getClient()->sendPacket<PKT_S2C_StartGame>(mgType);

	switch (mgType)
	{
	case MiniGameType::Pong:
		createMinigame<MGPong>();
		return;
	/*case MiniGameType::Rythm:
		return;*/
	case MiniGameType::TypeWrite:
		createMinigame<MGTypeWriter>();
		return;
	default:
		break;
	}

	Log::error("Server", "Tried to start invalid minigame!");
	stop(); // abort execution
}

void createMinigame(const MiniGame::Profile* minigame)
{
	if (currentMinigame)
	{
		currentMinigame->onDestroy();
		delete currentMinigame;
	}
	Log::info("Server", std::string("Created MiniGame: ") + minigame->name);
	MiniGame* newMg = minigame->ctor();
	newMg->onCreate();
	currentMinigame = newMg;
}

MiniGame* getMinigame(const MiniGame::Profile* minigame)
{
	if (currentMinigame && &currentMinigame->getProfile() == minigame)
		return currentMinigame;
	return nullptr;
}

void endGame()
{
	auto* finalStats = new RoundStats();
	std::vector<Player*> pointSortedPlayers(players.size());
	for (SizeT i = 0; i < players.size(); i++)
		pointSortedPlayers[i] = players[i].get();
	std::sort(pointSortedPlayers.begin(), pointSortedPlayers.end(), [](Player* a, Player* b){
		return a->getGamePoints() > b->getGamePoints();
	});
	auto& finalPlayerInfo = finalStats->playerInfo;
	finalPlayerInfo.resize(players.size());
	for (SizeT i = 0; i < finalPlayerInfo.size(); i++)
	{
		finalPlayerInfo[i].playerID = pointSortedPlayers[i]->getID();
		finalPlayerInfo[i].place = i;
		finalPlayerInfo[i].points = pointSortedPlayers[i]->getGamePoints();
	}
	PKT_S2C_GameEnd gameEndPkt(finalStats);
	for (auto& p : players)
	{
		p->getClient()->sendPacket(gameEndPkt);
		p->prepareForNextGame();
	}
	playedMinigameCount = 0;

	setTickRate(LobbyTickRate);
}

Player* getPlayerByID(u32 varPlayerID)
{
	for (auto& p : players)
	{
		if (p->getID() == varPlayerID)
			return p.get();
	}
	return nullptr;
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

u32 getRandom(u32 exclusiveMax)
{
	return random->next() % exclusiveMax;
}

}
