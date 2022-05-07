#pragma once

#include "types.hpp"

#include "network/server.hpp"
#include "network/connectedclient.hpp"

class PacketListener;

class ConnectedClientHandle
{
public:
	ConnectedClientHandle(u32 id, Network::ConnectedClientPtr connection, bool didHandshake) :
		m_id(id), m_connection(std::move(connection)), m_listener(nullptr), m_didHandshake(didHandshake)
	{}

	~ConnectedClientHandle();

	[[nodiscard]] constexpr u32 getID() const
	{ return m_id; }

	[[nodiscard]] Network::ConnectedClientPtr getConnection() const
	{ return m_connection; }

	void setListener(PacketListener* listener)
	{ m_listener = listener; }

	[[nodiscard]] constexpr bool didHandshake() const
	{ return m_didHandshake; }

	constexpr void markHandshake()
	{ m_didHandshake = true; }

private:
	u32 m_id;
	Network::ConnectedClientPtr m_connection;
	PacketListener* m_listener;
	bool m_didHandshake;
};

class ConnectionManager
{
public:
	using ConnectionListener = std::function<void(const ConnectedClientHandle&)>;
	using DisconnectionListener = std::function<void(const ConnectedClientHandle&)>;

	ConnectionManager();

	void startServer(u16 port);
	void stopServer();

	inline void setConnectionListener(const ConnectionListener& connectionListener)
	{ m_connectionListener = connectionListener; }

	inline void setDisconnectionListener(const DisconnectionListener& disconnectionListener)
	{ m_disconnectionListener = disconnectionListener; }

	ConnectionListener m_connectionListener;
	DisconnectionListener m_disconnectionListener;

private:
	Network::Server m_server;
	std::vector<std::unique_ptr<ConnectedClientHandle>> m_clients;
	u32 m_nextClientID;
};
