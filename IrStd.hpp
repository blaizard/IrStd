#pragma once

/**
 * \defgroup IrStd
 * \brief Generic C++ framework
 *
 * C++ framework taking advantage of C++11 features and adding useful functionanlities for 
 * application developpment such as a flexible logger, a built-in REST API server, memory allocation
 * monitoring, stack traces and much more.
 */

#include "Compiler.hpp"
#include "Assert.hpp"

// Some useful checks
#if __cplusplus < 201103L
	IRSTD_STATIC_ERROR("C++11 is required to use this library")
#endif
#ifdef IRSTD_PLATFORM_UNKNOWN
	IRSTD_STATIC_INFO("Unknown platform type")
#endif
#if IRSTD_PLATFORM_BIT == 0
	IRSTD_STATIC_INFO("Unknown platform bit width")
#endif
#if IRSTD_IS_DEBUG && IRSTD_IS_RELEASE
	IRSTD_STATIC_ERROR("Debug and Release build type are set")
#endif

#include "Allocator.hpp"
#include "Bootstrap.hpp"
#include "Crypto.hpp"
#include "Event.hpp"
#include "Exception.hpp"
#include "Fetch.hpp"
#include "FileSystem.hpp"
#include "Flag.hpp"
#include "Json.hpp"
#include "Logger.hpp"
#include "Main.hpp"
#include "Memory.hpp"
#include "Rand.hpp"
#include "RWLock.hpp"
#include "Server.hpp"
#include "Server/ServerHTTP.hpp"
#include "Server/ServerREST.hpp"
#include "Scope.hpp"
#include "Streambuf.hpp"
#include "Thread.hpp"
#include "Topic.hpp"
#include "Type.hpp"
#include "Utils.hpp"
#include "Websocket.hpp"

/**
 * \}
 */
