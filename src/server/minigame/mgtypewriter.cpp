#include "mgtypewriter.hpp"

#include "../server.hpp"
#include "network/packet/pkt_s2c_typestate.hpp"
#include "network/packet/pkt_s2c_typeend.hpp"
#include "util/log.hpp"
#include "util.hpp"

constexpr u32 TICK_RATE = 30;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;
constexpr u32 STORIES_PER_ROUND = 3;

IMPL_MINIGAME(MGTypeWriter, "Type Writer")

void MGTypeWriter::onCreate()
{
	Server::setTickRate(TICK_RATE);

	stories = TypeWriterStory::loadStories("@/typewriter/textdata.txt");
	for (auto& story : stories)
		Log::info("MGTypeWriter",
				  std::string("Loaded story: ") + Util::utf8AsString(story.getTitle()) +
				  " (size: " + std::to_string(story.getText().size()) + ")");
	playedStoryList = new bool[stories.size()]();
}

void MGTypeWriter::onUpdate()
{
	if (!playingStory)
		return;

	auto& players = Server::getPlayers();

	bool allFinished = true;
	for (auto& p : players)
	{
		if (p->getCharsTyped() < lettersToType)
		{
			allFinished = false;
			break;
		}
	}

	std::vector<Player*> placeSortedPlayers(players.size());
	for (SizeT i = 0; i < players.size(); i++)
		placeSortedPlayers[i] = players[i].get();
	std::sort(placeSortedPlayers.begin(), placeSortedPlayers.end(), [](Player* a, Player* b){
		if (a->getCharsTyped() == b->getCharsTyped())
			return a->getPreviousPlace() < b->getPreviousPlace();
		return a->getCharsTyped() > b->getCharsTyped();
	});
	for (SizeT i = 0; i < placeSortedPlayers.size(); i++)
		placeSortedPlayers[i]->setPreviousPlace(i);

	Player* leadingPlayer = placeSortedPlayers[0];

	for (auto& p : players)
	{
		u32 leadingPlace = 0;
		for (; leadingPlace < placeSortedPlayers.size(); leadingPlace++)
		{
			Player* po = placeSortedPlayers[leadingPlace];
			if (po == p.get())
				break;
		}
		auto* typeStats = new TypeStateStats(); // gets deleted by PKT_S2C_TypeState
		typeStats->leadingPlayerID = leadingPlayer->getID();
		typeStats->leadingPlayerTL = leadingPlayer->getCharsTyped();
		typeStats->leadingPlace = leadingPlace;
		typeStats->inFrontPlayerID = (leadingPlace == 0) ? p->getID() : placeSortedPlayers[leadingPlace - 1]->getID();
		p->getClient()->sendPacket<PKT_S2C_TypeState>(false, 0, typeStats);
	}

	if (allFinished)
	{
		Log::info("MGTypeWriter", "Round finished!");

		playingStory = false;
		playedRounds++;

		bool wasLastRound = playedRounds == STORIES_PER_ROUND;

		auto* stats = new RoundStats();
		RoundStats* finalStats = wasLastRound ? new RoundStats() : nullptr;

		auto& playerInfo = stats->playerInfo;
		playerInfo.resize(placeSortedPlayers.size());
		for (SizeT i = 0; i < playerInfo.size(); i++)
		{
			u32 earnedPoints = 100 - (i * 5);
			placeSortedPlayers[i]->setPoints(placeSortedPlayers[i]->getPoints() + earnedPoints);
			placeSortedPlayers[i]->setCharsTyped(0);
			playerInfo[i].playerID = placeSortedPlayers[i]->getID();
			playerInfo[i].place = i;
			playerInfo[i].points = earnedPoints;
		}

		if (wasLastRound)
		{
			std::vector<Player*> pointSortedPlayers(players.size());
			for (SizeT i = 0; i < players.size(); i++)
				pointSortedPlayers[i] = players[i].get();
			std::sort(pointSortedPlayers.begin(), pointSortedPlayers.end(), [](Player* a, Player* b){
				return a->getPoints() > b->getPoints();
			});

			auto& finalPlayerInfo = finalStats->playerInfo;
			finalPlayerInfo.resize(players.size());
			for (SizeT i = 0; i < finalPlayerInfo.size(); i++)
			{
				finalPlayerInfo[i].playerID = pointSortedPlayers[i]->getID();
				finalPlayerInfo[i].place = i;
				finalPlayerInfo[i].points = pointSortedPlayers[i]->getPoints();
			}
		}

		PKT_S2C_TypeEnd pkt(wasLastRound, stats, finalStats); // auto deletes stats + finalStats

		for (auto& p : Server::getPlayers())
		{
			p->getClient()->sendPacket(pkt);
		}
	}
}

void MGTypeWriter::onDestroy()
{
	delete playedStoryList;
}

void MGTypeWriter::onClientsReady()
{
	if (playedRounds == STORIES_PER_ROUND)
	{
		// end game and start next minigame
		Server::startRandomMinigame();
		return;
	}

	startStory();
}

void MGTypeWriter::decideStory()
{
	u32 curStoryTmp;
	do {
		curStoryTmp = Server::getRandom(stories.size());
	} while (playedStoryList[curStoryTmp]);
	playedStoryList[curStoryTmp] = true;
	curStory = curStoryTmp;

	lettersToType = stories[curStory].getCharCount();
}

void MGTypeWriter::startStory()
{
	Log::info("MGTypeWriter", "Deciding story...");
	decideStory();

	Log::info("MGTypeWriter", "Starting story: " + Util::utf8AsString(stories[curStory].getTitle()));
	for (auto& p : Server::getPlayers())
	{
		// TODO: Check if current story was decided before all clients ready
		// (HIGHLY UNLIKELY TO HAPPEN, unless server lags a lot)
		p->getClient()->sendPacket<PKT_S2C_TypeState>(true, curStory, nullptr);
	}

	playingStory = true;
}
