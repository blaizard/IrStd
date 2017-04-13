#include "../Test.hpp"
#include "../Bootstrap.hpp"
#include "../Assert.hpp"
#include "../Compiler.hpp"

#include <sstream>
#include <iostream>
#include <regex>

#if ! IRSTD_IS_COMPILER(GCC, 4, 9)
	#pragma message "Compiler: " IRSTD_COMPILER_STRING
	#error Minimum GCC version supported is 4.9
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
		: m_cout(std::cout)
{
}

IrStd::Test::~Test()
{
}

void IrStd::Test::SetUp()
{
}

void IrStd::Test::TearDown()
{
}

void IrStd::Test::testPrint(const char* const output)
{
	::testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] ");
	::testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, "%s\n", output);
}

bool IrStd::Test::validateOutput(const char* const output, const char* const regExpr, const bool expectSuccess)
{
	const std::regex re(regExpr);
	bool regExprMatch = true;

	if (!std::regex_search(output, re))
	{
		regExprMatch = false;
	}

	std::stringstream header;
	header << "Regular expression: '" << regExpr << "'";

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
