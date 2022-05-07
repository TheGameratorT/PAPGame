#include "connectedclient.hpp"
#include "private/connectedclientcontext.hpp"

#include "private/connection.hpp"

#include "server.hpp"
#include "util/log.hpp"

NETWORK_BEGIN

struct ConnectedClient::private_t
{
	Server* server;
};

ConnectedClient::ConnectedClient(const ConnectedClientContext& context) :
	Connection(),
	p(new private_t())
{
	p->server = context.server;
	Connection::p->ctx.io_context = context.io_context;
	Connection::p->ctx.socket = context.socket;
	m_readPkt = EncodedPacket::createReader(512);
	m_connected = true;
}

ConnectedClient::~ConnectedClient()
{
	closeIfConnected();
	delete Connection::p->ctx.socket;
	delete p;
}

void ConnectedClient::terminate()
{
	Connection::terminate();
	if (p->server)
	{
		p->server->unlinkConnection(this);
		p->server = nullptr;
	}
}

void ConnectedClient::detach()
{
	m_connected = false;
	Connection::terminate();
	p->server = nullptr;
}

NETWORK_END
