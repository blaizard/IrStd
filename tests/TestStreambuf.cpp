#include "../Test.hpp"
#include "../IrStd.hpp"

class StreambufTest : public IrStd::Test
{
};

// ---- StreambufTest::testCircular -------------------------------------------

TEST_F(StreambufTest, testCircular)
{
	IrStd::Streambuf::Circular circularStreambuf(10);
	std::ostream outStream(&circularStreambuf);

	{
		outStream << "01234";
		std::stringstream streamStr;
		circularStreambuf.toStream(streamStr);
		ASSERT_TRUE(streamStr.str() == "01234") << "str=" << streamStr.str();
	}

	{
		outStream << "56789";
		std::stringstream streamStr;
		circularStreambuf.toStream(streamStr);
		ASSERT_TRUE(streamStr.str() == "0123456789") << "str=" << streamStr.str();
	}

	{
		outStream << "A";
		std::stringstream streamStr;
		circularStreambuf.toStream(streamStr);
		ASSERT_TRUE(streamStr.str() == "23456789A") << "str=" << streamStr.str();
	}

	{
		outStream << "BCDEF";
		std::stringstream streamStr;
		circularStreambuf.toStream(streamStr);
		ASSERT_TRUE(streamStr.str() == "789ABCDEF") << "str=" << streamStr.str();
	}

	{
		std::stringstream streamStr;
		circularStreambuf.dump(streamStr);
		ASSERT_TRUE(validateOutput(streamStr.str().c_str(), "dump=41 42 43 44 45 46 00 37 38 39, ptr=6"))
				<< "str=" << streamStr.str();
	}
}