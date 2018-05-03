#pragma once

#include <iostream>
#include <string>
#include <sstream>

#include "../Compiler.hpp"

namespace IrStd
{
	namespace Type
	{
		template<class T>
		class Numeric
		{
		public:
			/**
			 * \brief Create a number from a string
			 * \note Only allow specializations
			 */
			static Numeric<T> fromString(const char* const pStr) = delete;

			/**
			 * \brief Return the minimal value
			 */
			static constexpr Numeric<T> min() noexcept
			{
				return Numeric<T>(std::numeric_limits<T>::min());
			}

			/**
			 * \brief Return the maximum value
			 */
			static constexpr Numeric<T> max() noexcept
			{
				return Numeric<T>(std::numeric_limits<T>::max());
			}

			constexpr Numeric() noexcept
			{
			}

			template<class U>
			constexpr Numeric(const U& v) noexcept
					: m_value(v)
			{
			}

			constexpr operator T() const noexcept
			{
				return m_value;
			}

			template<class U>
			constexpr Numeric operator+(const U& v) const noexcept
			{
				return Numeric(m_value + v);
			}
			template<class U>
			constexpr Numeric operator-(const U& v) const noexcept
			{
				return Numeric(m_value - v);
			}
			template<class U>
			constexpr Numeric operator*(const U& v) const noexcept
			{
				return Numeric(m_value * v);
			}
			template<class U>
			constexpr Numeric operator/(const U& v) const
			{
				return Numeric(m_value / v);
			}
			template<class U>
			constexpr Numeric operator%(const U& v) const
			{
				return Numeric(m_value % v);
			}

			template<class U>
			void operator=(const U& v) noexcept
			{
				m_value = v;
			}

			template<class U>
			void operator+=(const U& v) noexcept
			{
				m_value += v;
			}
			template<class U>
			void operator-=(const U& v) noexcept
			{
				m_value -= v;
			}
			template<class U>
			void operator*=(const U& v) noexcept
			{
				m_value *= v;
			}
			template<class U>
			void operator/=(const U& v)
			{
				m_value /= v;
			}
			template<class U>
			void operator%=(const U& v)
			{
				m_value %= v;
			}

			T operator++() noexcept
			{
				return ++m_value;
			}
			T operator--() noexcept
			{
				return --m_value;
			}
			T operator++(const int v) noexcept
			{
				return m_value++;
			}
			T operator--(const int v) noexcept
			{
				return m_value--;
			}

			template<class U>
			bool operator==(const U& v) const noexcept
			{
				return m_value == static_cast<T>(v);
			}
			template<class U>
			bool operator!=(const U& v) const noexcept
			{
				return !(m_value == static_cast<T>(v));
			}
			template<class U>
			bool operator>(const U& v) const noexcept
			{
				return m_value > static_cast<T>(v);
			}
			template<class U>
			bool operator<(const U& v) const noexcept
			{
				return m_value < static_cast<T>(v);
			}
			template<class U>
			bool operator>=(const U& v) const noexcept
			{
				return m_value >= static_cast<T>(v);
			}
			template<class U>
			bool operator<=(const U& v) const noexcept
			{
				return m_value <= static_cast<T>(v);
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

		/**
		 * Create a number from a string
		 */
		template<>
		Numeric<double> Numeric<double>::fromString(const char* const pStr);
		template<>
		Numeric<float> Numeric<float>::fromString(const char* const pStr);
		template<>
		Numeric<int> Numeric<int>::fromString(const char* const pStr);
		template<>
		Numeric<long int> Numeric<long int>::fromString(const char* const pStr);
		template<>
		Numeric<long double> Numeric<long double>::fromString(const char* const pStr);
		template<>
		Numeric<long long int> Numeric<long long int>::fromString(const char* const pStr);
		template<>
		Numeric<unsigned long int> Numeric<unsigned long int>::fromString(const char* const pStr);
		template<>
		Numeric<unsigned long long int> Numeric<unsigned long long int>::fromString(const char* const pStr);
	};
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const IrStd::Type::Numeric<T>& numType)
{
	numType.toStream(os);
	return os;
}
