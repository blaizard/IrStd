#pragma once

#include <thread>
#include <map>
#include <mutex>
#include <memory>
#include <future>
#include <atomic>

#include "Logger.hpp"
#include "Topic.hpp"
#include "Assert.hpp"
#include "Utils.hpp"
#include "Event.hpp"
#include "Bootstrap.hpp"
#include "Scope.hpp"

IRSTD_TOPIC_USE(IrStd, Thread);

namespace IrStd
{
	class Threads;
	class Thread;
	typedef std::shared_ptr<Thread> ThreadPtr;

	class Thread
	{
	public:
		enum class Status
		{
			IDLE = 0,
			ACTIVE,
			TERMINATED
		};

		Thread(const char* const pName = "<unamed>");
		~Thread();

		/**
		 * \brief Create a thread
		 */
		template<class Function, class ... Args>
		static ThreadPtr create(const char* const pName, Function&& f, Args&& ... args)
		{
			auto pThread = createIdle(pName, std::forward<Function>(f), std::forward<Args>(args)...);
			pThread->start();
			return std::move(pThread);
		}

		std::thread::id getId() const noexcept;

		void sendTerminateSignal() noexcept;

		void terminate() noexcept;

		/**
		 * Send a signal to the thread
		 */
		bool signal(const int sig) noexcept;

		bool isActive() const noexcept;
		bool isTerminated() const noexcept;
		bool isIdle() const noexcept;

		bool setIdle() noexcept;
		bool setActive() noexcept;

		/**
		 * \brief Put a thread to sleep
		 *
		 * A thread in sleep can wake up after the timeout or a \ref wakeup call,
		 * in this case the function will return true. If \ref terminate is called
		 * the sleep function returns immediatly and return false.
		 *
		 * Common pratcice, is to have a loop in the thread as follow:
		 * do
		 * {
		 * 		[...]
		 * } while (thread.sleep(1000));
		 *
		 * \param timeMs Time in milliseconds.
		 */
		bool sleep(const uint64_t timeMs) noexcept;

		/**
		 * \brief Wake up a thread
		 */
		void wakeup() noexcept;

		/**
		 * \brief Get the name of the thread
		 */
		const char* getName() const noexcept;

		/**
		 * Return a hash of the thread id
		 */
		static size_t getHash(std::thread::id threadId = std::this_thread::get_id()) noexcept;

	private:
		friend IrStd::Threads;
		friend IrStd::Bootstrap;

		/**
		 * Start a thread set to idle
		 */
		void start() noexcept;

		template<class Function, class ... Args>
		static ThreadPtr createIdle(const char* const pName, Function&& f, Args&& ... args)
		{
			auto pThread = std::make_shared<Thread>(pName);

			auto fct = std::bind(std::forward<Function>(f), std::forward<Args>(args)...);
			pThread->m_thread = std::thread(threadWrapper<decltype(fct)>, pThread.get(), fct);

			return std::move(pThread);
		}

		template<class Function>
		static void threadWrapper(Thread* pThread, Function fct)
		{
			// Here the object or reference have been passed to the thread, hence release
			// the current application
			pThread->m_event.waitForAtLeast(1);
			IRSTD_ASSERT(IRSTD_TOPIC(IrStd, Thread), pThread->m_event.getCounter() == 1,
					"The event counter must be equal to 1 (value="
					<< pThread->m_event.getCounter() << ")");

			// Set the status to active
			pThread->m_status = Status::ACTIVE;
			pThread->m_event.trigger();

			IRSTD_LOG_TRACE(IRSTD_TOPIC(IrStd, Thread), "Starting " << std::this_thread::get_id()
					<< " (" << pThread->getName() << ")");

			fct();

			pThread->m_status = Status::TERMINATED;
			IRSTD_LOG_TRACE(IRSTD_TOPIC(IrStd, Thread), "Terminating " << std::this_thread::get_id()
					<< " (" << pThread->getName() << ")");
		}

		std::thread m_thread;
		std::atomic<Status> m_status;
		bool m_isTerminate;
		Event m_event;
		std::string m_name;
	};

	class Threads : public SingletonImpl<Threads>
	{
	public:
		template<class Function, class ... Args>
		static std::thread::id create(const char* const pName, Function&& f, Args&& ... args)
		{
			auto pThread = IrStd::Thread::createIdle(
					pName, std::forward<Function>(f), std::forward<Args>(args)...);

			const auto id = pThread->getId();
			// Save the thread information
			{
				IRSTD_SCOPE(getInstance().m_lock);
				auto& map = getInstance().m_threadMap;
				IRSTD_ASSERT(map.find(id) == map.end(), "Thread (" << id << ") is already registered");
				map[id] = pThread;
			}

			// Start the thread
			pThread->start();

			return id;
		}

		/**
		 * Iterate through all registered threads
		 */
		static void each(const std::function<void(const Thread&)>& callback) noexcept;

		/**
		 * \brief Let the current thread sleep for a specified amount of time.
		 *
		 * \note If terminate for this thread is called, it will wake up
		 *       immediatly and return false.
		 */
		static bool sleep(const uint64_t timeMs) noexcept;

		/**
		 * \brief Terminate a specific or all threads
		 *
		 * \param [id] The id of the thread to be terminated. If no
		 *             argument, all registered threads will be
		 *             terminated.
		 */
		static void terminate(std::thread::id id = std::thread::id()) noexcept;

		/**
		 * \brief Assess if a thread is registered
		 *
		 * \param [id] The id of the thread to probe. If no argument
		 *             is provided, is will check the status of the
		 *             current thread.
		 *
		 * \return true if the thread is registered, false otherwise.
		 */
		static bool isRegistered(std::thread::id id = std::this_thread::get_id()) noexcept;

		/**
		 * \brief Assess if a registered thread is terminated
		 *
		 * \param [id] The id of the thread to probe. If no argument
		 *             is provided, is will check the status of the
		 *             current thread.
		 *
		 * \return true if the thread is terminated,
		 *         false otherwise.
		 */
		static bool isTerminated(std::thread::id id = std::this_thread::get_id()) noexcept;
		static bool isActive(std::thread::id id = std::this_thread::get_id()) noexcept;

		/**
		 * \brief Set the registered thread as idle
		 *
		 * \param [id] The id of the thread to probe. If no argument
		 *             is provided, is will check the status of the
		 *             current thread.
		 *
		 * \return true if the thread is set to idle,
		 *         false otherwise.
		 */
		static bool setIdle(std::thread::id id = std::this_thread::get_id()) noexcept;

		/**
		 * \brief Set the registered thread as active
		 *
		 * \param [id] The id of the thread to probe. If no argument
		 *             is provided, is will check the status of the
		 *             current thread.
		 *
		 * \return true if the thread is set to active,
		 *         false otherwise.
		 */
		static bool setActive(std::thread::id id = std::this_thread::get_id()) noexcept;

		/**
		 * \brief Return the Thread object idenditfied by its id
		 * or a nullptr if it is not registered
		 */
		static ThreadPtr get(std::thread::id id = std::this_thread::get_id()) noexcept;

		static void toStream(std::ostream& os);

	private:
		friend Bootstrap;

		ThreadPtr getNoLock(std::thread::id id = std::this_thread::get_id()) noexcept;

		void toStreamImpl(std::ostream& os);

		std::map<std::thread::id, std::shared_ptr<Thread>> m_threadMap;
		IRSTD_SCOPE_LOCK_THREAD_REGISTER(m_lock);
	};
}

std::ostream& operator<<(std::ostream& os, const IrStd::Thread& thread);
std::ostream& operator<<(std::ostream& os, const std::thread::id& threadId);

// Extra implementation
#include "Thread/ThreadPool.hpp"
