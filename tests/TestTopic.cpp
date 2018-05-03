#include "../Test.hpp"
#include "../IrStd.hpp"

IRSTD_TOPIC_REGISTER(Test);
IRSTD_TOPIC_REGISTER(Test, Hello);
IRSTD_TOPIC_REGISTER(Test, Hello, World);

IRSTD_TOPIC_USE_ALIAS(TestHelloWorld, Test, Hello, World);

class TopicTest : public IrStd::Test
{
};

// ---- TopicTest::testSimple -------------------------------------------------

TEST_F(TopicTest, testSimple)
{
	ASSERT_TRUE(!strcmp(IRSTD_TOPIC(Test).getStr(), "Test"));
	ASSERT_TRUE(!strcmp(IRSTD_TOPIC(Test, Hello).getStr(), "Test::Hello"));
	ASSERT_TRUE(!strcmp(TestHelloWorld.getStr(), "Test::Hello::World"));
}
