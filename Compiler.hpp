#pragma once

#include "Utils.hpp"

#define IRSTD_IS_COMPILER(name, ...) IRSTD_IS_COMPILER_##name(__VA_ARGS__)

#define IRSTD_IS_COMPILER_GCC(...) IRSTD_GET_MACRO(_IRSTD_IS_COMPILER_GCC, __VA_ARGS__)(__VA_ARGS__)

#define _IRSTD_IS_COMPILER_GCC0() _IRSTD_IS_COMPILER_GCC3(0, 0, 0)
#define _IRSTD_IS_COMPILER_GCC1(x) _IRSTD_IS_COMPILER_GCC3(x, 0, 0)
#define _IRSTD_IS_COMPILER_GCC2(x, y) _IRSTD_IS_COMPILER_GCC3(x, y, 0)
#define _IRSTD_IS_COMPILER_GCC3(x, y, z) (defined(__GNUC__) && (__GNUC__ > x \
		|| (__GNUC__ == x && (__GNUC_MINOR__ > y \
		|| ( __GNUC_MINOR__ == y && (__GNUC_PATCHLEVEL__ >= z))))))

#if IRSTD_IS_COMPILER(GCC)
	#define IRSTD_COMPILER_NAME "GCC"
	#define IRSTD_COMPILER_VERSION IRSTD_QUOTE(__GNUC__) "." IRSTD_QUOTE(__GNUC_MINOR__) "." IRSTD_QUOTE(__GNUC_PATCHLEVEL__)
#else
	#error This compiler is not supported
#endif

#define IRSTD_COMPILER_STRING IRSTD_COMPILER_NAME " v" IRSTD_COMPILER_VERSION
