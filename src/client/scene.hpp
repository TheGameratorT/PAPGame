#pragma once

#include "common/concepts.hpp"

#include "object.hpp"
#include "objects.hpp"
#include "network/packet.hpp"

class Scene : public Object
{
public:
	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	virtual void onDestroyRequest();
	virtual void onConnectionLost();
	virtual void onPacketReceived(Network::PacketID id, const Network::Packet& packet);

	constexpr static Scene* cast(void* other)
	{ return static_cast<Scene*>(other); }
};

template<class T>
concept SceneType = CC::BaseOf<Scene, T>;
