#pragma once

#include <iostream>
#include <cstdlib>

#include "Exception.hpp"
#include "Logger.hpp"

#define IRSTD_ASSERT(...) \
	IRSTD_GET_MACRO(IRSTD_ASSERT, __VA_ARGS__)(__VA_ARGS__)

#define IRSTD_ASSERT1(condition) IRSTD_ASSERT3(None, condition, "")
#define IRSTD_ASSERT2(condition, expr) IRSTD_ASSERT3(None, condition, "")
#define IRSTD_ASSERT3(topic, condition, expr) \
	{ \
		if (!(condition)) \
		{ \
			IRSTD_CRASH2(topic, "Assertion failed: '" #condition "'. " << expr); \
		} \
	}

#define IRSTD_CRASH(...) \
	IRSTD_GET_MACRO(IRSTD_CRASH, __VA_ARGS__)(__VA_ARGS__)

#define IRSTD_CRASH1(expr) IRSTD_CRASH2(None, expr)
#define IRSTD_CRASH2(topic, expr) \
	{ \
		IRSTD_LOG_FATAL(topic, expr); \
		IrStd::Exception::callStack(std::cerr); \
		std::abort(); \
	}
