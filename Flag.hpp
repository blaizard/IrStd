#pragma once

#include <mutex>
#include <thread>

namespace IrStd
{
	namespace Flag
	{
	}

	// ---- FlagImpl (interface) ----------------------------------------------

	class FlagInterface
	{
	public:
		virtual bool isSet() const noexcept = 0;
		virtual bool unsetAndGet() noexcept = 0;
		virtual bool setAndGet() noexcept = 0;
	};

	// ---- FlagBool ----------------------------------------------------------

	/**
	 * \brief Set or unset a flag atomically.
	 */
	class FlagBool : public FlagInterface
	{
	public:
		FlagBool()
				: m_flag(ATOMIC_FLAG_INIT)
		{
		}

		bool isSet() const noexcept final
		{
			return m_flag;
		}

		bool unsetAndGet() noexcept
		{
			const bool prev = m_flag.exchange(false);
			return prev;
		}

		bool setAndGet() noexcept
		{
			const bool prev = m_flag.exchange(true);
			return prev;
		}

	private:
		std::atomic<bool> m_flag;
	};

	// ---- FlagLock ----------------------------------------------------------

	/**
	 * \brief Act like a mutex.
	 */
	class FlagLock : public FlagInterface
	{
	public:
		FlagLock()
				: m_lock(m_mutex, std::defer_lock)
		{
		}

		bool isSet() const noexcept final
		{
			return m_lock.owns_lock();
		}

		virtual bool unsetAndGet() noexcept
		{
			m_lock.unlock();
			return true;
		}

		virtual bool setAndGet() noexcept
		{
			m_lock.lock();
			return false;
		}

	protected:
		std::mutex m_mutex;
		std::unique_lock<std::mutex> m_lock;
	};

	// ---- FlagLockThread ----------------------------------------------------

	/**
	 * \brief Similar to \ref FlagLock but locks only if the lock has been acquired
	 *        in a different thread.
	 */
	class FlagLockThread : public FlagInterface
	{
	public:
		FlagLockThread()
				: m_threadId(0)
		{
		}

		bool isSet() const noexcept
		{
			return (m_threadId == std::thread::id(0)) ? false : true;
		}

		bool unsetAndGet() noexcept
		{
			if (isSet())
			{
				m_threadId = std::thread::id(0);
				m_mutex.unlock();
				return true;
			}
			return false;
		}

		bool setAndGet() noexcept
		{
			// Is set and to the same thread
			if (m_threadId == std::this_thread::get_id())
			{
				return true;
			}

			m_mutex.lock();
			m_threadId = std::this_thread::get_id();
			return false;
		}

	private:
		std::thread::id m_threadId;
		std::mutex m_mutex;
	};
}
