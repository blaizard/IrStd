#include <limits>
#include <cmath>
#include <cstring>

#include "../Type.hpp"
#include "../Assert.hpp"
#include "../Topic.hpp"

IRSTD_TOPIC_REGISTER(IrStd, Type);

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
	} while (number && pBuffer < pStr);

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
	const double value = doubleFormat(n, maxPrecision, format);

	// Print the sign and the integer part
	// Note we cannot use directly int64ToString as it will not print th esign for value
	// within ]0; -1[
	size_t size = 0;
	if (value < 0.)
	{
		pBuffer[size++] = '-';
	}
	size += uint64ToString(&pBuffer[size], bufferSize - size, static_cast<uint64_t>(std::abs(value)));

	if (size < bufferSize - 1)
	{
		const size_t remainingSize = bufferSize - size - 1; // for the coma
		// Keep only the numbers to print after the coma
		double decimalToInteger = std::abs(value - static_cast<int>(value));
		double decimalToIntegerToPrint = 0;
		size_t nbPreZeros = 0;
		bool recordPreZeros = true;

		for (size_t precision = 0; precision < std::min(remainingSize, maxPrecision); ++precision)
		{
			decimalToInteger *= 10;

			// Don't print the trailing zeros, but count the zeros on the left
			// This cast is needed to round the number and avoid some double artifacts
			// For example, this prevents 0.83 to become 0.82 (as the double will actually be 0.829999...)
			const auto decimalToIntegerCast = static_cast<uint64_t>(doubleFormat(decimalToInteger,
					maxPrecision - precision, TypeFormat::FLAG_ROUND));

			if (decimalToIntegerCast % 10)
			{
				decimalToIntegerToPrint = decimalToInteger;
				recordPreZeros = false;
			}
			else if (recordPreZeros)
			{
				nbPreZeros++;
			}

			// Uncomment for debug
			//std::cout << ", decimalToInteger=" << decimalToInteger
			//		<< ", decimalToIntegerToPrint=" << decimalToIntegerToPrint
			//		<< ", decimalToIntegerCast=" << decimalToIntegerCast
			//		<< ", nbPreZeros=" << nbPreZeros << std::endl;
		}

		const auto integerPrint = static_cast<uint64_t>(std::round(decimalToIntegerToPrint));
		// Format the decimal to print
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

double IrStd::Type::doubleResolution(const size_t maxPrecision) noexcept
{
	// Maximum precision is 15 after the coma (see DBL_DIG)
	static const double resolutionList[15] = {
		1, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001, 0.00000001, 0.000000001,
		0.0000000001, 0.00000000001, 0.000000000001, 0.0000000000001, 0.00000000000001};

	IRSTD_ASSERT(maxPrecision < sizeof(resolutionList) / sizeof(double), "Precision of "
			<< maxPrecision << " is not supported");

	return resolutionList[maxPrecision];
}

double IrStd::Type::doubleFormat(
		const double value,
		const size_t maxPrecision,
		const TypeFormat format)
{
	const auto resolution = doubleResolution(maxPrecision);

	int64_t castedValue;
	switch (format)
	{
	case TypeFormat::FLAG_FLOOR:
		castedValue = static_cast<int64_t>(std::floor(value / resolution));
		break;
	case TypeFormat::FLAG_CEIL:
		castedValue = static_cast<int64_t>(std::ceil(value / resolution));
		break;
	case TypeFormat::FLAG_ROUND:
		castedValue = static_cast<int64_t>(std::round(value / resolution));
		break;
	default:
		IRSTD_UNREACHABLE();
	}
	return static_cast<double>(castedValue) * resolution;
}
