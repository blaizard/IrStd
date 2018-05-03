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

size_t IrStd::Type::Timestamp::getYears() const noexcept
{
	const uint64_t value = m_value / 1000;
	const tm *pltm = localtime(reinterpret_cast<const time_t*>(&value));
	return pltm->tm_year + 1900;
}

size_t IrStd::Type::Timestamp::getMonths() const noexcept
{
	const uint64_t value = m_value / 1000;
	const tm *pltm = localtime(reinterpret_cast<const time_t*>(&value));
	return pltm->tm_mon + 1;
}

size_t IrStd::Type::Timestamp::getDays() const noexcept
{
	const uint64_t value = m_value / 1000;
	const tm *pltm = localtime(reinterpret_cast<const time_t*>(&value));
	return pltm->tm_mday;
}

size_t IrStd::Type::Timestamp::getHours() const noexcept
{
	return (m_value / (1000 * 60 * 60)) % 24;
}

size_t IrStd::Type::Timestamp::getMinutes() const noexcept
{
	return (m_value / (1000 * 60)) % 60;
}

size_t IrStd::Type::Timestamp::getSeconds() const noexcept
{
	return (m_value / 1000) % 60;
}

size_t IrStd::Type::Timestamp::getMilliseconds() const noexcept
{
	return m_value % 1000;
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
			<< "." << std::setw(3) << getMilliseconds()
			<< std::setfill(' ');
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::Timestamp& time)
{
	time.toStream(os);
	return os;
}
