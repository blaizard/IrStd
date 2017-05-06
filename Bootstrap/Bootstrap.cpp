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

IRSTD_TOPIC_REGISTER(IrStdBootstrap, "IrStdBoot");
IRSTD_SCOPE_USE(IrStdMemoryNoTrace);

IrStd::Bootstrap::Bootstrap()
{
	m_sa.sa_sigaction = static_cast<void (*)(int, siginfo_t*, void*)>(Bootstrap::sigHandler);
	sigemptyset(&m_sa.sa_mask);
	m_sa.sa_flags = SA_RESTART | SA_SIGINFO;

	// Catch all signals
	for (const auto& signal : {SIGBUS, SIGSEGV, SIGSYS, SIGUSR1, SIGFPE, SIGILL, SIGHUP, SIGABRT})
	{
		sigaction(signal, &m_sa, nullptr);
	}

	std::set_terminate(&Bootstrap::onTerminate);

	IRSTD_LOG(IrStd::Topic::IrStdBootstrap, "Bootstrap initialized");
}

void IrStd::Bootstrap::onTerminate() noexcept
{
	IRSTD_SCOPE_THREAD(scope, IrStdMemoryNoTrace);

	if (auto e = std::current_exception())
	{
		IrStd::Exception::print(std::cerr, e);
		IrStd::Exception::callStack(std::cerr, /*skipFirstNb*/2);
	}
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

void IrStd::Bootstrap::sigHandler(int sig, siginfo_t* info, void* /*secret*/)
{
	const char* const description = getSigDescription(sig);

	IRSTD_SCOPE_THREAD(scope, IrStdMemoryNoTrace);

	std::cerr << "FATAL: Received SIGNAL " << std::dec << sig << description
			<< ", faulty address is " << std::hex << std::showbase << info->si_addr << std::endl
			<< "Callstack (threadID=" << std::showbase << std::hex << std::this_thread::get_id() << "):" << std::endl;

	IrStd::Exception::callStack(std::cerr, /*skipFirstNb*/2);
	std::_Exit(EXIT_FAILURE);
}
