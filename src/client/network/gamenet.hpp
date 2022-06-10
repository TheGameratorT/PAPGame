#pragma once

#include <string>
#include <functional>

#include "types.hpp"
#include "network/clientconnection.hpp"
#include "locale/unicodestring.hpp"

namespace GameNet
{
	using ConnectCallback = std::function<void(bool)>;

	void init();
	void destroy();

	void connect(const std::string& address, u16 port, const ConnectCallback& callback);
	void disconnect();

	void acceptConnection();

	Network::ClientConnection* getConnection();
	bool getHandshakeDone();
}
