#pragma once

#include "common.hpp"

NETWORK_BEGIN

class ConnectionError
{
public:
	enum Value
	{
		ConnectionFailed = 0,
		SendPacketUnlisted,
		SendPacketWrite,
		ReadPacketHeader,
		ReadPacketBody,
		ReadPacketInvalid,
		ReadPacketBufferExceeded
	};

	enum class Category
	{
		Connect = 0,
		SendPacket,
		ReadPacket
	};

	ConnectionError(Value value, Category category, const void* arg) :
		m_value(value), m_category(category), m_arg(arg)
	{}

	[[nodiscard]] constexpr Value value() const
	{ return m_value; }

	[[nodiscard]] constexpr Category category() const
	{ return m_category; }

	template<typename T>
	constexpr const T* arg() const
	{ return static_cast<const T*>(m_arg); }

	[[nodiscard]] constexpr const std::error_code& getCode() const
	{ return *static_cast<const std::error_code*>(m_arg); }

private:
	Value m_value;
	Category m_category;
	const void* m_arg;
};

NETWORK_END
