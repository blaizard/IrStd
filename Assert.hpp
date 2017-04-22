#pragma once

#include <iostream>
#include <cstdlib>

#include "Exception.hpp"
#include "Logger.hpp"

IRSTD_TOPIC_USE(None);

#define IRSTD_ASSERT(...) \
	IRSTD_GET_MACRO(_IRSTD_ASSERT, __VA_ARGS__)(__VA_ARGS__)

#define _IRSTD_ASSERT1(condition) _IRSTD_ASSERT3(IrStd::Topic::None, condition, "")
#define _IRSTD_ASSERT2(condition, expr) _IRSTD_ASSERT3(IrStd::Topic::None, condition, expr)
#define _IRSTD_ASSERT3(topic, condition, expr) \
	{ \
		if (!(condition)) \
		{ \
			_IRSTD_CRASH2(topic, "Assertion failed: '" #condition "'. " << expr); \
		} \
	}

#define IRSTD_ASSERT_NOLOG(...) \
	IRSTD_GET_MACRO(_IRSTD_ASSERT_NOLOG, __VA_ARGS__)(__VA_ARGS__)

#define _IRSTD_ASSERT_NOLOG1(condition) _IRSTD_ASSERT_NOLOG3(IrStd::Topic::None, condition, "")
#define _IRSTD_ASSERT_NOLOG2(condition, expr) _IRSTD_ASSERT_NOLOG3(IrStd::Topic::None, condition, expr)
#define _IRSTD_ASSERT_NOLOG3(topic, condition, expr) \
	{ \
		if (!(condition)) \
		{ \
			_IRSTD_CRASH_NOLOG2(topic, "Assertion failed: '" #condition "'. " << expr); \
		} \
	}

#define IRSTD_CRASH(...) \
	IRSTD_GET_MACRO(_IRSTD_CRASH, __VA_ARGS__)(__VA_ARGS__)

#define _IRSTD_CRASH0() _IRSTD_CRASH2(IrStd::Topic::None, "Crash!")
#define _IRSTD_CRASH1(expr) _IRSTD_CRASH2(IrStd::Topic::None, expr)
#define _IRSTD_CRASH2(topic, expr) \
	{ \
		IRSTD_LOG_FATAL(topic, expr); \
		std::abort(); \
	}

#define IRSTD_CRASH_NOLOG(...) \
	IRSTD_GET_MACRO(_IRSTD_CRASH_NOLOG, __VA_ARGS__)(__VA_ARGS__)

#define _IRSTD_CRASH_NOLOG0() _IRSTD_CRASH_NOLOG2(IrStd::Topic::None, "Crash!")
#define _IRSTD_CRASH_NOLOG1(expr) _IRSTD_CRASH_NOLOG2(IrStd::Topic::None, expr)
#define _IRSTD_CRASH_NOLOG2(topic, expr) \
	{ \
		std::cerr << expr << std::endl; \
		std::abort(); \
	}
