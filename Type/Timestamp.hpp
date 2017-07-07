#pragma once

#include "../Type.hpp"

namespace IrStd
{
	namespace Type
	{
		/**
		 * Timestamp in ms
		 */
		class Timestamp : public NumericImpl<uint64_t>
		{
		public:
			Timestamp() = default;

			template<typename T>
			Timestamp(const T value)
					: NumericImpl<uint64_t>(value)
			{
			}

			template<typename T>
			static Timestamp min(const T value)
			{
				return s(value * 60);
			}

			template<typename T>
			static Timestamp s(const T value)
			{
				return ms(value * 1000);
			}

			template<typename T>
			static Timestamp ms(const T value)
			{
				return Timestamp(value);
			}

			static Timestamp now() noexcept;

			void toStream(std::ostream& os) const;
		};
	}
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::Timestamp& time);
