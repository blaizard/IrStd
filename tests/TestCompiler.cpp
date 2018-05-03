#include "../Test.hpp"
#include "../IrStd.hpp"

class CompilerTest : public IrStd::Test
{
};

TEST_F(CompilerTest, testBuildVersion)
{
	const auto& version = IrStd::Compiler::getBuildVersion();
	ASSERT_TRUE(validateOutput(version, "20[1-9][0-9][0-1][0-9][0-3][0-9]\\.[0-2][0-9][0-5][0-9][0-5][0-9]", RegexMatch::MATCH_ALL));
}
