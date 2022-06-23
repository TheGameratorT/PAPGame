#pragma once

#include <vector>
#include <memory>

#include "player.hpp"
#include "locale/unicodestring.hpp"

using TaskCallback = std::function<void()>;

namespace Server {

extern bool gameStarted;
extern std::vector<std::unique_ptr<Player>> players;
extern std::vector<U8String> lobbyChat;

inline bool getGameStarted() { return gameStarted; }
inline std::vector<std::unique_ptr<Player>>& getPlayers() { return players; }
inline std::vector<U8String>& getLobbyChat() { return lobbyChat; }

bool init();
void destroy();

void run();
void stop();

void update(double timeNow);
void render();

void setTickRate(u32 newTickRate);

double getElapsedTime();

u32 schedule(const TaskCallback& callback, u32 delayMs);
void cancelTask(u32 taskID);

void sendChatMessage(const U8String& msg);
void executeCommand(const std::string& cmd);
void startGame();

u32 getNextPlayerID();
u32 getNextTaskID();

}
