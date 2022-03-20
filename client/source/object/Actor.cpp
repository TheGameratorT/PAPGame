#include "Actor.hpp"

Actor::Actor() :
	position(0.0f, 0.0f, 0.0f),
	rotation(0.0f, 0.0f, 0.0f),
	scale(0.0f, 0.0f, 0.0f),
	prevPosition(0.0f, 0.0f, 0.0f),
	prevRotation(0.0f, 0.0f, 0.0f),
	prevScale(0.0f, 0.0f, 0.0f)
{}

Actor::~Actor() = default;

void Actor::create()
{

}

void Actor::update()
{
	prevPosition = position;
	prevRotation = rotation;
	prevScale = scale;
}

void Actor::render()
{

}

void Actor::destroy()
{

}
