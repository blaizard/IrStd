#pragma once

#include <mutex>

#include "Assert.hpp"

namespace IrStd
{
	class RWLock
	{
	public:
		RWLock()
				: m_counter(0)
		{
		};

		class Scope
		{
		public:
			Scope(RWLock& lock)
					: m_lock(lock)
					, m_owner(true)
			{
			}
			Scope(const Scope& scope) = delete;
			Scope(Scope&& scope)
					: Scope(scope.m_lock)
			{
				scope.m_owner = false;
			}

		protected:
			RWLock& m_lock;
			bool m_owner;
		};

		class ReaderScope : public Scope
		{
		public:
			ReaderScope(RWLock& lock)
					: Scope(lock)
			{
			}
			ReaderScope(ReaderScope&& scope)
					: Scope(std::move(scope))
			{
			}
			void release()
			{
				if (m_owner)
				{
					std::unique_lock<std::mutex> lock(m_lock.m_mutex);
					m_lock.m_counter--;
					m_lock.m_conditionVariable.notify_all();
					m_owner = false;
				}
			}
			~ReaderScope()
			{
				release();
			}
		};

		class WriterScope : public Scope
		{
		public:
			WriterScope(RWLock& lock)
					: Scope(lock)
			{
			}
			WriterScope(WriterScope&& scope)
					: Scope(std::move(scope))
			{
			}
			void release()
			{
				if (m_owner)
				{
					std::unique_lock<std::mutex> lock(m_lock.m_mutex);
					m_lock.m_counter++;
					m_lock.m_conditionVariable.notify_all();
					m_owner = false;
				}
			}
			~WriterScope()
			{
				release();
			}
		};

		ReaderScope readScope()
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			IRSTD_ASSERT(m_counter.load() >= -1);

			// Get out of this loop only if m_counter > 0
			{
				int64_t expected;
				do
				{
					m_conditionVariable.wait(lock, [&]() {
						expected = m_counter.load();
						return (expected >= 0);
					});
				} while (!m_counter.compare_exchange_strong(expected, expected + 1));
			}

			IRSTD_ASSERT(isReadScope());

			return std::move(ReaderScope(*this));
		}

		WriterScope writeScope()
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			IRSTD_ASSERT(m_counter.load() >= -1);

			// Make sure only 1 write is here at a time
			{
				int64_t expected;
				do
				{
					expected = 0;
					m_conditionVariable.wait(lock, [&]() {
						return (m_counter.load() == 0);
					});
				} while (!m_counter.compare_exchange_strong(expected, -1));
			}

			IRSTD_ASSERT(isWriteScope());

			return std::move(WriterScope(*this));
		}

		bool isWriteScope() const noexcept
		{
			return (m_counter.load() == -1);
		}

		bool isReadScope() const noexcept
		{
			return (m_counter.load() > 0);
		}

		bool isScope() const noexcept
		{
			return (m_counter.load() != 0);
		}

		// For testing purpose
		int64_t getCounter() const noexcept
		{
			return m_counter.load();
		}

	private:
		friend ReaderScope;
		friend WriterScope;

		std::mutex m_mutex;
		// -1 = 1 write
		// 0 = nothing
		// >0 = multiple readers
		std::atomic<int64_t> m_counter;
		std::condition_variable m_conditionVariable;
	};
}
