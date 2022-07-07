#pragma once

#include <vector>
#include <memory>

#include "player.hpp"
#include "minigame.hpp"
#include "minigametype.hpp"
#include "locale/unicodestring.hpp"

using TaskCallback = std::function<void()>;

namespace Server {

extern bool restartFlag;
extern int currentTps;
extern bool gameStarted;
extern std::vector<std::unique_ptr<Player>> players;
extern std::vector<U8String> lobbyChat;

inline bool getRestartFlag() { return restartFlag; }
inline int getCurrentTPS() { return currentTps; }
inline bool getGameStarted() { return gameStarted; }
inline std::vector<std::unique_ptr<Player>>& getPlayers() { return players; }
inline std::vector<U8String>& getLobbyChat() { return lobbyChat; }

bool init();
void destroy();

void run();
void stop();
void restart();

void update(double timeNow);
void render();

void setTickRate(u32 newTickRate);

double getElapsedTime();

u32 schedule(const TaskCallback& callback, u32 delayMs);
void cancelTask(u32 taskID);

void sendChatMessage(const U8String& msg);
void executeCommand(const std::string& cmd);
void notifyClientsReady();
void resetClientsReady();
void resetPlayedMinigames();
bool playedAllMinigames();
MiniGameType getNextRandomMinigame();
void startRandomMinigame();
void startMinigame(MiniGameType mgType);

void createMinigame(const MiniGame::Profile* minigame);

template<CC::BaseOfMiniGame T>
inline void createMinigame()
{ createMinigame(&T::profile); }

MiniGame* getMinigame(const MiniGame::Profile* minigame);

template<CC::BaseOfMiniGame T>
inline T* getMinigame()
{ return static_cast<T*>(getMinigame(&T::profile)); }

void endGame();

Player* getPlayerByID(u32 varPlayerID);

u32 getNextPlayerID();
u32 getNextTaskID();
u32 getRandom(u32 exclusiveMax);

}
