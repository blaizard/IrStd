#pragma once

#include "Numeric.hpp"

namespace IrStd
{
	namespace Type
	{
		/**
		 * Memory representation (in bytes)
		 */
		class Memory : public Numeric<uint64_t>
		{
		public:
			Memory() noexcept = default;

			template<typename T>
			Memory(const T value, const bool isBit = false) noexcept
					: Numeric<uint64_t>((isBit) ? value : value * 8)
			{
			}
			constexpr operator uint64_t() const noexcept
			{
				return m_value / 8;
			}

			template<typename T>
			static Memory bit(const T value) noexcept
			{
				return Memory(value, /*isBit*/true);
			}

			template<typename T>
			static Memory byte(const T value) noexcept
			{
				return Memory(value * 8);
			}

			template<typename T>
			static Memory Kbyte(const T value) noexcept
			{
				return byte(value * 1024);
			}

			template<typename T>
			static Memory Mbyte(const T value) noexcept
			{
				return Kbyte(value * 1024);
			}

			template<typename T>
			static Memory Gbyte(const T value) noexcept
			{
				return Mbyte(value * 1024);
			}

			template<typename T>
			static Memory Tbyte(const T value) noexcept
			{
				return Gbyte(value * 1024);
			}

			void toStream(std::ostream& os) const;
		};
	}
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::Memory& mem);
