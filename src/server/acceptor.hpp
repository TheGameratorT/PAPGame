#pragma once

#include <memory>

#include "types.hpp"
#include "network/server.hpp"
#include "player.hpp"

class Acceptor
{
public:
	using ClientAcceptedCallback = std::function<void(std::unique_ptr<Player>)>;

	Acceptor();
	~Acceptor();

	void start(u16 port);
	void stop();

	[[nodiscard]] bool hasStopped() const;

	inline void setOnClientAccepted(const ClientAcceptedCallback& clientAcceptedCallback)
	{ m_clientAcceptedCallback = clientAcceptedCallback; }

public:
	Network::Server* m_server;
	ClientAcceptedCallback m_clientAcceptedCallback;
};
