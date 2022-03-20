#pragma once

#include <type_traits>

#define DECL_OBJECT_PROFILE() \
static const ObjectProfile profile; \
const ObjectProfile& getProfile() const override;

#define IMPL_OBJECT_PROFILE(T) \
const ObjectProfile T::profile = { construct<T>, #T }; \
const ObjectProfile& T::getProfile() const { return profile; }

class ObjectProfile;

// Abstract class for a game object
class Object
{
public:
	Object();
	virtual ~Object();

	// Returns information about this object
	[[nodiscard]] virtual const ObjectProfile& getProfile() const = 0;

	// Virtual called when the object is created
	virtual void create();
	// Virtual called before rendering
	virtual void update();
	// Virtual called before rendering
	virtual void render();
	// Virtual called before the object is destroyed
	virtual void destroy();

	template<class T> requires std::is_base_of_v<Object, T>
	static Object* construct()
	{ return new T(); }
};

using ObjectCtorFunc = Object* (*)();

struct ObjectProfile
{
	ObjectCtorFunc ctor;
	const char* name;
};
