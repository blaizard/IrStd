#include <iomanip>
#include <chrono>

#include "Timestamp.hpp"

// ---- IrStd::Timestamp ------------------------------------------------------

IrStd::Type::Timestamp IrStd::Type::Timestamp::now() noexcept
{
	return static_cast<Timestamp>(std::chrono::duration_cast
			<std::chrono::milliseconds>(std::chrono::system_clock::now()
			.time_since_epoch()).count());
}

void IrStd::Type::Timestamp::toStream(std::ostream& os) const
{
	const uint64_t value = m_value / 1000;
	const tm *pltm = localtime(reinterpret_cast<const time_t*>(&value));
	os << std::dec << std::right << std::setfill('0')
			<< std::setw(4) << (pltm->tm_year + 1900)
			<< "-" << std::setw(2) << (pltm->tm_mon + 1)
			<< "-" << std::setw(2) << (pltm->tm_mday)
			<< " " << std::setw(2) << (pltm->tm_hour)
			<< ":" << std::setw(2) << (pltm->tm_min)
			<< ":" << std::setw(2) << (pltm->tm_sec)
			<< "." << std::setw(3) << (m_value % 1000)
			<< std::setfill(' ');
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::Timestamp& time)
{
	time.toStream(os);
	return os;
}
