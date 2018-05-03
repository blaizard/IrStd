#pragma once

#include "Numeric.hpp"

namespace IrStd
{
	namespace Type
	{
		/**
		 * Timestamp in ms
		 */
		class Timestamp : public Numeric<uint64_t>
		{
		public:
			template<class ... Args>
			Timestamp(Args&& ... args)
					: IrStd::Type::Numeric<uint64_t>(std::forward<Args>(args)...)
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

			size_t getYears() const noexcept;
			size_t getMonths() const noexcept;
			size_t getDays() const noexcept;
			size_t getHours() const noexcept;
			size_t getMinutes() const noexcept;
			size_t getSeconds() const noexcept;
			size_t getMilliseconds() const noexcept;

			void toStream(std::ostream& os) const;
		};
	}
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::Timestamp& time);
