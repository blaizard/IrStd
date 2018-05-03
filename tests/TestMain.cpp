#include "../Test.hpp"
#include "../IrStd.hpp"

class MainTest : public IrStd::Test
{
};

TEST_F(MainTest, testCurrentPath)
{
	const char* pPath = IrStd::Main::getExecutablePath();
	ASSERT_TRUE(strlen(pPath) >= strlen("tests")) << "path=" << pPath;
	ASSERT_TRUE(validateOutput(pPath, "tests$")) << "path=" << pPath;
	getStdout() << "Path: " << pPath << std::endl;
}
