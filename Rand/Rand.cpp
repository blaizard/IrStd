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
