#include <signal.h>
#include <execinfo.h>
#include <iostream>
#include <mutex>
#include <csignal>
#include <iomanip>
#include <thread>

#include <cxxabi.h>
#include <execinfo.h>

#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <ucontext.h>

#ifdef __x86_64__
#define REG_EIP REG_RIP
#endif

#include "../Bootstrap.hpp"
#include "../Exception.hpp"
#include "../Logger.hpp"
#include "../Topic.hpp"
#include "../Thread.hpp"
#include "../Memory.hpp"

#define SIGNAL_THREAD SIGUSR2

IRSTD_TOPIC_REGISTER(IrStdBootstrap, "IrStdBoot");
IRSTD_SCOPE_USE(IrStdMemoryNoTrace);

IrStd::Bootstrap::Bootstrap()
		: m_loggerStreamBuf(10 * 1024)
		, m_loggerStream(&m_loggerStreamBuf)
{
	m_sa.sa_sigaction = static_cast<void (*)(int, siginfo_t*, void*)>(Bootstrap::sigHandler);
	sigemptyset(&m_sa.sa_mask);
	m_sa.sa_flags = SA_RESTART | SA_SIGINFO;

	// Register handlers for all known signals
	for (const auto& signal : {
			SIGABRT, SIGALRM, SIGBUS, SIGCHLD, SIGCONT, SIGFPE, SIGHUP, SIGILL,
			SIGINT, SIGKILL, SIGPIPE, SIGQUIT, SIGSEGV, SIGSTOP, SIGTERM, SIGTSTP,
			SIGTTIN, SIGTTOU, SIGUSR1, SIGUSR2, SIGPOLL, SIGPROF, SIGSYS, SIGTRAP,
			SIGURG, SIGVTALRM, SIGXCPU, SIGXFSZ})
	{
		sigaction(signal, &m_sa, nullptr);
	}

	std::set_terminate(&Bootstrap::onTerminate);

	// Add a stream for debug
	{
		Logger::Filter filter;
		filter.allTopics();
		Logger::Stream stream(m_loggerStream, filter);
		Logger::getDefault().addStream(stream);
	}

	IRSTD_LOG(IrStd::Topic::IrStdBootstrap, "Bootstrap initialized");
}

void IrStd::Bootstrap::onTerminate() noexcept
{
/*	IRSTD_SCOPE(IrStd::Flag::IrStdMemoryNoTrace);

	if (auto e = std::current_exception())
	{
		IrStd::Exception::print(std::cerr, e);
		IrStd::Exception::callStack(std::cerr, 2);
	}*/
}

const char* IrStd::Bootstrap::getSigDescription(int sig)
{
	const char* description;
	switch (sig)
	{
	case SIGABRT:
		description = " (SIGABRT: Process abort signal)";
		break;
	case SIGALRM:
		description = " (SIGALRM: Alarm clock)";
		break;
	case SIGBUS:
		description = " (SIGBUS: Access to an undefined portion of a memory object)";
		break;
	case SIGCHLD:
		description = " (SIGCHLD: Child process terminated)";
		break;
	case SIGCONT:
		description = " (SIGCONT: Continue executing, if stopped)";
		break;
	case SIGFPE:
		description = " (SIGFPE: Erroneous arithmetic operation)";
		break;
	case SIGHUP:
		description = " (SIGHUP: Hangup)";
		break;
	case SIGILL:
		description = " (SIGILL: Illegal instruction)";
		break;
	case SIGINT:
		description = " (SIGINT: Terminal interrupt signal)";
		break;
	case SIGKILL:
		description = " (SIGKILL: Kill)";
		break;
	case SIGPIPE:
		description = " (SIGPIPE: Write on a pipe with no one to read it)";
		break;
	case SIGQUIT:
		description = " (SIGQUIT: Terminal quit signal)";
		break;
	case SIGSEGV:
		description = " (SIGSEGV: Invalid memory reference)";
		break;
	case SIGSTOP:
		description = " (SIGSTOP: Stop executing)";
		break;
	case SIGTERM:
		description = " (SIGTERM: Termination signal)";
		break;
	case SIGTSTP:
		description = " (SIGTSTP: Termination stop signal)";
		break;
	case SIGTTIN:
		description = " (SIGTTIN: Background process attempting read)";
		break;
	case SIGTTOU:
		description = " (SIGTTOU: Background process attempting write)";
		break;
	case SIGUSR1:
		description = " (SIGUSR1: User-defined signal 1)";
		break;
	case SIGUSR2:
		description = " (SIGUSR2: User-defined signal 2)";
		break;
	case SIGPOLL:
		description = " (SIGPOLL: Pollable event)";
		break;
	case SIGPROF:
		description = " (SIGPROF: Profiling timer expired)";
		break;
	case SIGSYS:
		description = " (SIGSYS: Bad system call)";
		break;
	case SIGTRAP:
		description = " (SIGTRAP: Trace/breakpoint trap)";
		break;
	case SIGURG:
		description = " (SIGURG: High bandwidth data is available at a socket)";
		break;
	case SIGVTALRM:
		description = " (SIGVTALRM: Virtual timer expired)";
		break;
	case SIGXCPU:
		description = " (SIGXCPU: CPU time limit exceeded)";
		break;
	case SIGXFSZ:
		description = " (SIGXFSZ: File size limit exceeded)";
		break;
	default:
		description = " (Unknown signal description)";
	}
	return description;
}

static IrStd::Event sigHandlerThreadCompleted;

void IrStd::Bootstrap::sigHandlerThread(int sig, siginfo_t* info, void* secret)
{
	if (sig == SIGNAL_THREAD)
	{
		auto pThread = IrStd::Threads::getInstance().getNoLock(std::this_thread::get_id());
		IRSTD_ASSERT(pThread, "Only threads from IrStd::Thread should go through this path");

		std::cerr << std::endl << "Callstack (" << *pThread << "):" << std::endl;
		IrStd::Exception::callStack(std::cerr, /*skipFirstNb*/2);
		sigHandlerThreadCompleted.trigger();
	}
	else
	{
		sigHandler(sig, info, secret);
	}
}

void IrStd::Bootstrap::sigHandler(int sig, siginfo_t* info, void* /*secret*/)
{
	const char* const description = getSigDescription(sig);
	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);

	// Disable the memory manager to prevent any lock while accessing malloc
	IrStd::Memory::disable();
	IrStd::Logger::disable();

	// Flush all standard streams
	std::flush(std::cout);
	std::flush(std::cerr);

	// Print latest traces
	{
		std::cerr << std::endl << "Dump of last traces before crash:" << std::endl << "...";
		getInstance().m_loggerStreamBuf.toStream(std::cerr);
	}

	// Print the exceptions
	if (auto e = std::current_exception())
	{
		std::cerr << std::endl;
		IrStd::Exception::print(std::cerr, e);
	}

	// Print the callstack
	{
		std::cerr << std::endl << "FATAL: Received SIGNAL " << std::dec << sig << description
				<< ", faulty address is " << std::hex << std::showbase << info->si_addr << std::endl
				<< "Callstack (" << std::this_thread::get_id() << "):" << std::endl;
		IrStd::Exception::callStack(std::cerr, /*skipFirstNb*/2);
	}

	// Print the threads
	{
		std::cerr << std::endl;
		IrStd::Threads::toStream(std::cerr);

		// Set the thread signal handler to print threads' callstack
		{
			struct sigaction m_sa;
			m_sa.sa_sigaction = static_cast<void (*)(int, siginfo_t*, void*)>(Bootstrap::sigHandlerThread);
			sigemptyset(&m_sa.sa_mask);
			m_sa.sa_flags = SA_RESTART | SA_SIGINFO;
			sigaction(SIGNAL_THREAD, &m_sa, nullptr);
		}

		for (auto& item : IrStd::Threads::getInstance().m_threadMap)
		{
			auto pThread = item.second;
			if (pThread->isActive() || pThread->m_status == Thread::Status::TERMINATING)
			{
				// Kill the thread
				if (!pThread->signal(SIGNAL_THREAD))
				{
					std::cerr << "Error while killing thread (" << *pThread << ")" << std::endl;
				}
				// Wait for the thread to be killed, if the timeout happens, print an error
				if (!sigHandlerThreadCompleted.waitForNext(/*timeoutMs*/100))
				{
					std::cerr << "Timeout while waiting for thread (" << *pThread << ") to terminate" << std::endl;
				}
			}
		}
	}

	// Exit
	std::_Exit(EXIT_FAILURE);
}
