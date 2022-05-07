#include "object.hpp"

Object::Object() :
	m_lifeID(0),
	m_created(false),
	m_pendingDestroy(false)
{}

Object::~Object() = default;

void Object::onCreate()
{

}

void Object::onUpdate()
{

}

void Object::onRender()
{

}

void Object::onDestroy()
{

}
