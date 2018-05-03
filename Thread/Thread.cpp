#include <iomanip>
#include <thread>

#include "../Thread.hpp"
#include "../Compiler.hpp"

IRSTD_TOPIC_REGISTER(IrStd, Thread);
IRSTD_TOPIC_USE_ALIAS(IrStdThread, IrStd, Thread);

// ---- IrStd::Thread ---------------------------------------------------------

IrStd::Thread::Thread(const char* const pName)
		: m_status(Status::IDLE)
		, m_isTerminate(false)
		, m_event("ThreadWakeup")
		, m_name(pName)
{
}

IrStd::Thread::~Thread()
{
	IRSTD_ASSERT(!m_thread.joinable(), "Thread::0x" << std::setw(16)
			<< std::setfill('0') << std::noshowbase << std::hex
			<< getHash() << " has not been joined before its destructor was called");
}

std::thread::id IrStd::Thread::getId() const noexcept
{
	return m_thread.get_id();
}

void IrStd::Thread::sendTerminateSignal() noexcept
{
	m_isTerminate = true;
	m_event.trigger();
}

void IrStd::Thread::terminate() noexcept
{
	sendTerminateSignal();
	m_thread.join();
}

void IrStd::Thread::start() noexcept
{
	IRSTD_ASSERT(IrStdThread, isIdle(), "The thread must be idle");
	IRSTD_ASSERT(IrStdThread, m_event.getCounter() == 0,
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
	IRSTD_CRASH(IrStdThread, "signal is not supported on this platform ("
			<< IRSTD_PLATFORM_STRING << ")");
#endif
}

bool IrStd::Thread::isTerminated() const noexcept
{
	return m_isTerminate || m_status == Status::TERMINATED;
}

bool IrStd::Thread::isActive() const noexcept
{
	return m_status == Status::ACTIVE;
}

bool IrStd::Thread::isIdle() const noexcept
{
	return m_status == Status::IDLE;
}

bool IrStd::Thread::setIdle() noexcept
{
	Status expected = Status::ACTIVE;
	return m_status.compare_exchange_weak(expected, Status::IDLE);
}

bool IrStd::Thread::setActive() noexcept
{
	Status expected = Status::IDLE;
	return m_status.compare_exchange_weak(expected, Status::ACTIVE);
}

bool IrStd::Thread::sleep(const uint64_t timeMs) noexcept
{
	if (isTerminated())
	{
		return false;
	}

	setIdle();
	m_event.waitForNext(timeMs);
	setActive();

	return !isTerminated();
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

std::ostream& operator<<(std::ostream& os, const IrStd::Thread& thread)
{
	os << "Thread::" << thread.getName() << "::0x" << std::setw(16) << std::setfill('0')
			<< std::noshowbase << std::hex << IrStd::Thread::getHash(thread.getId()) << std::dec;
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
				<< std::hex << IrStd::Thread::getHash(threadId) << std::dec;
	}

	return os;
}
