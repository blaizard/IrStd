#include <ostream>
#include <cxxabi.h>
#include <execinfo.h>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>
#include <iomanip>

#include "../Compiler.hpp"
#include "../Exception.hpp"
#include "../Assert.hpp"

IrStd::Exception::Exception(
		const size_t line,
		const char* const file,
		const char* const func,
		const IrStd::TopicImpl& topic)
		: m_line(line)
		, m_file(file)
		, m_func(func)
		, m_topic(topic)
		, m_stream(m_message)
		, m_allowRetry(false)
{
}

IrStd::Exception::Exception(
		const size_t line,
		const char* const file,
		const char* const func,
		const bool retry,
		const IrStd::TopicImpl& topic)
		: Exception(line, file, func, topic)
{
	allowRetry(retry);
}

IrStd::Exception::Exception(const Exception&& e)
		: m_line(e.m_line)
		, m_file(e.m_file)
		, m_func(e.m_func)
		, m_topic(e.m_topic)
		, m_message(e.m_message)
		, m_stream(m_message)
		, m_allowRetry(e.m_allowRetry)
{
}

void IrStd::Exception::print(
		std::ostream& out,
		std::exception_ptr ptr) noexcept
{
	if (!ptr)
	{
		ptr = std::current_exception();
		if (!ptr)
		{
			return;
		}
	}

	const std::function<void (const IrStd::ExceptionPtr, const size_t)> printRec
			= [&](const IrStd::ExceptionPtr pE, const size_t l)
	{
		out << std::string(l, ' ') << "EXCEPTION #" << l << ": ";

		IRSTD_ASSERT(pE, "ExceptionPtr passed into argument is null");

		if (pE.isIrStdException())
		{
			const auto& e = pE.getIrStdException();
			out << "IrStd::Exception: " << e.m_file << ":" << e.m_line << " (" << e.m_func << ")";
			if (std::strlen(e.m_topic.getStr()))
			{
				out << " [" << e.m_topic.getStr() << "]";
			}
			out << " " << e.what() << std::endl;
		}
		else
		{
			IRSTD_ASSERT(pE.isException(), "The exception must be of type std::exception");
			const auto& e = pE.getException();
			out << "std::exception: " << e.what() << std::endl;
		}

		// Handle chained exceptions
		if (auto pNexE = pE.getNext())
		{
			printRec(pNexE, l + 1);
		}
	};

	printRec(IrStd::ExceptionPtr::fromExceptionPtr(ptr), 0);
}

void IrStd::Exception::allowRetry(const bool retry) noexcept
{
	m_allowRetry = retry;
}

bool IrStd::Exception::isAllowRetry() const noexcept
{
	return m_allowRetry;
}

IrStd::ExceptionPtr IrStd::Exception::getNext() const
{
	return getNext(this);
}

IrStd::ExceptionPtr IrStd::Exception::getNext(const std::exception* pE)
{
	// Check if this is a nested_exception and it has a next pointer
	auto pNested = dynamic_cast<const std::nested_exception*>(std::addressof(*pE));
	if (!pNested || !pNested->nested_ptr())
	{
		return ExceptionPtr();
	}

	// Returns the exception
	try
	{
		std::rethrow_if_nested(std::move(*pE));
	}
	catch (IrStd::Exception& e)
	{
		return ExceptionPtr(&e);
	}
	catch (std::exception& e)
	{
		return ExceptionPtr(&e);
	}
	catch (...)
	{
		IRSTD_THROW("Exception type unhandled");
	}

	return ExceptionPtr();
}

void IrStd::Exception::rethrow()
{
	auto pE = std::current_exception();
	IRSTD_ASSERT(pE, "There are no pending exceptions");

	try
	{
		std::rethrow_exception(pE);
	}
	catch (IrStd::Exception& e)
	{
		throw;
	}
	catch(...)
	{
		IRSTD_THROW("Exception wrapper");
	}

	throw;
}


void IrStd::Exception::rethrowRetry()
{
	auto pE = std::current_exception();
	IRSTD_ASSERT(pE, "There are no pending exceptions");

	try
	{
		std::rethrow_exception(pE);
	}
	catch (IrStd::Exception& e)
	{
		e.allowRetry();
		throw;
	}
	catch(...)
	{
		IRSTD_THROW_RETRY("Retry wrapper");
	}
}

std::string IrStd::Exception::trace() const
{
	std::stringstream streamStr;
	const std::function<void (const IrStd::ExceptionPtr)> printRec
			= [&](const IrStd::ExceptionPtr pE)
	{
		IRSTD_ASSERT(pE, "ExceptionPtr passed into argument is null");

		if (pE.isIrStdException())
		{
			const auto& e = pE.getIrStdException();
			streamStr << e.m_file << ":" << e.m_line << " (" << e.m_func << ")";
		}
		else
		{
			streamStr << "unknown";
		}

		// Handle chained exceptions
		if (auto pNexE = pE.getNext())
		{
			streamStr << " -> ";
			printRec(pNexE);
		}
	};

	printRec(IrStd::ExceptionPtr(this));
	return streamStr.str();
}

const char* IrStd::Exception::what() const noexcept
{
	return m_message.c_str();
}

const IrStd::TopicImpl& IrStd::Exception::getTopic() const noexcept
{
	return m_topic;
}

void IrStd::Exception::demangle(std::ostream& out, const char* const symbol)
{
	int status;
	const std::unique_ptr<char, decltype(&std::free)> demangled(abi::__cxa_demangle(symbol, 0, 0, &status), &std::free);
	out << ((demangled && status == 0) ? demangled.get() : symbol);
}

void IrStd::Exception::callStack(std::ostream& out, const size_t skipFirstNb) noexcept
{
	const size_t MAX_STACK_LEVEL = 64;
	void* addresses[MAX_STACK_LEVEL];

	const int nbLevels = ::backtrace(addresses, MAX_STACK_LEVEL);
	const std::unique_ptr<char*, decltype(&std::free)> symbols(::backtrace_symbols(addresses, nbLevels), &std::free);

	for(int level = static_cast<int>(skipFirstNb); level < nbLevels; ++level)
	{
		char* const symbol = symbols.get()[level];
		char* end = symbol;
		// example: symbol = end = ./test(operator new(int)+0x10) [0x45]
		while (*end)
		{
			++end;
		}
		// example: symbol = "./test(operator new(int)+0x10) [0x45]" = end
		while (end != symbol && *end != '+')
		{
			--end;
		}
		// example: symbol = "./test(operator new(int)" = end
		char* begin = end;
		while (begin != symbol && *begin != '(')
		{
			--begin;
		}

		out << "#" << std::dec << std::left << std::setfill(' ')
				<< std::setw(3) << (level - skipFirstNb);
		if (begin != symbol)
		{
			out.write(symbol, ++begin - symbol);
			*end++ = '\0';
			demangle(out, begin);
			out << '+' << end;
		}
		else
		{
			out << symbol;
		}
		out << std::endl;
	}
}
