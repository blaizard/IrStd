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
#include "../Topic.hpp"
#include "../Main.hpp"

IRSTD_TOPIC_REGISTER(IrStd, Exception);
IRSTD_TOPIC_USE_ALIAS(IrStdException, IrStd, Exception);

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

IrStd::Exception::Exception(const Exception& e)
		: m_line(e.m_line)
		, m_file(e.m_file)
		, m_func(e.m_func)
		, m_topic(e.m_topic)
		, m_message(e.m_message)
		, m_stream(m_message)
		, m_allowRetry(e.m_allowRetry)
{
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
		IRSTD_THROW(IrStdException, "Exception type unhandled");
	}

	return ExceptionPtr();
}

void IrStd::Exception::rethrow()
{
	auto pE = std::current_exception();
	IRSTD_ASSERT(IrStdException, pE, "There are no pending exceptions");

	try
	{
		std::rethrow_exception(pE);
	}
	catch (IrStd::Exception& e)
	{
		throw;
	}
	catch (...)
	{
		IRSTD_THROW("Exception wrapper");
	}
}

void IrStd::Exception::rethrowRetry()
{
	auto pE = std::current_exception();
	IRSTD_ASSERT(IrStdException, pE, "There are no pending exceptions");

	try
	{
		std::rethrow_exception(pE);
	}
	catch (IrStd::Exception& e)
	{
		e.allowRetry();
		throw;
	}
	catch (...)
	{
		IRSTD_THROW_RETRY(IrStdException, "Exception wrapper (retry)");
	}
}

const char* IrStd::Exception::what() const noexcept
{
	return m_message.c_str();
}

const IrStd::TopicImpl& IrStd::Exception::getTopic() const noexcept
{
	return m_topic;
}

bool IrStd::Exception::demangle(char* pBuffer, const size_t size, const char* const pSymbol) noexcept
{
	int status;
	try
	{
		const std::unique_ptr<char, decltype(&std::free)> demangled(abi::__cxa_demangle(pSymbol, 0, 0, &status), &std::free);
		std::strncpy(pBuffer, ((demangled && status == 0) ? demangled.get() : pSymbol), size);
	}
	catch (...)
	{
		// Ignore and return false
		return false;
	}
	return (size > 0 && pBuffer[0] != '\0');
}

bool IrStd::Exception::addressToFileInfo(char* pBuffer, const size_t size, const void* pAddress, const char* pExecutablePath) noexcept
{
	int link[2];
	if (::pipe(link) == -1)
	{
		return false;
	}

	const ::pid_t pid = ::fork();
	if (pid < 0)
	{
		return false;
	}
	else if (pid == 0)
	{
		char addressToString[64];
		const int retVal = snprintf(addressToString, sizeof(addressToString), "%p", pAddress);
		if (retVal > 0)
		{
			::dup2(link[1], STDOUT_FILENO);
			::close(link[0]);
			::close(link[1]);
			::execlp("addr2line", "addr2line", addressToString, "-pfCse",
					(pExecutablePath) ? pExecutablePath : IrStd::Main::getExecutablePath(),
					nullptr);
		}
		std::_Exit(-1);
	}

	// Wait for forked process to complete
	{
		int status;
		const int retPid = ::waitpid(pid, &status, 0);
		if (retPid != pid || WEXITSTATUS(status) != 0)
		{
			return false;
		}
	}

	::close(link[1]);
	const auto nbBytes = ::read(link[0], pBuffer, size - 1);
	if (nbBytes < 4)
	{
		return false;
	}

	pBuffer[nbBytes - 1] = '\0';
	if (std::strstr(pBuffer, "??:0") == nullptr)
	{
		// Strip everything after the  last ':<number>'
		// Sometimes it append extra information after that is not needed
		auto pPos = std::strrchr(pBuffer, ':');
		if (pPos)
		{
			do
			{
				pPos++;
			} while (*pPos >= '0' && *pPos <= '9');
			*pPos = '\0';
			return true;
		}
	}
	return false;
}

void IrStd::Exception::callStack(std::ostream& out, const size_t skipFirstNb) noexcept
{
	constexpr size_t MAX_STACK_LEVEL = 64;
	void* addresses[MAX_STACK_LEVEL];

	const int nbLevels = ::backtrace(addresses, MAX_STACK_LEVEL);
	const std::unique_ptr<char*, decltype(&std::free)> symbols(::backtrace_symbols(addresses, nbLevels), &std::free);

	for(int level = static_cast<int>(skipFirstNb); level < nbLevels; ++level)
	{
		char* pSymbol = symbols.get()[level];
		const char* pSourcePath = nullptr;
		const char* pFunction = nullptr;
		const char* pOffset = nullptr;

		// Look for the source path
		{
			const auto pEnd = std::strchr(pSymbol, '(');
			if (pEnd)
			{
				*pEnd = '\0';
				pSourcePath = pSymbol;
				pSymbol = pEnd + 1;
			}
		}

		// Look for the function
		if (pSourcePath)
		{
			const auto pEnd = std::strchr(pSymbol, '+');
			if (pEnd)
			{
				*pEnd = '\0';
				pFunction = pSymbol;
				pSymbol = pEnd + 1;
			}
		}

		// Look for the offset
		if (pFunction)
		{
			const auto pEnd = std::strchr(pSymbol, ')');
			if (pEnd)
			{
				*pEnd = '\0';
				pOffset = pSymbol;
				pSymbol = pEnd + 1;
			}
		}

		// Print stack trace number and memory address
		out << "#" << std::dec << std::left << std::setfill(' ')
				<< std::setw(3) << (level - skipFirstNb)
				<< "0x" << std::setfill('0') << std::hex << std::right
				<< std::setw(16) << reinterpret_cast<uint64_t>(addresses[level]);

		char pBuffer[1024];
		if (addressToFileInfo(pBuffer, sizeof(pBuffer), addresses[level], pSourcePath))
		{
			out << " in " << pBuffer;
		}
		else if (pFunction)
		{
			if (demangle(pBuffer, sizeof(pBuffer), pFunction))
			{
				out << " in " << pBuffer;
				if (pOffset)
				{
					out << "+" << pOffset;
				}
			}
		}

		out << " (" << pSourcePath << ")";
		out << std::endl;
	}
}

// ---- print -----------------------------------------------------------------

void IrStd::Exception::toStream(std::ostream& os) const noexcept
{
	IrStd::ExceptionPtr(this).toStream(os);
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

		IRSTD_ASSERT(IrStdException, pE, "ExceptionPtr passed into argument is null");

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
			IRSTD_ASSERT(IrStdException, pE.isException(), "The exception must be of type std::exception");
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

std::string IrStd::Exception::trace() const
{
	std::stringstream streamStr;
	const std::function<void (const IrStd::ExceptionPtr)> printRec
			= [&](const IrStd::ExceptionPtr pE)
	{
		IRSTD_ASSERT(IrStdException, pE, "ExceptionPtr passed into argument is null");

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

std::ostream& operator<<(std::ostream& os, const IrStd::Exception& e)
{
	e.toStream(os);
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::exception& e)
{
	IrStd::ExceptionPtr(&e).toStream(os);
	return os;
}