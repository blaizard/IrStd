#pragma once

#include <signal.h>
#include <iostream>

#include "Utils.hpp"
#include "Compiler.hpp"
#include "Streambuf.hpp"

/**
 * \defgroup IrStd-Bootstrap
 * \brief Bootstrap code to initilaize signal handler and memory
 * monitoring.
 * \{
 * \ingroup IrStd
 */

namespace IrStd
{
	/**
	 * \brief
	 * \ingroup IrStd-Bootstrap
	 */
	class Bootstrap : public SingletonImpl<Bootstrap>
	{
	public:
		/**
		 * \brief Initiate the bootstraping code
		 *
		 * \ingroup IrStd-Bootstrap
		 *
		 * \note This function should be called only if the user knows
		 * exactly what he does. In general it is preferable to use
		 * it through IrStd::Main
		 */
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

/**
 * \}
 */
