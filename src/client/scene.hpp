#pragma once

#include "util/concepts.hpp"

#include "object.hpp"
#include "objects.hpp"

class Scene : public Object
{
public:
	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	virtual void onDestroyRequest();

	constexpr static Scene* cast(void* other)
	{ return static_cast<Scene*>(other); }
};

template<class T>
concept SceneType = BaseOf<Scene, T>;
