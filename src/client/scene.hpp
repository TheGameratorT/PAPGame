#pragma once

#include "common/concepts.hpp"

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
	virtual void onConnectionLost();

	constexpr static Scene* cast(void* other)
	{ return static_cast<Scene*>(other); }
};

template<class T>
concept SceneType = CC::BaseOf<Scene, T>;
