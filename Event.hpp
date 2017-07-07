#pragma once

#include <array>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "Logger.hpp"
#include "Topic.hpp"

IRSTD_TOPIC_USE(IrStdEvent);

namespace IrStd
{
	class Event
	{
	public:
		Event(const char* const pName = nullptr);

		template<class ...Events>
		static bool waitForNexts(const uint64_t timeoutMs, Events&... events)
		{
			const std::array<Event* const, sizeof...(Events)> pEventList{{&events...}};
			std::array<size_t, sizeof...(Events)> initialCounterList;

			// This operation is not atomic, but it does not matter as during the call itself
			// a counter change can happen.
			for (auto i=0; i<sizeof...(Events); ++i)
			{
				initialCounterList[i] = pEventList[i]->getCounter();
			}

			// Monitor and make sure the next event will happen
			auto timeoutLeft = std::chrono::milliseconds(timeoutMs);
			for (auto i=0; i<sizeof...(Events); ++i)
			{
				Event* const pEvent = pEventList[i];

				if (initialCounterList[i] == pEvent->getCounter())
				{
					const auto timeStart = std::chrono::system_clock::now().time_since_epoch();
					const auto isTimeout = !pEvent->waitForNextInternal(initialCounterList[i], timeoutLeft);
					const auto timeSpent = std::chrono::system_clock::now().time_since_epoch() - timeStart;

					// Update the remaining timeout
					if (isTimeout || pEvent->m_counter <= initialCounterList[i] || timeoutLeft <= timeSpent)
					{
						IRSTD_LOG_TRACE(IrStd::Topic::IrStdEvent, "Timeout (" << timeoutMs
								<< "ms) for event '" << *pEvent << "'");
						return false;
					}
					timeoutLeft = std::chrono::duration_cast<std::chrono::milliseconds>(timeoutLeft - timeSpent);
				}
			}

			return true;
		}

		size_t getCounter() const noexcept;

		/**
		 * Will reset the state of the event and reset its counter.
		 * Note, this will trigger the event.
		 */
		void reset() noexcept;

		/**
		 * \brief Trigger an event and increment the counter.
		 */
		void trigger() noexcept;

		/**
		 * \brief Wait until the trigger or the timeout if specified.
		 *
		 * \return The counter number after wakeup when the trigger has been pulled
		 *         or 0 if the timeout has been reached.
		 */
		size_t waitForNext(const uint64_t timeoutMs = 0) noexcept;

		/**
		 * \brief Wait for at least a minimum number of event
		 */
		size_t waitForAtLeast(const size_t nbEvents, const uint64_t timeoutMs) noexcept;
		size_t waitForAtLeast(const size_t nbEvents) noexcept;

		/**
		 * \brief Return the name associated with this event.
		 */
		const char* getName() const noexcept;

	private:

		template<class Rep, class Period>
		size_t waitForNextInternal(const size_t curCounter,
				const std::chrono::duration<Rep, Period>& timeout) noexcept
		{
			{
				std::unique_lock<std::mutex> lock(m_lock);
				if (m_counter == curCounter)
				{
					auto status = m_cv.wait_for(lock, timeout);
					if (status == std::cv_status::timeout)
					{
						return 0;
					}
					if (m_counter <= curCounter)
					{
						IRSTD_LOG_TRACE(IrStd::Topic::IrStdEvent, "Event (" << getName()
								<< ") was reset, returning from wait state. counter=" << m_counter);
						return 0;
					}
					return m_counter;
				}
			}
			return m_counter;
		}

		size_t waitForNextInternal(const size_t curCounter) noexcept;

		std::mutex m_lock;
		std::condition_variable m_cv;
		size_t m_counter;
		const char* const m_pName;
	};
}

std::ostream& operator<<(std::ostream& os, const IrStd::Event& event);
