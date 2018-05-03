#include "Stopwatch.hpp"

// ---- IrStd::Type::Stopwatch::Counter ---------------------------------------

IrStd::Type::Stopwatch::Counter::Counter()
		: m_value(0)
{
}


uint64_t IrStd::Type::Stopwatch::Counter::getNs() const noexcept
{
	return m_value.count();
}

uint64_t IrStd::Type::Stopwatch::Counter::getUs() const noexcept
{
	return m_value.count() / 1000;
}

uint64_t IrStd::Type::Stopwatch::Counter::getMs() const noexcept
{
	return m_value.count() / (1000 * 1000);
}

uint64_t IrStd::Type::Stopwatch::Counter::getS() const noexcept
{
	return m_value.count() / (1000 * 1000 * 1000);
}

// ---- IrStd::Type::Stopwatch ------------------------------------------------

IrStd::Type::Stopwatch::Stopwatch(const bool autoStart)
		: m_pCounter(nullptr)
		, m_running(false)
{
	if (autoStart)
	{
		start();
	}
}

IrStd::Type::Stopwatch::Stopwatch(Counter& counter, const bool autoStart)
		: m_pCounter(&counter)
{
	if (autoStart)
	{
		start();
	}
}

IrStd::Type::Stopwatch::~Stopwatch()
{
	if (m_running)
	{
		stop();
	}
}

void IrStd::Type::Stopwatch::start() noexcept
{
	m_running = true;
	m_start = std::chrono::steady_clock::now();
}

IrStd::Type::Stopwatch::Counter IrStd::Type::Stopwatch::get() const noexcept
{
	return Counter(std::chrono::steady_clock::now() - m_start);
}

IrStd::Type::Stopwatch::Counter IrStd::Type::Stopwatch::stop() noexcept
{
	const auto diff = std::chrono::steady_clock::now() - m_start;
	m_running = false;

	if (m_pCounter)
	{
		m_pCounter->m_value += diff;
	}

	return Counter(diff);
}
