#include <ostream>
#include <cxxabi.h>
#include <execinfo.h>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iomanip>

#include "../Compiler.hpp"
#include "../Exception.hpp"

static std::string demangle(const char* const symbol)
{
	const std::unique_ptr<char, decltype(&std::free)> demangled(abi::__cxa_demangle(symbol, 0, 0, 0), &std::free);
	return (demangled) ? demangled.get() : symbol;
}

void IrStd::Exception::callStack(std::ostream& out, const size_t skipFirstNb) noexcept
{
	const size_t MAX_STACK_LEVEL = 64;
	void* addresses[MAX_STACK_LEVEL];

	const int nbLevels = ::backtrace(addresses, MAX_STACK_LEVEL);
	const std::unique_ptr<char*, decltype(&std::free)> symbols(::backtrace_symbols(addresses, nbLevels), &std::free);

	for(size_t level = skipFirstNb; level < nbLevels; ++level)
	{
		char* const symbol = symbols.get()[level];
		char* end = symbol;
		while (*end)
		{
			++end;
		}
		while (end != symbol && *end != '+')
		{
			--end;
		}
		char* begin = end;
		while (begin != symbol && *begin != '(')
		{
			--begin;
		}

		out << "#" << std::dec << std::left << std::setw(3) << (level - skipFirstNb);
		if (begin != symbol)
		{
			out << std::string(symbol, ++begin - symbol);
			*end++ = '\0';
			out << demangle(begin) << '+' << end;
		}
		else
		{
			out << symbol;
		}
		out << std::endl;
	}
}
