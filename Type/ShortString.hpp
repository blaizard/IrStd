#pragma once

#include "../Type.hpp"

namespace IrStd
{
	namespace Type
	{
		class ShortString
		{
		public:
			ShortString(const char* const pStr);
			ShortString(const uint8_t n);
			ShortString(const uint16_t n);
			ShortString(const uint32_t n);
			ShortString(const uint64_t n);
			ShortString(const int8_t n);
			ShortString(const int16_t n);
			ShortString(const int32_t n);
			ShortString(const int64_t n);
			ShortString(const double n, const size_t maxPrecision = 6,
					const TypeFormat format = TypeFormat::FLAG_ROUND);
			ShortString(const float n, const size_t maxPrecision = 6,
					const TypeFormat format = TypeFormat::FLAG_ROUND);

			// Copy constructor
			ShortString(const ShortString& str);
			ShortString& operator=(const ShortString& str);

			operator const char*() const noexcept;
			void toStream(std::ostream& os) const;

			const char* c_str() const noexcept;

			// max unsigned int size of 64-bit is 20 characters + null
			// max signed int size of 64-bit is 19 characters + sign + null
			static constexpr size_t SIZE = 21;
		private:
			char m_string[SIZE];
			const char* m_pString;
		};
	}
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::ShortString& str);
