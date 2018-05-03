#include <limits>

#include "Decimal.hpp"
#include "ShortString.hpp"

IRSTD_TOPIC_USE_ALIAS(IrStdType, IrStd, Type);

// ---- IrStd::Decimal -------------------------------------------------------

IrStd::Type::Decimal IrStd::Type::Decimal::getMinPrecision(const size_t precision) noexcept
{
	static const std::array<IrStd::Type::Decimal, 15> precisionList = {
		1, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001, 0.00000001, 0.000000001,
		0.0000000001, 0.00000000001, 0.000000000001, 0.0000000000001, 0.00000000000001
	};
	IRSTD_ASSERT(IrStdType, precision < 15, "Precision is not supported");
	return precisionList[precision];
}

void IrStd::Type::Decimal::toStream(std::ostream& os, const size_t maxPrecision) const
{
	os << ShortString(m_value, maxPrecision);
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::Decimal& numType)
{
	numType.toStream(os);
	return os;
}
