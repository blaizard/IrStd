#pragma once
// This file must be included only once

#include <signal.h>
#include <execinfo.h>
#include <iostream>
#include <mutex>
#include <csignal>

#include <cxxabi.h>
#include <execinfo.h>

#include "Exception.hpp"

static std::mutex mtxTemp;

#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <ucontext.h>

#ifdef __x86_64__
#define REG_EIP REG_RIP
#endif

class Bootstrap
{
public:
	Bootstrap()
	{
		m_sa.sa_sigaction = static_cast<void (*)(int, siginfo_t*, void*)>(Bootstrap::sigHandler);
		sigemptyset(&m_sa.sa_mask);
		m_sa.sa_flags = SA_RESTART | SA_SIGINFO;

		// Catch all signals
		for (const auto& signal : {SIGBUS, SIGSEGV, SIGSYS, SIGUSR1, SIGFPE, SIGILL, SIGHUP, SIGABRT})
		{
			sigaction(SIGSEGV, &m_sa, NULL);
		}

		std::set_terminate(&Bootstrap::onTerminate);
	}

	static void onTerminate() noexcept
	{
		if(auto exc = std::current_exception())
		{
			std::cerr << "Unhandled exception" << std::endl;
			IrStd::Exception::callStack(std::cerr);
		}
	//	std::_Exit(EXIT_FAILURE);
	}

	static void sigHandler(int sig, siginfo_t *info, void *secret)
	{
		std::cerr << "Got signal " << std::dec << sig << ", faulty address is "
				<< std::showbase << std::hex << info->si_addr
				<< std::endl;

		IrStd::Exception::callStack(std::cerr, /*skipFirstNb*/3);

		std::_Exit(EXIT_FAILURE);
	}

private:
	struct sigaction m_sa;
	static std::mutex mtx;
};

Bootstrap bootstrap;
