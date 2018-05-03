#include <cstring>

#include "../Topic.hpp"
#include "../Assert.hpp"
#include "Buffer.hpp"

IRSTD_TOPIC_USE_ALIAS(IrStdType, IrStd, Type);

// ---- IrStd::Data -----------------------------------------------------------

IrStd::Type::Buffer::Buffer(const size_t length)
		: m_length(length)
		, m_pBuffer(new uint8_t[m_length])
		, m_allocated(true)
{
}

IrStd::Type::Buffer::Buffer(const std::string& str, const bool copy)
		: Buffer(str.c_str(), str.size(), copy)
{
}

IrStd::Type::Buffer::Buffer(const char* const pData, const bool copy)
		: Buffer(pData, (pData) ? strlen(pData) : 0, (pData) ? copy : false)
{
}

IrStd::Type::Buffer::Buffer(const Buffer& buffer, const bool copy)
		: Buffer(buffer.get<uint8_t>(), buffer.size(), copy)
{
}

IrStd::Type::Buffer::~Buffer()
{
	if (m_allocated)
	{
		delete[] m_pBuffer;
	}
}

template<>
void IrStd::Type::Buffer::memcpy(const size_t position, Buffer& buffer)
{
	this->memcpy(position, const_cast<const Buffer&>(buffer));
}

template<>
void IrStd::Type::Buffer::memcpy(const size_t position, const Buffer& buffer)
{
	IRSTD_ASSERT(IrStdType, m_allocated == true,
			"Can only append to already allocated buffers");
	IRSTD_ASSERT(IrStdType, position + buffer.size() <= m_length,
			"The buffer will overflow! position=" << position << ", buffer.size()="
			<< buffer.size() << ", m_length=" << m_length);
	std::memcpy(&m_pBuffer[position], buffer.get<uint8_t>(), buffer.size());
}

size_t IrStd::Type::Buffer::size() const noexcept
{
	return m_length;
}

void IrStd::Type::Buffer::setSize(const size_t length) noexcept
{
	IRSTD_ASSERT(IrStdType, length <= m_length, "The new length (" << length
			<< ") cannot be larger than the current length (" << m_length << ")");
	m_length = length;
}

std::string IrStd::Type::Buffer::hex() const
{
    static const char* const pDigits = "0123456789abcdef";

	const uint8_t* pData = get<uint8_t>();
	std::string result;
	result.resize(size() * 2);

	for (size_t i = 0; i < size(); ++i)
	{
		const uint8_t ch = pData[i];
		result.at(i*2) = pDigits[(ch & 0xf0) >> 4];
		result.at(i*2 + 1) = pDigits[ch & 0xf];
	}

	return result;
}

std::string IrStd::Type::Buffer::base64Encode() const
{
	const char* const BASE64_STR = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string result;

	int val = 0;
	int valb = -6;
	for (size_t i=0; i<m_length; i++)
	{
		const uint8_t c = m_pBuffer[i];
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0)
		{
			result.push_back(BASE64_STR[(val>>valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6)
	{
		result.push_back(BASE64_STR[((val << 8) >> (valb + 8)) & 0x3F]);
	}
	while (result.size() % 4)
	{
		result.push_back('=');
	}

	return result;
}

std::string IrStd::Type::Buffer::base64Decode() const
{
	const char* const BASE64_STR = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string result;

	std::vector<int> T(256, -1);
	for (int i=0; i<64; i++)
	{
		T[BASE64_STR[i]] = i;
	}

	int val = 0;
	int valb = -8;
	for (size_t i=0; i<m_length; i++)
	{
		const uint8_t c = m_pBuffer[i];
		if (T[c] == -1)
		{
			break;
		}
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0)
		{
			result.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return result;
}
