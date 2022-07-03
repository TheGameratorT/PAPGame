#pragma once

class RoundStats;

namespace PlaceListingScreen {

void init();
void destroy();

void update();
void render();

void show(bool lastRound, const RoundStats* stats, const RoundStats* finalStats);
void close();

}
