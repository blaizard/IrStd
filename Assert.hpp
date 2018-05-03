#pragma once

#include <iostream>
#include <cstdlib>

#include "Exception.hpp"
#include "Logger.hpp"
#include "Compiler.hpp"

/**
 * \defgroup IrStd-Assert
 * \brief Set of assertion tests that can be used to test invariants.
 * \{
 * \ingroup IrStd
 */

IRSTD_TOPIC_USE(IrStd, None);
IRSTD_SCOPE_USE(IrStdMemoryNoTrace);

/**
 * \name Assertions
 * \{
 */

/**
 * \brief Ensures value evaluates to true and log the failure (if any)
 *
 * If the value evaluates to false, a crash is immediatly triggered
 * by calling \ref IRSTD_CRASH .
 *
 * \param topic (optional) The topic to which this assertion applies.
 *                This must be a topic of type \ref IrStd::TopicImpl .
 * \param value The value to be tested
 * \param message (optional) A descriptive message that will be displayed in case the
 *                assertion fails.
 */
#define IRSTD_ASSERT(...) \
	IRSTD_GET_MACRO(_IRSTD_ASSERT, __VA_ARGS__)(__VA_ARGS__)

/**
 * \brief Ensures value evaluates to true and print to std::cerr the failure (if any)
 *
 * If the value evaluates to false, a crash is immediatly triggered
 * by calling \ref IRSTD_CRASH_NOLOG .
 *
 * \param topic (optional) The topic to which this assertion applies.
 *                This must be a topic of type \ref IrStd::TopicImpl .
 * \param value The value to be tested
 * \param message (optional) A descriptive message that will be displayed in case the
 *                assertion fails.
 */
#define IRSTD_ASSERT_NOLOG(...) \
	IRSTD_GET_MACRO(_IRSTD_ASSERT_NOLOG, __VA_ARGS__)(__VA_ARGS__)

/**
 * \brief Ensures value evaluates to true and throw an exception in case of failure
 *
 * \param topic (optional) The topic to which this assertion applies.
 *                This must be a topic of type \ref IrStd::TopicImpl .
 * \param value The value to be tested
 * \param message (optional) A descriptive message that will be displayed in case the
 *                assertion fails.
 */
#define IRSTD_THROW_ASSERT(...) \
	IRSTD_GET_MACRO(_IRSTD_THROW_ASSERT, __VA_ARGS__)(__VA_ARGS__)

/**
 * \brief Assert that a type is a child of \b typeBase
 *
 * This assertion is done at compile time, hence the error message will be displayed
 * on std::cerr.
 *
 * \param typeChild The child or descendant type.
 * \param typeBase The parent type.
 */
#define IRSTD_ASSERT_CHILDOF(typeChild, typeBase) \
	{ \
		(void) static_cast<typeBase*>((typeChild*) nullptr); \
	}

/**
 * \brief Assert that a variable is a child of \b typeBase
 *
 * This assertion is done at compile time, hence the error message will be displayed
 * on std::cerr.
 *
 * \param variable The child or descendant type.
 * \param typeBase The parent type.
 */
#define IRSTD_ASSERT_VAR_CHILDOF(variable, typeBase) \
		IRSTD_ASSERT_TYPE_CHILD_OF(decltype(variable), typeBase)

/**
 * \brief Assert that a type is of a specific type
 *
 * This assertion is done at compile time, hence the error
 * message will be displayed on std::cerr.
 *
 * \param type The type to be checked.
 * \param ... The different types. \b type must comply
 *            to at least one of them.
 */
#define IRSTD_ASSERT_TYPEOF(type, ...) \
	{ \
		static_assert(IRSTD_TYPEOF(type, __VA_ARGS__), "Type '" #type \
				"' is not of type (" #__VA_ARGS__ ")"); \
	}

/**
 * \brief Assert that a variable is of a specific type
 *
 * This assertion is done at compile time, hence the error
 * message will be displayed on std::cerr.
 *
 * \param variable The variable to be checked.
 * \param ... The different types. \b variable must comply
 *            to at least one of them.
 */
#define IRSTD_ASSERT_VAR_TYPEOF(variable, ...) \
	{ \
		static_assert(IRSTD_TYPEOF(decltype(variable), __VA_ARGS__), "Variable '" #variable \
				"' is not of type (" #__VA_ARGS__ ")"); \
	}

/**
 * \brief Ensures that a code path does not run into thread concurrency
 */
#define IRSTD_ASSERT_NO_CONCURRENCY() \
	IRSTD_SCOPE_GLOBAL(IRSTD_PASTE(irStdNoConcurrency, __LINE__)); \
	IRSTD_ASSERT(IRSTD_PASTE(irStdNoConcurrency, __LINE__).isActivator(), \
			"This code path cannot be executed concurrenlty");

/**
 * \}
 */

/**
 * \name Termination
 * \{
 */

/**
 * \brief Immediatly terminates the program and logs the crash.
 *
 * The crash will be logged using the default logger.
 * See \ref IrStd::Logger for more information.
 *
 * \param topic (optional) The topic to which this crash applies.
 *                This must be a topic of type \ref IrStd::TopicImpl .
 * \param message (optional) A descriptive message.
 */
#define IRSTD_CRASH(...) \
	IRSTD_GET_MACRO(_IRSTD_CRASH, __VA_ARGS__)(__VA_ARGS__)

/**
 * \brief Immediatly terminates the program and prints the crash over std::cerr.
 *
 * \param topic (optional) The topic to which this crash applies.
 *                This must be a topic of type \ref IrStd::TopicImpl .
 * \param message (optional) A descriptive message.
 */
#define IRSTD_CRASH_NOLOG(...) \
	IRSTD_GET_MACRO(_IRSTD_CRASH_NOLOG, __VA_ARGS__)(__VA_ARGS__)

/**
 * \brief Similar to \ref IRSTD_CRASH, this will immediatly terminates the program
 *        and logs the termination.
 *
 * This should be used when a line of code should never be reached.
 *
 * \param topic (optional) The topic to which this applies.
 *                This must be a topic of type \ref IrStd::TopicImpl .
 */
#define IRSTD_UNREACHABLE(...) \
	IRSTD_GET_MACRO(_IRSTD_UNREACHABLE, __VA_ARGS__)(__VA_ARGS__)

/**
 * \}
 */

/**
 * \internal
 * \{
 */

#define _IRSTD_ASSERT1(condition) _IRSTD_ASSERT3(IRSTD_TOPIC(IrStd, None), condition, "")
#define _IRSTD_ASSERT2(condition, expr) _IRSTD_ASSERT3(IRSTD_TOPIC(IrStd, None), condition, expr)
#define _IRSTD_ASSERT3(topic, condition, expr) \
	{ \
		if (!(condition)) \
		{ \
			_IRSTD_CRASH2(topic, "Assertion failed: '" #condition "'. " << expr); \
		} \
	}

#define _IRSTD_ASSERT_NOLOG1(condition) _IRSTD_ASSERT_NOLOG3(IRSTD_TOPIC(IrStd, None), condition, "")
#define _IRSTD_ASSERT_NOLOG2(condition, expr) _IRSTD_ASSERT_NOLOG3(IRSTD_TOPIC(IrStd, None), condition, expr)
#define _IRSTD_ASSERT_NOLOG3(topic, condition, expr) \
	{ \
		if (!(condition)) \
		{ \
			_IRSTD_CRASH_NOLOG2(topic, "Assertion failed: '" #condition "'. " << expr); \
		} \
	}

#define _IRSTD_THROW_ASSERT1(condition) _IRSTD_THROW_ASSERT3(IRSTD_TOPIC(IrStd, None), condition, "")
#define _IRSTD_THROW_ASSERT2(condition, expr) _IRSTD_THROW_ASSERT3(IRSTD_TOPIC(IrStd, None), condition, expr)
#define _IRSTD_THROW_ASSERT3(topic, condition, expr) \
	{ \
		if (!(condition)) \
		{ \
			IRSTD_THROW(topic, "Assertion failed: '" #condition "'. " << expr); \
		} \
	}

#define _IRSTD_UNREACHABLE0() _IRSTD_UNREACHABLE1(IRSTD_TOPIC(IrStd, None)) 
#define _IRSTD_UNREACHABLE1(topic) IRSTD_CRASH(topic, "Code location unreachable")
#define _IRSTD_UNREACHABLE2(topic, expr) IRSTD_CRASH(topic, expr)

#define _IRSTD_CRASH0() _IRSTD_CRASH2(IRSTD_TOPIC(IrStd, None), "Crash!")
#define _IRSTD_CRASH1(expr) _IRSTD_CRASH2(IRSTD_TOPIC(IrStd, None), expr)
#define _IRSTD_CRASH2(topic, expr) \
	{ \
		if (IrStd::Flag::IrStdMemoryNoTrace->isSet()) \
		{ \
			std::cerr << "(" << IRSTD_FILENAME << ":" << __LINE__ << ") " << expr << std::endl; \
		} \
		else \
		{ \
			IRSTD_LOG_FATAL(topic, expr); \
		} \
		std::abort(); \
	}

#define _IRSTD_CRASH_NOLOG0() _IRSTD_CRASH_NOLOG2(IRSTD_TOPIC(IrStd, None), "Crash!")
#define _IRSTD_CRASH_NOLOG1(expr) _IRSTD_CRASH_NOLOG2(IRSTD_TOPIC(IrStd, None), expr)
#define _IRSTD_CRASH_NOLOG2(topic, expr) \
	{ \
		std::cerr << "(" << IRSTD_FILENAME << ":" << __LINE__ << ") " << expr << std::endl; \
		std::abort(); \
	}

/**
 * \}
 */
