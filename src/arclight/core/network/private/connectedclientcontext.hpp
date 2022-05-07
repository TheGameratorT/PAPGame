#pragma once

#include <asio/ip/tcp.hpp>

#include "types.hpp"
#include "../common.hpp"

NETWORK_BEGIN

class Server;

struct ConnectedClientContext
{
	Server* server;
	asio::io_context* io_context;
	asio::ip::tcp::socket* socket;
};

NETWORK_END
