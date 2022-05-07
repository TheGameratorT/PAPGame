#pragma once

#include <memory>

#include "common.hpp"
#include "connection.hpp"

NETWORK_BEGIN

struct ConnectedClientContext;

class ConnectedClient : public Connection
{
public:
	[[nodiscard]] static inline std::shared_ptr<ConnectedClient> create(const ConnectedClientContext& context)
	{ return std::shared_ptr<ConnectedClient>(new ConnectedClient(context)); }

	~ConnectedClient() override;

	inline void start()
	{ readIncomingPacketHeader(); }

	void detach();

protected:
	void terminate() override;

private:
	explicit ConnectedClient(const ConnectedClientContext& context);

	struct private_t;
	private_t* p;
};

using ConnectedClientPtr = std::shared_ptr<ConnectedClient>;

NETWORK_END
