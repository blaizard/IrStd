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
		T getNumber(const T min = std::numeric_limits<T>::min(), const T max = std::numeric_limits<T>::max()) noexcept
		{
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(m_gen);
		}

		/**
		 * \brief Get a random item from an object
		 */
		template<class T>
		typename T::value_type& getArrayItem(T& array)
		{
			IRSTD_ASSERT(array.size() > 0);
			const auto idx = getNumber<size_t>(0, array.size() - 1);
			return array.at(idx);
		}

	private:
		typedef std::mt19937 Generator;

		Generator m_gen;
		Seed m_seed;
	};

	// Specialization
	template<>
	float Rand::getNumber<float>(const float min, const float max) noexcept;
	template<>
	double Rand::getNumber<double>(const double min, const double max) noexcept;
}