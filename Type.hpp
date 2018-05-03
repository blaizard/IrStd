#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <ostream>
#include <cmath>

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
				const size_t maxPrecision = 6, const TypeFormat format = TypeFormat::FLAG_ROUND) noexcept;
		/// \}

		/**
		 * Format a double number
		 */
		double doubleFormat(const double value, const size_t maxPrecision, const TypeFormat format = TypeFormat::FLAG_ROUND);

		/**
		 * Return the redolution (min value) of a double of a specific precision
		 */
		double doubleResolution(const size_t maxPrecision) noexcept;

		/**
		 * \brief Cast an enum class to its underlying type
		 */
		template<class T>
		constexpr typename std::underlying_type<T>::type toIntegral(const T value)
		{
			return static_cast<typename std::underlying_type<const T>::type>(value);
		}
	};
}

// ---- Implementations -------------------------------------------------------

#include "Type/Numeric.hpp"
#include "Type/Timestamp.hpp"
#include "Type/Memory.hpp"
#include "Type/ShortString.hpp"
#include "Type/RingBuffer.hpp"
#include "Type/RingBufferSorted.hpp"
#include "Type/Decimal.hpp"
#include "Type/Gson.hpp"
#include "Type/Buffer.hpp"
#include "Type/StreamDB.hpp"
#include "Type/Stopwatch.hpp"
