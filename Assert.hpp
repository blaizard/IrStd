#pragma once

#include <iostream>
#include <cstdlib>

#include "Exception.hpp"
#include "Logger.hpp"

IRSTD_TOPIC_USE(None);
IRSTD_SCOPE_USE(IrStdMemoryNoTrace);

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

#define IRSTD_THROW_ASSERT(...) \
	IRSTD_GET_MACRO(_IRSTD_THROW_ASSERT, __VA_ARGS__)(__VA_ARGS__)

#define _IRSTD_THROW_ASSERT1(condition) _IRSTD_THROW_ASSERT3(IrStd::Topic::None, condition, "")
#define _IRSTD_THROW_ASSERT2(condition, expr) _IRSTD_THROW_ASSERT3(IrStd::Topic::None, condition, expr)
#define _IRSTD_THROW_ASSERT3(topic, condition, expr) \
	{ \
		if (!(condition)) \
		{ \
			IRSTD_THROW(topic, "Assertion failed: '" #condition "'. " << expr); \
		} \
	}

#define IRSTD_CRASH(...) \
	IRSTD_GET_MACRO(_IRSTD_CRASH, __VA_ARGS__)(__VA_ARGS__)

#define _IRSTD_CRASH0() _IRSTD_CRASH2(IrStd::Topic::None, "Crash!")
#define _IRSTD_CRASH1(expr) _IRSTD_CRASH2(IrStd::Topic::None, expr)
#define _IRSTD_CRASH2(topic, expr) \
	{ \
		if (IrStd::Flag::IrStdMemoryNoTrace->isSet()) \
		{ \
			std::cerr << expr << std::endl; \
		} \
		else \
		{ \
			IRSTD_LOG_FATAL(topic, expr); \
		} \
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

#define IRSTD_UNREACHABLE(...) \
	IRSTD_GET_MACRO(_IRSTD_UNREACHABLE, __VA_ARGS__)(__VA_ARGS__)

#define _IRSTD_UNREACHABLE0() _IRSTD_UNREACHABLE1(IrStd::Topic::None) 
#define _IRSTD_UNREACHABLE1(topic) IRSTD_CRASH(topic, "Code location unreachable")

/**
 * \brief Assert that \b base is the base class of the class \b descendant
 */
#define IRSTD_ASSERT_TYPE_DESCENDANT(base, descendant) \
	{ \
		static_assert(!std::is_same<base, descendant>::value, "The class " #descendant " must be a descendant of type " #base); \
		(void) static_cast<base*>((descendant*) nullptr); \
	}


#define IRSTD_ASSERT_TYPEOF(variable, ...) \
	static_assert(IRSTD_TYPEOF(variable, __VA_ARGS__), "Variable '" #variable "' is not of type (" #__VA_ARGS__ ")");
