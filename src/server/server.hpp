#pragma once

#include <vector>
#include <memory>

class Player;

namespace Server {

extern std::vector<std::unique_ptr<Player>> players;

bool init();
void destroy();

void run();
void stop();

inline std::vector<std::unique_ptr<Player>>& getPlayers() { return players; }

}
