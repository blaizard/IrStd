#include <iostream>
#include <string>

// Include gtest API
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waggregate-return"
#include "gtest/gtest.h"
#pragma GCC diagnostic pop
#pragma GCC diagnostic ignored "-Wstrict-overflow"

#include "Memory.hpp"
#include "Rand.hpp"

#define IRSTD_TEST_REGEX_EOL "\\r?\\n"

namespace IrStd
{
	class Test : public ::testing::Test
	{
	public:
		static void print(const std::string& output);

	protected:
		Test();
		virtual ~Test();

		virtual void SetUp();
		virtual void TearDown();

		/**
		 * \brief Returns a reference on the test standard output
		 * \return Standard output used for the test
		 */
		std::ostream& getStdout()
		{
			return m_cout;
		}

		enum class RegexMatch
		{
			MATCH_ANY,
			MATCH_ALL
		};
		/**
		 * \brief Validate the standard output of the test
		 *
		 * \param output The string output of the test
		 * \param regexStr Regular expression to match the output
		 * \param expectSuccess Whether or not a successful analyzes of the output is expected
		 *
		 * \return true if the analyze succeed, false otherwise.
		 */
		bool validateOutput(const std::string& output, const char* const regexStr, const bool expectSuccess = true)
		{
			return validateOutput(output.c_str(), regexStr, RegexMatch::MATCH_ANY, expectSuccess);
		}
		bool validateOutput(const std::string& output, const char* const regexStr, const RegexMatch mode, const bool expectSuccess = true)
		{
			return validateOutput(output.c_str(), regexStr, mode, expectSuccess);
		}
		bool validateOutput(const char* const output, const char* const regexStr, const RegexMatch mode = RegexMatch::MATCH_ANY, const bool expectSuccess = true);

		Rand m_rand;

	private:
		std::ostream& m_cout;
		IrStd::Memory::StatisticsScope m_statisticsScope;

		bool validateCondition(const bool isMatch, const char* const header, const char* const output, const bool expectSuccess)
		{
			if (isMatch != expectSuccess)
			{
				printError(header, output, expectSuccess);
				return false;
			}
			return true;
		}
		void printError(const char* const header, const char* const output, const bool expectSuccess);
	};
}
