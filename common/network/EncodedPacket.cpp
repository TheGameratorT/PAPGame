#include "EncodedPacket.hpp"

#include <vector>
#include <algorithm>

EncodedPacket::EncodedPacket(std::unique_ptr<char[]> data, std::size_t dataSize) :
		m_data(std::move(data)),
		m_dataSize(dataSize)
{};

EncodedPacket::EncodedPacket(const EncodedPacket& other)
{
	std::size_t dataSize = other.m_dataSize;
	m_data = std::unique_ptr<char[]>(new char[dataSize]);
	std::copy(&other.m_data[0], &other.m_data[dataSize], &m_data[0]);
	m_dataSize = dataSize;
}

EncodedPacket EncodedPacket::encode(const Packet& packet, std::uint_fast32_t id)
{
	ByteBuf buf = packet.serialize();
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
	std::unique_ptr<char[]> buffer(new char[bufferSize]);
	return {std::move(buffer), bufferSize};
}
