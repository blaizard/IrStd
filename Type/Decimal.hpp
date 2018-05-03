#pragma once

#include "Numeric.hpp"

namespace IrStd
{
	namespace Type
	{
		/**
		 * Generic decimal representation of a number.
		 * It extends the type double and adds usefull functionalities like formating
		 * comparison and more.
		 */
		class Decimal : public IrStd::Type::Numeric<double>
		{
		public:
			template<class ... Args>
			Decimal(Args&& ... args)
					: IrStd::Type::Numeric<double>(std::forward<Args>(args)...)
			{
			}

			template<typename T>
			bool operator==(const T v) const
			{
				return fabs(m_value - v) < std::numeric_limits<double>::epsilon();
			}

			/**
			 * Return the minimal precision of a decimal.
			 * For example if precision is 2, it will return 0.01
			 */
			static IrStd::Type::Decimal getMinPrecision(const size_t precision) noexcept;

			void toStream(std::ostream& os, const size_t maxPrecision = 6) const;
		};
	}
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::Decimal& numType);
