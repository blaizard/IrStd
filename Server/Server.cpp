#include <cstring>

#include "../Logger.hpp"
#include "../Server.hpp"

IRSTD_TOPIC_REGISTER(IrStdServer);

// ---- IrStd::ClientInfo -----------------------------------------------------

IrStd::ClientInfo::ClientInfo(const int socket)
		: m_socket(socket)
{
	m_ip[0] = '\0';
}

int IrStd::ClientInfo::getSocket() const noexcept
{
	return m_socket;
}

const char* IrStd::ClientInfo::getIp() const noexcept
{
	return m_ip;
}

void IrStd::ClientInfo::setIp(const char* const ip) noexcept
{
	std::strncpy(m_ip, ip, sizeof(m_ip) - 1);
}

std::string& IrStd::ClientInfo::getDataStr() noexcept
{
	return m_data;
}

void IrStd::ClientInfo::toStream(std::ostream& os) const
{
	os << "socket=" << getSocket() << ", ip=" << getIp();
}

std::ostream& operator<<(std::ostream& os, const IrStd::ClientInfo& info)
{
	info.toStream(os);
	return os;
}
