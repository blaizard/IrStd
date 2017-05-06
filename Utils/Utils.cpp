#include "../Utils.hpp"

#include <cmath>
#include <limits>

bool IrStd::almostEqual(const double a, const double b) noexcept
{
	return fabs(a - b) < std::numeric_limits<double>::epsilon();
}