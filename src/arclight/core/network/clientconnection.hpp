#pragma once

#include <thread>
#include <memory>

#include "common.hpp"
#include "connection.hpp"

NETWORK_BEGIN

class ClientConnection : public Connection
{
public:
	ClientConnection();
	~ClientConnection() override;

	void connect(const std::string& host, u16 port);
	void cancelConnect();

	static bool checkConnectCanceled(const std::error_code& error);

protected:
	void terminate() override;

private:
	std::thread m_io_thread;
	bool m_connecting;
};

NETWORK_END
