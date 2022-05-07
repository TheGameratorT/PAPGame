#pragma once

#include <asio/ip/tcp.hpp>

#include "../common.hpp"
#include "../connection.hpp"
#include "connectioncontext.hpp"

NETWORK_BEGIN

struct Connection::private_t
{
	ConnectionContext ctx;
};

NETWORK_END
