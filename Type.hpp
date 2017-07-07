#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <ostream>

namespace IrStd
{
	enum class TypeFormat
	{
		FLAG_FLOOR = 0x1,
		FLAG_ROUND = 0x2,
		FLAG_CEIL = 0x3
	};

	namespace Type
	{
		/**
		 * Number conversion to string
		 * \{
		 */
		size_t uint64ToString(char* pBuffer, const size_t bufferSize, const uint64_t n) noexcept;
		size_t int64ToString(char* pBuffer, const size_t bufferSize, const int64_t n) noexcept;
		size_t doubleToString(char* pBuffer, const size_t bufferSize, const double n,
				const size_t maxPrecision = 6, const TypeFormat = TypeFormat::FLAG_ROUND) noexcept;
		/// \}

		template<typename T>
		class NumericImpl
		{
		public:
			NumericImpl()
			{
			}

			template<typename U>
			NumericImpl(const U& v)
					: m_value(v)
			{
			}

			operator T() const
			{
				return m_value;
			}

			template<typename U>
			void operator=(const U& v)
			{
				m_value = v;
			}

			template<typename U>
			NumericImpl operator+(const U& v) const
			{
				return NumericImpl(m_value + v);
			}
			template<typename U>
			NumericImpl operator-(const U& v) const
			{
				return NumericImpl(m_value - v);
			}
			template<typename U>
			NumericImpl operator*(const U& v) const
			{
				return NumericImpl(m_value * v);
			}
			template<typename U>
			NumericImpl operator/(const U& v) const
			{
				return NumericImpl(m_value / v);
			}
			template<typename U>
			NumericImpl operator%(const U& v) const
			{
				return NumericImpl(m_value % v);
			}

			template<typename U>
			void operator+=(const U& v)
			{
				m_value += v;
			}
			template<typename U>
			void operator-=(const U& v)
			{
				m_value -= v;
			}
			template<typename U>
			void operator*=(const U& v)
			{
				m_value *= v;
			}
			template<typename U>
			void operator/=(const U& v)
			{
				m_value /= v;
			}
			template<typename U>
			void operator%=(const U& v)
			{
				m_value %= v;
			}

			T operator++()
			{
				return ++m_value;
			}
			T operator--()
			{
				return --m_value;
			}
			T operator++(const int v)
			{
				return m_value++;
			}
			T operator--(const int v)
			{
				return m_value--;
			}

			template<typename U>
			bool operator==(const U& v) const
			{
				return m_value == v;
			}
			template<typename U>
			bool operator!=(const U& v) const
			{
				return m_value != v;
			}
			template<typename U>
			bool operator>(const U& v) const
			{
				return m_value > v;
			}
			template<typename U>
			bool operator<(const U& v) const
			{
				return m_value < v;
			}
			template<typename U>
			bool operator>=(const U& v) const
			{
				return m_value >= v;
			}
			template<typename U>
			bool operator<=(const U& v) const
			{
				return m_value <= v;
			}

			void toStream(std::ostream& os) const
			{
				os << m_value;
			}

			std::string toString() const
			{
				std::stringstream ss;
				std::string str;

				toStream(ss);
				ss >> str;

				return str;
			}
		protected:
			T m_value;
		};
	};
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const IrStd::Type::NumericImpl<T>& numType)
{
	numType.toStream(os);
	return os;
}

// ---- Implementations -------------------------------------------------------

#include "Type/Timestamp.hpp"
#include "Type/ShortString.hpp"
#include "Type/Circular.hpp"
