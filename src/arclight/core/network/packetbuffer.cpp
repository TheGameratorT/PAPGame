#include "packetbuffer.hpp"

NETWORK_BEGIN

PacketBuffer::PacketBuffer() :
	m_cursor(0)
{}

PacketBuffer::PacketBuffer(std::vector<char> data) :
	m_data(std::move(data)),
	m_cursor(0)
{}

void PacketBuffer::reserve(SizeT size)
{ m_data.reserve(size); }

void PacketBuffer::expand(SizeT size)
{ m_data.resize(m_data.size() + size); }

void PacketBuffer::seek(SizeT size)
{ m_cursor = size; }

SizeT PacketBuffer::readableBytes() const
{ return m_data.size() - m_cursor; }

std::string PacketBuffer::readString()
{
	char* cur = &m_data[m_cursor];
	std::string out(cur);
	m_cursor += out.size() + 1;
	return out;
}

void PacketBuffer::writeString(const std::string& str)
{
	SizeT writeSize = str.size() + 1;
	expand(writeSize);
	const char* strData = str.data();
	std::copy(strData, strData + writeSize, &m_data[m_cursor]);
	m_cursor += writeSize;
}

NETWORK_END
