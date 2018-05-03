#pragma once

#include <stdint.h>
#include <string>

#include "Utils.hpp"

// ---- COMPILER --------------------------------------------------------------

/**
 * \defgroup IrStd-Compiler
 * \brief Set of macro to test the compiler, compilation options and platform
 * \{
 * \ingroup IrStd
 */

#define _IRSTD_IS_COMPILER_GCC0() _IRSTD_IS_COMPILER_GCC3(0, 0, 0)
#define _IRSTD_IS_COMPILER_GCC1(x) _IRSTD_IS_COMPILER_GCC3(x, 0, 0)
#define _IRSTD_IS_COMPILER_GCC2(x, y) _IRSTD_IS_COMPILER_GCC3(x, y, 0)
#define _IRSTD_IS_COMPILER_GCC3(x, y, z) (defined(__GNUC__) && (__GNUC__ > x \
		|| (__GNUC__ == x && (__GNUC_MINOR__ > y \
		|| ( __GNUC_MINOR__ == y && (__GNUC_PATCHLEVEL__ >= z))))))

/**
 * \name Compiler
 * \{
 */

/**
 * \brief Check if the name and version of the compiler used
 *
 * \param name The name of the compiler, "GCC" for example.
 * \param ... (optional) Extra parameters to be passed to the
 *            IRSTD_IS_COMPILER_##name macro
 */
#define IRSTD_IS_COMPILER(name, ...) IRSTD_PASTE(IRSTD_IS_COMPILER_, name)(__VA_ARGS__)

/**
 * \brief Check if the compiler used is GCC and, optionaly,
 * if its version is greater or equal to the one supplied
 * into arguments 
 *
 * \param major (optional) Major version
 * \param minor (optional) Minor version
 * \param patch (optional) Patch level
 */
#define IRSTD_IS_COMPILER_GCC(...) IRSTD_GET_MACRO(_IRSTD_IS_COMPILER_GCC, __VA_ARGS__)(__VA_ARGS__)

/**
 * \brief Check if the compiler (preprocessor in this case) used is DOXYGEN
 */
#define IRSTD_IS_COMPILER_DOXYGEN(...) defined(DOXYGEN)

// For documentation purpose
#if IRSTD_IS_COMPILER_DOXYGEN()
 	/**
	 * \brief The name of the compiler used, example "GCC"
	 */
	#define IRSTD_COMPILER_NAME "DOXYGEN"
	/**
	 * \brief The version of the compiler, example "5.3.1"
	 */
	#define IRSTD_COMPILER_VERSION ""
#elif IRSTD_IS_COMPILER(GCC)
	#define IRSTD_COMPILER_NAME "GCC"
	#define IRSTD_COMPILER_VERSION IRSTD_QUOTE(__GNUC__) "." IRSTD_QUOTE(__GNUC_MINOR__) "." IRSTD_QUOTE(__GNUC_PATCHLEVEL__)
#else
	#define IRSTD_COMPILER_NAME "UKNOWN"
	#define IRSTD_COMPILER_VERSION ""
#endif

/**
 * \brief Descriptive string representing the compiler used.
 * It includes the compiler name and its version.
 */
#define IRSTD_COMPILER_STRING IRSTD_COMPILER_NAME " v" IRSTD_COMPILER_VERSION

/// \}

// ---- PLATFORM --------------------------------------------------------------

/**
 * \name Platform
 * \{
 */

/**
 * \brief Check if the compilation is performed on a specific platform
 *
 * \param name The name of the platform, LINUX for example.
 */
#define IRSTD_IS_PLATFORM(name, ...) IRSTD_PASTE(IRSTD_IS_PLATFORM_, name)(__VA_ARGS__)

/**
 * \brief Check if the compilation is performed on a Linux platform
 */
#define IRSTD_IS_PLATFORM_LINUX() (defined(__linux__))

/**
 * \brief Check if the compilation is performed on a Windows platform
 */
#define IRSTD_IS_PLATFORM_WINDOWS() (defined(_WIN32))

/**
 * \brief Check if the compilation is performed on an Apple platform
 */
#define IRSTD_IS_PLATFORM_APPLE() (defined(__APPLE__))

// For documentation purpose
#if IRSTD_IS_COMPILER_DOXYGEN()
	/**
	 * \brief The name of the platform used for the compilation
	 */
	#define IRSTD_PLATFORM_NAME "DOXYGEN"
#elif IRSTD_IS_PLATFORM(LINUX)
	#define IRSTD_PLATFORM_NAME "LINUX"
#elif IRSTD_IS_PLATFORM(WINDOWS)
	#define IRSTD_PLATFORM_NAME "WINDOWS"
#elif IRSTD_IS_PLATFORM(APPLE)
	#define IRSTD_PLATFORM_NAME "APPLE"
#else
	#define IRSTD_PLATFORM_NAME "UNKNOWN"
	#define IRSTD_PLATFORM_UNKNOWN
#endif


// For documentation purpose
#if IRSTD_IS_COMPILER_DOXYGEN()
	/**
	 * \brief The architecture bit width of the processor used for the compilation
	 */
	#define IRSTD_PLATFORM_BIT 32
#elif defined(UINTPTR_MAX) && UINTPTR_MAX == 0xffffffffffffffff
	#define IRSTD_PLATFORM_BIT 64
#elif defined(UINTPTR_MAX) && UINTPTR_MAX == 0xffffffff
	#define IRSTD_PLATFORM_BIT 32
#else
	#define IRSTD_PLATFORM_BIT 0
#endif

/**
 * \brief Descriptive string representing the platform used.
 * It includes the platform name and the bit width.
 */
#define IRSTD_PLATFORM_STRING IRSTD_PLATFORM_NAME " " IRSTD_QUOTE(IRSTD_PLATFORM_BIT) "-bit"

/// \}

// ---- BUILD TYPE ------------------------------------------------------------

/**
 * \name Build Type
 * \{
 */

/**
 * \brief Defined when the build is for debug
 */
#define IRSTD_IS_DEBUG (defined(IRSTD_DEBUG))

/**
 * \brief Defined when the build is for release
 */
#define IRSTD_IS_RELEASE (defined(IRSTD_RELEASE))

/// \}

// ---- MESSAGES --------------------------------------------------------------

/**
 * \name Messaging
 * \{
 */

// For documentation purpose
#if IRSTD_IS_COMPILER_DOXYGEN()
	/**
	 * \brief Display an informal message at compilation time and
	 * continue compilation
	 */
	#define IRSTD_STATIC_INFO(expr)
	/**
	 * \brief Display an error at compilation time and breaks compilation
	 */
	#define IRSTD_STATIC_ERROR(expr)
#elif IRSTD_IS_COMPILER(GCC)
	#define _IRSTD_PRAGMA(x) _Pragma (#x)
	#define IRSTD_STATIC_INFO(expr) _IRSTD_PRAGMA(message (#expr))
	#define IRSTD_STATIC_ERROR(expr) _IRSTD_PRAGMA(GCC error #expr)
#else
	#define IRSTD_STATIC_INFO(expr)
	#define IRSTD_STATIC_ERROR(expr) static_assert(0, expr);
#endif

/**
 * \brief Get the current file's name
 */
#if IRSTD_IS_PLATFORM(WINDOWS)
	#define IRSTD_FILENAME (std::strrchr("\\" __FILE__, '\\') + 1)
#else
	#define IRSTD_FILENAME (std::strrchr("/" __FILE__, '/') + 1)
#endif

/**
 * \}
 */

/// \}

namespace IrStd
{
	namespace Compiler
	{
		const std::string& getBuildVersion() noexcept;
	}
}
