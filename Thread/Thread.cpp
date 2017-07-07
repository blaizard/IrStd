#include <iomanip>
#include <thread>

#include "../Thread.hpp"
#include "../Compiler.hpp"

IRSTD_TOPIC_REGISTER(IrStdThread);

// ---- IrStd::Thread ---------------------------------------------------------

IrStd::Thread::Thread(const char* const pName)
		: m_status(Status::IDLE)
		, m_event("ThreadWakeup")
		, m_name(pName)
{
}

IrStd::Thread::~Thread()
{
	IRSTD_ASSERT(!m_thread.joinable(), "This thread (" << getId()
			<< ") has not been joined before its destructor was called");
}

std::thread::id IrStd::Thread::getId() const noexcept
{
	return m_thread.get_id();
}

void IrStd::Thread::sendTerminateSignal() noexcept
{
	m_status = Status::TERMINATING;
	m_event.trigger();
}

void IrStd::Thread::terminate() noexcept
{
	sendTerminateSignal();
	m_thread.join();
}

void IrStd::Thread::start() noexcept
{
	IRSTD_ASSERT(IrStd::Topic::IrStdThread, isIdle(), "The thread must be idle");
	IRSTD_ASSERT(IrStd::Topic::IrStdThread, m_event.getCounter() == 0,
			"The event counter must be null (value=" << m_event.getCounter() << ")");
	m_event.trigger();
	// Wait until the thread is active
	m_event.waitForAtLeast(2);
}

bool IrStd::Thread::signal(const int sig) noexcept
{
#if IRSTD_IS_PLATFORM(LINUX)
	const pthread_t tId = m_thread.native_handle();
	return (pthread_kill(tId, sig) == 0);
#else
	IRSTD_CRASH(IrStd::Topic::IrStdThread, "signal is not supported on this platform ("
			<< IRSTD_PLATFORM_STRING << ")");
#endif
}

bool IrStd::Thread::isTerminated() const noexcept
{
	return m_status == Status::TERMINATED
			|| m_status == Status::TERMINATING;
}

bool IrStd::Thread::isActive() const noexcept
{
	return m_status == Status::ACTIVE;
}

bool IrStd::Thread::isIdle() const noexcept
{
	return m_status == Status::IDLE;
}

bool IrStd::Thread::sleep(const uint64_t timeMs) noexcept
{
	if (isTerminated())
	{
		return false;
	}

	m_event.waitForNext(timeMs);
	return isActive();
}

void IrStd::Thread::wakeup() noexcept
{
	m_event.trigger();
}

const char* IrStd::Thread::getName() const noexcept
{
	return m_name.c_str();
}

size_t IrStd::Thread::getHash(std::thread::id threadId) noexcept
{
	std::hash<std::thread::id> hasher;
	return hasher(threadId);
}


// ---- IrStd::Threads --------------------------------------------------------

bool IrStd::Threads::sleep(const uint64_t timeMs) noexcept
{
	const auto id = std::this_thread::get_id();
	auto pThread = getInstance().get(id);
	IRSTD_ASSERT(IrStd::Topic::IrStdThread, pThread, "The thread (" << id << ") is not registered");
	return pThread->sleep(timeMs);
}

bool IrStd::Threads::isTerminated(std::thread::id id) noexcept
{
	// Check if the thread is registered
	auto& threads = IrStd::Threads::getInstance();
	{
		IRSTD_SCOPE(threads.m_lock);
		auto pThread = threads.getNoLock(id);
		IRSTD_ASSERT(IrStd::Topic::IrStdThread, pThread, "The thread (" << id << ") is not registered");
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
				IRSTD_ASSERT(IrStd::Topic::IrStdThread, pThread, "Thread is not registered");
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
		IRSTD_ASSERT(IrStd::Topic::IrStdThread, pThread, "The thread (" << id << ") is not registered");
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
		case Thread::Status::TERMINATING:
			os << "TERMINATING";
			break;
		case Thread::Status::TERMINATED:
			os << "TERMINATED";
			break;
		default:
			os << "<Unknown>";
		}
		os << std::endl;
	}
}

std::ostream& operator<<(std::ostream& os, const IrStd::Thread& thread)
{
	os << "Thread::" << thread.getName() << "::0x" << std::setw(16) << std::setfill('0')
			<< std::noshowbase << std::hex << IrStd::Thread::getHash(thread.getId());
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::thread::id& threadId)
{
	auto pThread = IrStd::Threads::get(threadId);
	if (pThread)
	{
		os << *pThread;
	}
	else
	{
		os << "Thread::0x" << std::setw(16) << std::setfill('0') << std::noshowbase
				<< std::hex << IrStd::Thread::getHash(threadId);
	}

	return os;
}
