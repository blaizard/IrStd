#include "../Assert.hpp"
#include "../Topic.hpp"

#include "Numeric.hpp"

IRSTD_TOPIC_USE_ALIAS(IrStdType, IrStd, Type);

#define HELPER_SPECIALIZATION_FROMSTRING(type, convertionFct) \
	template<> \
	IrStd::Type::Numeric<type> IrStd::Type::Numeric<type>::fromString(const char* const pStr) \
	{ \
		char* pEnd; \
		const type number = convertionFct(pStr, &pEnd); \
		IRSTD_THROW_ASSERT(IrStdType, *pEnd == '\0', "Error while converting \"" << pStr \
				<< "\" into " #type); \
		return Numeric<type>(number); \
	}

#define HELPER_SPECIALIZATION_FROMSTRING_BASE(type, convertionFct, base) \
	template<> \
	IrStd::Type::Numeric<type> IrStd::Type::Numeric<type>::fromString(const char* const pStr) \
	{ \
		char* pEnd; \
		const type number = static_cast<type>(convertionFct(pStr, &pEnd, 10)); \
		IRSTD_THROW_ASSERT(IrStdType, *pEnd == '\0', "Error while converting \"" << pStr \
				<< "\" into " #type); \
		return Numeric<type>(number); \
	}

// ---- IrStd::Type::Numeric (fromString) -------------------------------------

HELPER_SPECIALIZATION_FROMSTRING(double, std::strtod);
HELPER_SPECIALIZATION_FROMSTRING(float, std::strtof);
HELPER_SPECIALIZATION_FROMSTRING_BASE(int, std::strtol, 10);
HELPER_SPECIALIZATION_FROMSTRING_BASE(long int, std::strtol, 10);
HELPER_SPECIALIZATION_FROMSTRING(long double, std::strtold);
HELPER_SPECIALIZATION_FROMSTRING_BASE(long long int, std::strtoll, 10);
HELPER_SPECIALIZATION_FROMSTRING_BASE(unsigned long int, std::strtoul, 10);
HELPER_SPECIALIZATION_FROMSTRING_BASE(unsigned long long int, std::strtoull, 10);
