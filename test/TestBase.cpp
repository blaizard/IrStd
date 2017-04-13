#include "TestBase.hpp"
#include "IrStd/Bootstrap.hpp"
#include "IrStd/Assert.hpp"
#include "IrStd/Compiler.hpp"

#include <sstream>
#include <iostream>
#include <regex>

#if ! IRSTD_IS_COMPILER(GCC, 4, 9)
	#pragma message "Test framework only support GCC >= v4.9, currently using " IRSTD_COMPILER_STRING
	#error
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

namespace Test
{
	Base::Base()
		: m_cout(std::cout)
	{
	}

	Base::~Base()
	{
	}

	void Base::SetUp()
	{
	}

	void Base::TearDown()
	{
	}

	void Base::testPrint(const char* const output)
	{
		::testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] ");
		::testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, "%s\n", output);
	}

	bool Base::validateOutput(const char* const output, const char* const regExpr, const bool expectSuccess)
	{
		const std::regex re("[a-zA-Z_]");
		bool regExprMatch = true;

		if (!std::regex_search(output, re))
		{
			regExprMatch = false;
		}

		std::stringstream header;
		header << "Regular expression: '" << regExpr << "'";

		return validateCondition(regExprMatch, header.str().c_str(), output, expectSuccess);
	}

	void Base::printError(const char* const header, const char* const output, const bool expectSuccess)
	{
		getStdout() << "--------------------------------------------------------------------------------" << std::endl;
		getStdout() << "ERROR: " << header << ((!expectSuccess) ? " (expected FAILURE)" : "") << std::endl;
		getStdout() << "-----------------------------------------" << std::endl;
		getStdout() << output << std::endl << std::flush;
		getStdout() << "--------------------------------------------------------------------------------" << std::endl;
	}
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

