#pragma once

#include "render/gui/imgui/label.hpp"
#include "connectionmanager.hpp"

class NetPlayer
{
public:
	[[nodiscard]] constexpr const ConnectedClientHandle& getConnectionHandle() const
	{ return *m_connHandle; }

	[[nodiscard]] constexpr const U8String& getName() const
	{ return m_name; }

	const ConnectedClientHandle* m_connHandle;
	std::unique_ptr<GUI::Label> m_guiLabel;
	HandleT m_guiLabelHandle;
	U8String m_name;
};
