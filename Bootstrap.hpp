#pragma once
// This file must be included only once

#include <signal.h>

#include "Utils.hpp"
#include "Compiler.hpp"

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
		static const char* getSigDescription(int sig);

		struct sigaction m_sa;
	};
}
