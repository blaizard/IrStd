#pragma once

#include "Assert.hpp"

#include <cstdint>
#include <limits>
#include <random>

namespace IrStd
{
	class Rand
	{
	public:
		typedef uint32_t Seed;

		/**
		 * Constructors
		 */
		Rand();
		Rand(const Seed seed);

		static Seed generateSeed() noexcept;

		void setSeed(Seed seed) noexcept;
		Seed getSeed() const noexcept;

		/**
		 * Random functions
		 */
		bool getBool() noexcept;

		/**
		 * \brief Generates a random number from min to max (inclusive)
		 */
		template<class T>
		T getNumber(const T min = std::numeric_limits<int>::min(), const T max = std::numeric_limits<int>::max()) noexcept
		{
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(m_gen);
		}

		/**
		 * \brief Get a random item from an object
		 */
		template<class T>
		T& getVectorItem(std::vector<T>& vector)
		{
			IRSTD_ASSERT(vector.size() > 0);
			const auto idx = getNumber<size_t>(0, vector.size() - 1);
			return vector.at(idx);
		}

	private:
		typedef std::mt19937 Generator;

		Generator m_gen;
		Seed m_seed;
	};
}