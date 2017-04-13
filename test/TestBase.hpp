#include "gtest/gtest.h"

#include <iostream>

namespace Test
{
	class Base : public ::testing::Test
	{
	protected:
		Base();
		virtual ~Base();

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

		void testPrint(const char* const output);
		/**
		 * \brief Validate the standard output of the test
		 *
		 * \param output The string output of the test
		 * \param regExpr Regular expression to match the output
		 * \param expectSuccess Whether or not a successful analyzes of the output is expected
		 *
		 * \return true if the analyze succeed, false otherwise.
		 */
		bool validateOutput(const std::string& output, const char* const regExpr, const bool expectSuccess = true)
		{
			return validateOutput(output.c_str(), regExpr, expectSuccess);
		}
		bool validateOutput(const char* const output, const char* const regExpr, const bool expectSuccess = true);

	private:
		std::ostream& m_cout;

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
