#pragma once
// This file must be included only once

#include <signal.h>

#include "Utils.hpp"

namespace IrStd
{
	class Bootstrap : public SingletonImpl<Bootstrap>
	{
	protected:
		friend SingletonImpl;

		Bootstrap();

		static void onTerminate() noexcept;
		static void sigHandler(int sig, siginfo_t *info, void *secret);

		struct sigaction m_sa;
	};
}
