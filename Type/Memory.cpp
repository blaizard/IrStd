#include <iomanip>

#include "Memory.hpp"
#include "ShortString.hpp"

// ---- IrStd::Memory ---------------------------------------------------------

void IrStd::Type::Memory::toStream(std::ostream& os) const
{
	constexpr auto COEF_LIMIT = 0.9;
	auto value = m_value;

	// Means there are some bits set
	if ((value & 0x7) && value < 129)
	{
		os << value << ((value > 1) ? ".bits" : ".bit");
		return;
	}
	value /= 8;

	if (value == 0)
	{
		os << "0";
	}
	else if (value < static_cast<uint64_t>(1024llu * COEF_LIMIT))
	{
		os << value << ((value > 1) ? ".bytes" : ".byte");
	}
	else if (value < static_cast<uint64_t>(1024llu * 1024llu * COEF_LIMIT))
	{
		os << IrStd::Type::ShortString(static_cast<double>(value) / 1024., 2) << ".KB";
	}
	else if (value < static_cast<uint64_t>(1024llu * 1024llu * 1024llu * COEF_LIMIT))
	{
		os << IrStd::Type::ShortString(static_cast<double>(value) / (1024. * 1024.), 2) << ".MB";
	}
	else if (value < static_cast<uint64_t>(1024llu * 1024llu * 1024llu * 1024llu * COEF_LIMIT))
	{
		os << IrStd::Type::ShortString(static_cast<double>(value) / (1024. * 1024. * 1024.), 2) << ".GB";
	}
	else
	{
		os << IrStd::Type::ShortString(static_cast<double>(value) / (1024. * 1024. * 1024. * 1024.), 2) << ".TB";
	}
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::Memory& mem)
{
	mem.toStream(os);
	return os;
}
