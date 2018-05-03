#include "../Event.hpp"
#include "../Thread.hpp"

IRSTD_TOPIC_REGISTER(IrStd, Event);
IRSTD_TOPIC_USE_ALIAS(IrStdEvent, IrStd, Event);

// ---- IrStd::Event ----------------------------------------------------------

IrStd::Event::Event(const char* const pName)
		: m_counter(0)
		, m_pName(pName)
{
}

size_t IrStd::Event::getCounter() const noexcept
{
	return m_counter;
}

void IrStd::Event::reset() noexcept
{
	trigger();
	m_counter = 0;
}

void IrStd::Event::trigger() noexcept
{
	std::unique_lock<std::mutex> lock(m_lock);
	m_counter++;
	m_cv.notify_all();
}

size_t IrStd::Event::waitForNext(const uint64_t timeoutMs) const noexcept
{
	if (timeoutMs)
	{
		return waitForNextInternal(m_counter, std::chrono::milliseconds(timeoutMs));
	}
	return waitForNextInternal(m_counter);
}

size_t IrStd::Event::waitForAtLeast(const size_t nbMinEvents, const uint64_t timeoutMs) noexcept
{
	// If timeoutMs == 0, then do not include the timeout
	if (timeoutMs == 0)
	{
		return IrStd::Event::waitForAtLeast(nbMinEvents);
	}

	auto timeoutLeft = std::chrono::milliseconds(timeoutMs);
	auto curCounter = m_counter;
	while (curCounter < nbMinEvents)
	{
		const auto timeStart = std::chrono::system_clock::now().time_since_epoch();
		curCounter = waitForNextInternal(curCounter, timeoutLeft);
		const auto timeSpent = std::chrono::system_clock::now().time_since_epoch() - timeStart;

		// Update the remaining timeout
		if (curCounter == 0 || timeoutLeft <= timeSpent)
		{
			return 0;
		}
		timeoutLeft = std::chrono::duration_cast<std::chrono::milliseconds>(timeoutLeft - timeSpent);
	}
	return m_counter;
}

size_t IrStd::Event::waitForAtLeast(const size_t nbMinEvents) noexcept
{
	auto curCounter = m_counter;
	while (curCounter < nbMinEvents)
	{
		curCounter = waitForNextInternal(curCounter);
		if (curCounter == 0)
		{
			return 0;
		}
	}
	return m_counter;
}

size_t IrStd::Event::waitForNextInternal(const size_t curCounter) const noexcept
{
	{
		std::unique_lock<std::mutex> lock(m_lock);
		if (curCounter == m_counter)
		{
			m_cv.wait(lock);
			if (m_counter <= curCounter)
			{
				IRSTD_LOG_TRACE(IrStdEvent, "Event (" << getName()
						<< ") was reset, returning from wait state. counter=" << m_counter);
				return 0;
			}
		}
	}
	return m_counter;
}

const char* IrStd::Event::getName() const noexcept
{
	return (m_pName) ? m_pName : "<unamed>";
}

std::ostream& operator<<(std::ostream& os, const IrStd::Event& event)
{
	os << "Event::" << event.getName();
	return os;
}
