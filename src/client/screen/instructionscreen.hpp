#pragma once

class Texture;

namespace InstructionScreen {

void init();
void destroy();

void update();
void render();

void show();
void close();

void setInstructionTexture(Texture* texture);

}
