#pragma once

#include <asio/ip/tcp.hpp>

#include "../common.hpp"

NETWORK_BEGIN

struct ConnectionContext
{
	asio::io_context* io_context;
	asio::ip::tcp::socket* socket;
};

NETWORK_END
