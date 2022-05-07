#pragma once

#include <type_traits>

#include "objects.hpp"

#define DECL_OBJECT(T) \
static const ObjectProfile profile; \
const ObjectProfile& getProfile() const override;

#define IMPL_OBJECT(T) \
const ObjectProfile T::profile = { construct<T>, #T, Objects::idOf<T>() }; \
const ObjectProfile& T::getProfile() const { return profile; }

class Object;

using ObjectCtorT = Object* (*)();

struct ObjectProfile
{
	ObjectCtorT ctor;
	const char* name;
	int id;
};

class Object
{
public:
	template<class T> requires std::is_base_of_v<Object, T>
	static Object* construct()
	{ return new T(); }

	Object();
	virtual ~Object();

	// Returns information about this object
	[[nodiscard]] virtual const ObjectProfile& getProfile() const = 0;

	// Virtual called when the object is created
	virtual void onCreate();
	// Virtual called in between frames, update game logic here
	virtual void onUpdate();
	// Virtual called before every frame, do drawing here
	virtual void onRender();
	// Virtual called before the object is destroyed
	virtual void onDestroy();

	[[nodiscard]] constexpr
	bool getLifeID() const
	{ return m_lifeID; }

	[[nodiscard]] constexpr
	bool getCreated() const
	{ return m_created; }

	constexpr void markCreated()
	{ m_created = true; }

	[[nodiscard]] constexpr
	bool getPendingDestroy() const
	{ return m_pendingDestroy; }

	void destroy()
	{ m_pendingDestroy = true; }

private:
	int m_lifeID;
	bool m_created;
	bool m_pendingDestroy;
};
