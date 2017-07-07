#pragma once

#include <memory>
#include <string>

#include "Data.hpp"

namespace IrStd
{
	class Crypto
	{
	public:
		Crypto(const char* const pString);
		Crypto(const uint8_t* const pData, const size_t length);

		std::unique_ptr<IrStd::Data> HMACSHA1(const char* const key, const size_t keyLength = 0);
		std::unique_ptr<IrStd::Data> HMACSHA512(const char* const key, const size_t keyLength = 0);
	private:
		const uint8_t* const m_pData;
		const size_t m_length;
	};
}
