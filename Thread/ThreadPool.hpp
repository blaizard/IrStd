#pragma once

#include <array>
#include <queue>
#include <mutex>

#include "../Assert.hpp"
#include "../Thread.hpp"
#include "../Type.hpp"

namespace IrStd
{
	template<size_t N>
	class ThreadPool
	{
	public:
		ThreadPool(const std::string& name)
		{
			// Initialize the threads
			for (size_t i=0; i<N; ++i)
			{
				std::string workerName(name);
				workerName += "::";
				workerName += IrStd::Type::ShortString(i + 1);
				m_workerList[i] = IrStd::Threads::create(workerName.c_str(), &ThreadPool<N>::process, this);
			}
		}

		~ThreadPool()
		{
			// Delete existing workers
			for (size_t i=0; i<N; ++i)
			{
				auto pThread = IrStd::Threads::get(m_workerList[i]);
				pThread->sendTerminateSignal();
				m_condition.notify_all();
				pThread->terminate();
			}
		}

		/**
		 * Add a new job to the list
		 */
		void addJob(const std::function<void()>& job)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_jobList.push(job);
			m_condition.notify_one();
		}

		size_t getNbPendingJobs() const noexcept
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			return m_jobList.size();
		}

		/**
		 * Wait until all pending jobs are completed. This function
		 * will return only when there are no pending jobs left in the queue.
		 */
		void waitForAllJobsToBeCompleted() noexcept
		{
			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			m_triggerComplete.wait(lock, [&]() {
				return getNbPendingJobs() == 0;
			});
		}

	private:
		void process()
		{
			auto pThread = IrStd::Threads::get();

			// Infinite loop
			while (true)
			{
				std::function<void()> job;
				pThread->setIdle();

				// Grab the first job from the list
				{
					std::unique_lock<std::mutex> lock(m_mutex);
					m_condition.wait(lock, [&]() {
						return !m_jobList.empty() || IrStd::Threads::isTerminated();
					});
					if (IrStd::Threads::isTerminated())
					{
						break;
					}
					IRSTD_ASSERT(!m_jobList.empty());
					job = m_jobList.front();
					m_jobList.pop();
				}

				// Execute the function
				pThread->setActive();
				job();
				m_triggerComplete.notify_one();
			}
		}

		mutable std::mutex m_mutex;
		std::array<std::thread::id, N> m_workerList;
		std::queue<std::function<void()>> m_jobList;
		std::condition_variable m_condition;
		std::condition_variable m_triggerComplete;
	};
}
