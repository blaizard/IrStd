#include "../Rand.hpp"

IrStd::Rand::Rand()
		: Rand(generateSeed())
{
}

IrStd::Rand::Rand(const Seed seed)
		: m_gen(0)
{
	setSeed(seed);
}

IrStd::Rand::Seed IrStd::Rand::generateSeed() noexcept
{
	static Seed random = 1;
	std::random_device rd;
	return rd() * random + random;
}

void IrStd::Rand::setSeed(Seed seed) noexcept
{
	m_seed = seed;
	m_gen.seed(seed);
}

IrStd::Rand::Seed IrStd::Rand::getSeed() const noexcept
{
	return m_seed;
}

bool IrStd::Rand::getBool() noexcept
{
	return (getNumber<int>(0, 1) == 1) ? true : false;
}

template<>
float IrStd::Rand::getNumber<float>(const float min, const float max) noexcept
{
	const auto rand = static_cast<float>(getNumber<uint64_t>()) / static_cast<float>(std::numeric_limits<uint64_t>::max());
	return min + rand * (max - min);
}

template<>
double IrStd::Rand::getNumber<double>(const double min, const double max) noexcept
{
	const auto rand = static_cast<double>(getNumber<uint64_t>()) / static_cast<double>(std::numeric_limits<uint64_t>::max());
	return min + rand * (max - min);
}

