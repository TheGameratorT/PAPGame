#include "encodedpacket.hpp"

#include <vector>
#include <algorithm>

NETWORK_BEGIN

EncodedPacket::EncodedPacket() :
	m_dataSize(0)
{}

EncodedPacket::EncodedPacket(std::unique_ptr<char[]> data, std::size_t dataSize) :
	m_data(std::move(data)),
	m_dataSize(dataSize)
{}

EncodedPacket EncodedPacket::encode(const Packet& packet, u32 id)
{
	PacketBuffer buf;
	packet.serialize(buf);
	std::vector<char>& bufData = buf.getData();
	std::size_t bufSize = bufData.size();
	std::size_t dataSize = bufSize + headerSize;
	auto encPacketData = std::make_unique<char[]>(dataSize);
	EncodedPacketHeader& header = *reinterpret_cast<EncodedPacketHeader*>(encPacketData.get());
	header.id = id;
	header.bodySize = bufSize;
	std::copy(bufData.begin(), bufData.end(), &encPacketData[headerSize]);
	return {std::move(encPacketData), dataSize};
}

EncodedPacket EncodedPacket::createReader(std::size_t bufferSize)
{
	return {std::make_unique<char[]>(bufferSize), bufferSize};
}

NETWORK_END
