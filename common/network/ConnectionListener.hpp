#pragma once

#include "ConnectedClient.hpp"

class ConnectionListener
{
public:
	ConnectionListener() = default;
	virtual ~ConnectionListener() = default;

	virtual void onClientConnected(ConnectedClient& conClient) = 0;
	virtual void onClientDisconnected(ConnectedClient& conClient) = 0;
};
