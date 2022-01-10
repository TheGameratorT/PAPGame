#include "ByteBuf.hpp"

ByteBuf::ByteBuf() :
	m_cursor(0)
{}

ByteBuf::ByteBuf(std::vector<char> data) :
	m_data(std::move(data)),
	m_cursor(0)
{}

void ByteBuf::reserve(std::size_t size)
{ m_data.reserve(size); }

void ByteBuf::expand(std::size_t size)
{ m_data.resize(m_data.size() + size); }

void ByteBuf::seek(std::size_t size)
{ m_cursor = size; }

std::size_t ByteBuf::readableBytes() const
{ return m_data.size() - m_cursor; }

std::string ByteBuf::readString()
{
	char* cur = &m_data[m_cursor];
	std::string out(cur);
	m_cursor = out.size() + 1;
	return out;
}

void ByteBuf::writeString(const std::string& str)
{
	std::size_t writeSize = str.size() + 1;
	expand(writeSize);
	const char* strData = str.data();
	std::copy(strData, strData + writeSize, &m_data[m_cursor]);
	m_cursor += writeSize;
}
