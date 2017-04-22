#include "../Test.hpp"
#include "../Bootstrap.hpp"
#include "../Assert.hpp"
#include "../Compiler.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <regex>

#if IRSTD_IS_COMPILER(GCC) && ! IRSTD_IS_COMPILER(GCC, 4, 9)
	IRSTD_STATIC_ERROR("Minimum GCC version supported: 4.9, current compiler version: " IRSTD_COMPILER_STRING);
#endif

namespace testing
{
	namespace internal
	{
		enum GTestColor
		{
			COLOR_DEFAULT,
			COLOR_RED,
			COLOR_GREEN,
			COLOR_YELLOW
		};
		extern void ColoredPrintf(GTestColor color, const char* fmt, ...);
	}
}

IrStd::Test::Test()
		: m_cout(std::cerr)
{
}

IrStd::Test::~Test()
{
}

void IrStd::Test::SetUp()
{
	IrStd::Rand::Seed seed = IrStd::Rand::generateSeed();
	m_rand.setSeed(seed);
	{
		std::stringstream stream;
		stream << "Seed: " << std::dec << std::setw(10) << seed;
		print(stream.str().c_str());
	}
}

void IrStd::Test::TearDown()
{
}

void IrStd::Test::print(const char* const output)
{
	::testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] ");
	::testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, "%s\n", output);
}

bool IrStd::Test::validateOutput(const char* const output, const char* const regexStr, const RegexMatch mode, const bool expectSuccess)
{
	const std::regex re(regexStr);
	bool regExprMatch = true;

	if ((mode == RegexMatch::MATCH_ANY && !std::regex_search(output, re))
			|| (mode == RegexMatch::MATCH_ALL && !std::regex_match(output, re)))
	{
		regExprMatch = false;
	}

	std::stringstream header;
	header << "Regular expression: '" << regexStr << "'";

	return validateCondition(regExprMatch, header.str().c_str(), output, expectSuccess);
}

void IrStd::Test::printError(const char* const header, const char* const output, const bool expectSuccess)
{
	getStdout() << "--------------------------------------------------------------------------------" << std::endl;
	getStdout() << "ERROR: " << header << ((!expectSuccess) ? " (expected FAILURE)" : "") << std::endl;
	getStdout() << "-----------------------------------------" << std::endl;
	getStdout() << output << std::endl << std::flush;
	getStdout() << "--------------------------------------------------------------------------------" << std::endl;
}
