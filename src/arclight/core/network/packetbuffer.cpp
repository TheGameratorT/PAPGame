#include "packetbuffer.hpp"

NETWORK_BEGIN

PacketBuffer::PacketBuffer() :
	m_cursor(0)
{}

PacketBuffer::PacketBuffer(std::vector<char> data) :
	m_data(std::move(data)),
	m_cursor(0)
{}

void PacketBuffer::reserve(std::size_t size)
{ m_data.reserve(size); }

void PacketBuffer::expand(std::size_t size)
{ m_data.resize(m_data.size() + size); }

void PacketBuffer::seek(std::size_t size)
{ m_cursor = size; }

std::size_t PacketBuffer::readableBytes() const
{ return m_data.size() - m_cursor; }

std::string PacketBuffer::readString()
{
	char* cur = &m_data[m_cursor];
	std::string out(cur);
	m_cursor = out.size() + 1;
	return out;
}

void PacketBuffer::writeString(const std::string& str)
{
	std::size_t writeSize = str.size() + 1;
	expand(writeSize);
	const char* strData = str.data();
	std::copy(strData, strData + writeSize, &m_data[m_cursor]);
	m_cursor += writeSize;
}

NETWORK_END
