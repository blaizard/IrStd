#include <iomanip>
#include <thread>

#include "../Thread.hpp"
#include "../Compiler.hpp"

IRSTD_TOPIC_USE_ALIAS(IrStdThread, IrStd, Thread);

// ---- IrStd::Threads --------------------------------------------------------

void IrStd::Threads::each(const std::function<void(const Thread&)>& callback) noexcept
{
	auto& threads = IrStd::Threads::getInstance();
	{
		IRSTD_SCOPE(threads.m_lock);
		for (auto& item : threads.m_threadMap)
		{
			callback(*item.second);
		}
	}
}

bool IrStd::Threads::sleep(const uint64_t timeMs) noexcept
{
	const auto id = std::this_thread::get_id();
	auto pThread = getInstance().get(id);
	IRSTD_ASSERT(IrStdThread, pThread, "The thread (" << id << ") is not registered");
	return pThread->sleep(timeMs);
}

bool IrStd::Threads::isTerminated(std::thread::id id) noexcept
{
	// Check if the thread is registered
	auto& threads = IrStd::Threads::getInstance();
	{
		IRSTD_SCOPE(threads.m_lock);
		auto pThread = threads.getNoLock(id);
		IRSTD_ASSERT(IrStdThread, pThread, "The thread (" << id << ") is not registered");
		return pThread->isTerminated();
	}
}

bool IrStd::Threads::isActive(std::thread::id id) noexcept
{
	return !isTerminated(id);
}

bool IrStd::Threads::isRegistered(std::thread::id id) noexcept
{
	auto& threads = IrStd::Threads::getInstance();
	{
		IRSTD_SCOPE(threads.m_lock);
		return threads.m_threadMap.find(id) != threads.m_threadMap.end();
	}
}

bool IrStd::Threads::setIdle(std::thread::id id) noexcept
{
	auto& threads = IrStd::Threads::getInstance();
	{
		IRSTD_SCOPE(threads.m_lock);
		auto pThread = threads.getNoLock(id);
		IRSTD_ASSERT(IrStdThread, pThread, "The thread (" << id << ") is not registered");
		return pThread->setIdle();
	}
}

bool IrStd::Threads::setActive(std::thread::id id) noexcept
{
	auto& threads = IrStd::Threads::getInstance();
	{
		IRSTD_SCOPE(threads.m_lock);
		auto pThread = threads.getNoLock(id);
		IRSTD_ASSERT(IrStdThread, pThread, "The thread (" << id << ") is not registered");
		return pThread->setActive();
	}
}

void IrStd::Threads::terminate(std::thread::id id) noexcept
{
	auto& threads = IrStd::Threads::getInstance();
	if (id == std::thread::id())
	{
		// Send terminate signals to all threads
		{
			IRSTD_SCOPE(threads.m_lock);
			for (auto& item : threads.m_threadMap)
			{
				item.second->sendTerminateSignal();
			}
		}

		// Wait until their completion
		{
			while (true)
			{
				IrStd::ThreadPtr pThread; 
				std::thread::id threadId;
				{
					IRSTD_SCOPE(threads.m_lock);
					if (threads.m_threadMap.empty())
					{
						break;
					}
					const auto it = threads.m_threadMap.begin();
					threadId = it->first;
					pThread = it->second;
				}

				// Terminate the thread
				IRSTD_ASSERT(IrStdThread, pThread, "Thread is not registered");
				pThread->terminate();

				// Remove it from the list only once terminated
				{
					IRSTD_SCOPE(threads.m_lock);
					threads.m_threadMap.erase(threadId);
				}
			}
		}
	}
	else
	{
		auto pThread = threads.get(id);
		IRSTD_ASSERT(IrStdThread, pThread, "The thread (" << id << ") is not registered");
		pThread->terminate();
		{
			IRSTD_SCOPE(threads.m_lock);
			threads.m_threadMap.erase(id);
		}
	}
}

void IrStd::Threads::toStream(std::ostream& os)
{
	IrStd::Threads::getInstance().toStreamImpl(os);
}

std::shared_ptr<IrStd::Thread> IrStd::Threads::getNoLock(std::thread::id id) noexcept
{
	auto it = m_threadMap.find(id);
	return (it == m_threadMap.end()) ? nullptr : it->second;
}

std::shared_ptr<IrStd::Thread> IrStd::Threads::get(std::thread::id id) noexcept
{
	IRSTD_SCOPE(IrStd::Threads::getInstance().m_lock);
	return IrStd::Threads::getInstance().getNoLock(id);
}

void IrStd::Threads::toStreamImpl(std::ostream& os)
{
	IRSTD_SCOPE(m_lock);
	const auto nbThreads = m_threadMap.size();

	os << "IrStd::Threads registered: " << nbThreads << std::endl;

	for (auto& item : m_threadMap)
	{
		os << "\t" << *item.second << ", status=";
		switch (item.second->m_status)
		{
		case Thread::Status::IDLE:
			os << "IDLE";
			break;
		case Thread::Status::ACTIVE:
			os << "ACTIVE";
			break;
		case Thread::Status::TERMINATED:
			os << "TERMINATED";
			break;
		default:
			os << "<Unknown>";
		}
		if (item.second->m_isTerminate)
		{
			os << ", terminate=true";
		}
		os << std::endl;
	}
}
