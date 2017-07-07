#pragma once

#include <signal.h>
#include <iostream>

#include "Utils.hpp"
#include "Compiler.hpp"
#include "Streambuf.hpp"

namespace IrStd
{
	class Bootstrap : public SingletonImpl<Bootstrap>
	{
	public:
		static void init()
		{
			IrStd::Bootstrap::getInstance();
		}

	protected:
		friend SingletonImpl;

		Bootstrap();

		static void onTerminate() noexcept;
		static void sigHandler(int sig, siginfo_t *info, void *secret);
		static void sigHandlerThread(int sig, siginfo_t *info, void *secret);
		static const char* getSigDescription(int sig);

		struct sigaction m_sa;
		Streambuf::Circular m_loggerStreamBuf;
		std::ostream m_loggerStream;
	};
}
