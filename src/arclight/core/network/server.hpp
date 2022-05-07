#pragma once

#include <thread>
#include <vector>
#include <memory>
#include <functional>

#include "types.hpp"
#include "common.hpp"
#include "connection.hpp"

#include "connectedclient.hpp"

NETWORK_BEGIN

class Server
{
public:
	using ConnectionListener = std::function<void(ConnectedClientPtr)>;

	Server();
	~Server();

	void start(u16 port);
	void stop();
	void waitUntilStopped();
	void onStopped();

	inline void setConnectionListener(const ConnectionListener& connectionListener)
	{ m_connectionListener = connectionListener; }

	void unlinkConnection(ConnectedClient* connection);

private:
	struct private_t;
	private_t* p;

	bool m_active;
	std::thread m_io_thread;
	std::vector<ConnectedClientPtr> m_connectedClients;

	ConnectionListener m_connectionListener;

	void acceptIncomingConnections();
};

NETWORK_END
