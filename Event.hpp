#pragma once

#include <array>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "Logger.hpp"
#include "Topic.hpp"

IRSTD_TOPIC_USE(IrStd, Event);

namespace IrStd
{
	/**
	 * \defgroup IrStd-Event
	 * \brief Event mutex
	 * \ingroup IrStd
	 */

	class Event
	{
	public:

		/**
		 * \brief Create an event of a specified name
		 * \ingroup IrStd-Event
		 * \param pName (optional) The name of the event
		 */
		Event(const char* const pName = nullptr);

		/**
		 * \brief Get the current count of the event
		 * \ingroup IrStd-Event
		 * \return The number of times this event has been triggered
		 */
		size_t getCounter() const noexcept;

		/**
		 * Will reset the state of the event and reset its counter.
		 * \note this will trigger the event.
		 * \ingroup IrStd-Event
		 */
		void reset() noexcept;

		/**
		 * \brief Trigger an event and increment the counter.
		 * \ingroup IrStd-Event
		 */
		void trigger() noexcept;

		/**
		 * \brief Wait until the trigger or the timeout if specified.
		 * \ingroup IrStd-Event
		 *
		 * \param timeoutMs Timeout in ms before this function returns
		 *
		 * \return The counter after wakeup or 0 if the timeout has been reached.
		 */
		size_t waitForNext(const uint64_t timeoutMs = 0) const noexcept;

		/**
		 * \brief Wait for at least a minimum number of event
		 * \ingroup IrStd-Event
		 *
		 * \param nbEvents The number of events to wait for
		 * \param timeoutMs (Optional) Timeout in ms before this function returns
		 *
		 * \return The counter after wakeup or 0 if the timeout has been reached.
		 */
		size_t waitForAtLeast(const size_t nbEvents, const uint64_t timeoutMs) noexcept;
		size_t waitForAtLeast(const size_t nbEvents) noexcept;

		/**
		 * \brief Wait until all the events passed into argument are triggered
		 * \ingroup IrStd-Event
		 *
		 * \param timeoutMs The maximum timeout in ms
		 * \param events The events
		 *
		 * \return nullptr if all the events are triggered on time. Otherwise a pointer
		 *         on the event that triggered the timeout, hence a non-null value means
		 *         it has reached the timeout.
		 */
		template<class ...Events>
		static Event* waitForNexts(const uint64_t timeoutMs, Events&... events)
		{
			const std::array<Event* const, sizeof...(Events)> eventList{{&events...}};
			std::array<size_t, sizeof...(Events)> initialCounterList;
			return waitForNextsInternal(timeoutMs, eventList, initialCounterList.data());
		}

		template<class T>
		static Event* waitForNexts(const uint64_t timeoutMs, const T& eventList)
		{
			std::vector<size_t> initialCounterList(eventList.size());
			return waitForNextsInternal(timeoutMs, eventList, initialCounterList.data());
		}

		/**
		 * \brief Return the name associated with this event.
		 * \ingroup IrStd-Event
		 *
		 * \return The name of the event.
		 */
		const char* getName() const noexcept;

	private:

		template<class T>
		static Event* waitForNextsInternal(const uint64_t timeoutMs, const T& eventList, size_t* pInitialCounterList)
		{
			// This operation is not atomic, but it does not matter as during the call itself
			// a counter change can happen.
			for (size_t i=0; i<eventList.size(); ++i)
			{
				pInitialCounterList[i] = eventList[i]->getCounter();
			}

			// Monitor and make sure the next event will happen
			auto timeoutLeft = std::chrono::milliseconds(timeoutMs);
			for (size_t i=0; i<eventList.size(); ++i)
			{
				Event* const pEvent = eventList[i];

				if (pInitialCounterList[i] == pEvent->getCounter())
				{
					const auto timeStart = std::chrono::system_clock::now().time_since_epoch();
					const auto isTimeout = !pEvent->waitForNextInternal(pInitialCounterList[i], timeoutLeft);
					const auto timeSpent = std::chrono::system_clock::now().time_since_epoch() - timeStart;

					// Update the remaining timeout
					if (isTimeout || pEvent->m_counter <= pInitialCounterList[i] || timeoutLeft <= timeSpent)
					{
						IRSTD_LOG_TRACE(IRSTD_TOPIC(IrStd, Event), "Timeout (" << timeoutMs
								<< "ms) for event '" << *pEvent << "'");
						return pEvent;
					}
					timeoutLeft = std::chrono::duration_cast<std::chrono::milliseconds>(timeoutLeft - timeSpent);
				}
			}

			return nullptr;
		}

		template<class Rep, class Period>
		size_t waitForNextInternal(const size_t curCounter,
				const std::chrono::duration<Rep, Period>& timeout) const noexcept
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
						IRSTD_LOG_TRACE(IRSTD_TOPIC(IrStd, Event), "Event (" << getName()
								<< ") was reset, returning from wait state. counter=" << m_counter);
						return 0;
					}
					return m_counter;
				}
			}
			return m_counter;
		}

		size_t waitForNextInternal(const size_t curCounter) const noexcept;

		mutable std::mutex m_lock;
		mutable std::condition_variable m_cv;
		size_t m_counter;
		const char* const m_pName;
	};
}

std::ostream& operator<<(std::ostream& os, const IrStd::Event& event);
