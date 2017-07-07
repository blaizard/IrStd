#include <cstring>

#include "../Assert.hpp"
#include "../Data.hpp"

// ---- IrStd::Data -----------------------------------------------------------

IrStd::Data::Data(const char* const pData)
		: Data(strlen(pData))
{
	memcpy(get(), pData, m_length);
}

IrStd::Data::Data(const uint8_t* const pData, const size_t length)
		: Data(length)
{
	memcpy(get(), pData, m_length);
}

IrStd::Data::Data(const size_t length)
		: m_length(length)
		, m_bufferLength(m_length)
		, m_pBuffer(m_bufferLength)
{
}

uint8_t* IrStd::Data::get() noexcept
{
	return m_pBuffer.data();
}

const uint8_t* IrStd::Data::get() const noexcept
{
	return m_pBuffer.data();
}

size_t IrStd::Data::getLength() const noexcept
{
	return m_length;
}

void IrStd::Data::setLength(const size_t length) noexcept
{
	IRSTD_ASSERT(length <= m_bufferLength, "The new length (" << length
			<< ") cannot be larger than the initial buffer length (" << m_bufferLength << ")");
	m_length = length;
}

std::unique_ptr<std::string> IrStd::Data::toHex() const
{
    static const char* const pDigits = "0123456789abcdef";

	auto pStr = std::unique_ptr<std::string>(new std::string(getLength() * 2, '0'));
	const uint8_t* pData = get();

	for (size_t i = 0; i < getLength(); ++i)
	{
		const uint8_t ch = pData[i];
		pStr->at(i*2) = pDigits[(ch & 0xf0) >> 4];
		pStr->at(i*2 + 1) = pDigits[ch & 0xf];
	}

	return std::move(pStr);
}