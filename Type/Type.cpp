#include <limits>
#include <cmath>
#include <cstring>

#include "../Type.hpp"

// ---- IrStd::Type (conversions) --------------------------------------

size_t IrStd::Type::uint64ToString(
		char* pBuffer,
		const size_t bufferSize,
		const uint64_t n) noexcept
{
	char* pStr = &pBuffer[bufferSize];
	uint64_t number = n;

	*--pStr = '\0';
	do
	{
		const int digit = static_cast<int>(number % 10) + '0';
		number /= 10;
		*--pStr = static_cast<char>(digit);
	} while (number);

	{
		const size_t size = static_cast<size_t>(&pBuffer[bufferSize] - pStr);
		std::memmove(pBuffer, pStr, size);
		return size;
	}
}

size_t IrStd::Type::int64ToString(
		char* pBuffer,
		const size_t bufferSize,
		const int64_t n) noexcept
{
	if (n < 0)
	{
		*const_cast<char*>(pBuffer) = '-';
		return uint64ToString(pBuffer + 1, bufferSize - 1, -n) + 1;
	}
	else
	{
		return uint64ToString(pBuffer, bufferSize, n);
	}
}

size_t IrStd::Type::doubleToString(
		char* pBuffer,
		const size_t bufferSize,
		const double n,
		const size_t maxPrecision,
		const TypeFormat format) noexcept
{
	// Set the integer part
	size_t size = int64ToString(pBuffer, bufferSize, static_cast<int64_t>(n));

	if (size < bufferSize - 1)
	{
		auto castAndFormat = [&format](const double d) -> uint64_t {
			if (format == TypeFormat::FLAG_ROUND)
			{
				return static_cast<uint64_t>(round(d));
			}
			else if (format == TypeFormat::FLAG_FLOOR)
			{
				return static_cast<uint64_t>(floor(d));
			}
			return static_cast<uint64_t>(ceil(d));
		};

		const size_t remainingSize = bufferSize - size - 1/*for the coma*/;
		// Keep only the numbers to print after the coma
		double decimalToInteger = std::abs(n - static_cast<int>(n));
		double decimalToIntegerPrint = 0;
		size_t nbPreZeros = 0;

		for (size_t precision = 0; precision < std::min(remainingSize, maxPrecision); ++precision)
		{
			decimalToInteger *= 10;
			// Don't print the trailing zeros, but count the zeros on the left
			if (castAndFormat(decimalToInteger) % 10)
			{
				decimalToIntegerPrint = decimalToInteger;
			}
			else if (!static_cast<int>(decimalToIntegerPrint))
			{
				nbPreZeros++;
			}
		}

		// Format the decimal to print
		const uint64_t integerPrint = castAndFormat(decimalToIntegerPrint);
		if (integerPrint)
		{
			pBuffer[size - 1] = '.';
			std::memset(&pBuffer[size], '0', nbPreZeros);
			size += nbPreZeros;
			size += uint64ToString(&pBuffer[size], remainingSize + 1 - nbPreZeros, integerPrint);
		}
	}

	return size;
}
