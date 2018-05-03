#include <cstring>

#include "ShortString.hpp"

// ---- IrStd::Type::ShortString::ShortString ---------------------------------

IrStd::Type::ShortString::ShortString(const char* const pStr)
		: m_pString(pStr)
{
}

IrStd::Type::ShortString::ShortString(const uint8_t n)
		: m_pString(m_string)
{
	uint64ToString(m_string, SIZE, n);
}

IrStd::Type::ShortString::ShortString(const uint16_t n)
		: m_pString(m_string)
{
	uint64ToString(m_string, SIZE, n);
}

IrStd::Type::ShortString::ShortString(const uint32_t n)
		: m_pString(m_string)
{
	uint64ToString(m_string, SIZE, n);
}

IrStd::Type::ShortString::ShortString(const uint64_t n)
		: m_pString(m_string)
{
	uint64ToString(m_string, SIZE, n);
}

IrStd::Type::ShortString::ShortString(const int8_t n)
		: m_pString(m_string)
{
	int64ToString(m_string, SIZE, n);
}

IrStd::Type::ShortString::ShortString(const int16_t n)
		: m_pString(m_string)
{
	int64ToString(m_string, SIZE, n);
}

IrStd::Type::ShortString::ShortString(const int32_t n)
		: m_pString(m_string)
{
	int64ToString(m_string, SIZE, n);
}

IrStd::Type::ShortString::ShortString(const int64_t n)
		: m_pString(m_string)
{
	int64ToString(m_string, SIZE, n);
}

IrStd::Type::ShortString::ShortString(const double n, const size_t maxPrecision, const TypeFormat format)
		: m_pString(m_string)
{
	doubleToString(m_string, SIZE, n, maxPrecision, format);
}

IrStd::Type::ShortString::ShortString(const float n, const size_t maxPrecision, const TypeFormat format)
		: m_pString(m_string)
{
	doubleToString(m_string, SIZE, n, maxPrecision, format);
}

void IrStd::Type::ShortString::toStream(std::ostream& os) const
{
	os << m_pString;
}

IrStd::Type::ShortString::ShortString(const ShortString& str)
{
	*this = str;
}

IrStd::Type::ShortString& IrStd::Type::ShortString::operator=(const ShortString& str)
{
	// If the string pointer points onto its internal buffer
	if (str.m_pString >= str.m_string && str.m_pString < &str.m_string[SIZE])
	{
		std::memcpy(m_string, str.m_string, SIZE);
		m_pString = m_string;
	}
	else
	{
		m_pString = str.m_pString;
	}
	return *this;
}

// ---- IrStd::Type::ShortString::operator ------------------------------------


const char* IrStd::Type::ShortString::c_str() const noexcept
{
	return m_pString;
}

IrStd::Type::ShortString::operator const char*() const noexcept
{
	return c_str();
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::ShortString& str)
{
	str.toStream(os);
	return os;
}
