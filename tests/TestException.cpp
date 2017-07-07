#include "../Test.hpp"
#include "../IrStd.hpp"

class ExceptionTest : public IrStd::Test
{
};

// ---- ExceptionTest::testSimple ---------------------------------------------

TEST_F(ExceptionTest, testSimple) {
	// IrStd::Exception
	try
	{
		IRSTD_THROW("test");
	}
	catch (const IrStd::Exception& e)
	{
		ASSERT_TRUE(validateOutput(e.what(), "test"));
	}

	// std::exception
	try
	{
		IRSTD_THROW("test");
	}
	catch (const std::exception& e)
	{
		ASSERT_TRUE(validateOutput(e.what(), "test"));
	}

	// rethrow
	try
	{
		try
		{
			IRSTD_THROW("test");
		}
		catch (const IrStd::Exception& e)
		{
			IrStd::Exception::rethrow();
		}
	}
	catch (const IrStd::Exception& e)
	{
		ASSERT_TRUE(e.getNext() == nullptr);
	}

	// rethrow std::exception
	try
	{
		try
		{
			throw std::bad_alloc();
		}
		catch (...)
		{
			IrStd::Exception::rethrow();
		}
	}
	catch (const IrStd::Exception& e)
	{
		ASSERT_TRUE(validateOutput(e.what(), "wrapper"));
		const auto pE2 = e.getNext();
		ASSERT_TRUE(pE2);
		ASSERT_TRUE(validateOutput(pE2->what(), "bad_alloc"));
		const auto pE3 = pE2.getNext();
		ASSERT_TRUE(!pE3);
	}
}

// ---- ExceptionTest::testChained --------------------------------------------

TEST_F(ExceptionTest, testChained) {
	// Only one
	try
	{
		IRSTD_THROW("test");
	}
	catch (const IrStd::Exception& e)
	{
		ASSERT_TRUE(validateOutput(e.what(), "test"));
		ASSERT_TRUE(e.getNext() == nullptr);
	}

	// Both of IrStd::Exception type
	try
	{
		try
		{
			IRSTD_THROW("test1");
		}
		catch (...)
		{
			IRSTD_THROW("test2");
		}
	}
	catch (const IrStd::Exception& e1)
	{
		ASSERT_TRUE(validateOutput(e1.what(), "test2"));
		const auto pE2 = e1.getNext();
		ASSERT_TRUE(pE2);
		ASSERT_TRUE(validateOutput(pE2->what(), "test1"));
		const auto pE3 = pE2.getNext();
		ASSERT_TRUE(!pE3);
	}

	// The first one from std::exception type
	try
	{
		try
		{
			throw std::bad_alloc();
		}
		catch (...)
		{
			IRSTD_THROW("nested");
		}
	}
	catch (const IrStd::Exception& e1)
	{
		ASSERT_TRUE(validateOutput(e1.what(), "nested"));
		const auto pE2 = e1.getNext();
		ASSERT_TRUE(pE2);
		ASSERT_TRUE(validateOutput(pE2->what(), "bad_alloc"));
		const auto pE3 = pE2.getNext();
		ASSERT_TRUE(!pE3);
	}
}

// ---- ExceptionTest::testRetry ----------------------------------------------

TEST_F(ExceptionTest, testRetry) {

	// No flag
	try
	{
		IRSTD_THROW("test");
	}
	catch (const IrStd::Exception& e)
	{
		ASSERT_TRUE(e.isAllowRetry() == false);
	}

	// Simple
	try
	{
		IRSTD_THROW_RETRY("test");
	}
	catch (const IrStd::Exception& e)
	{
		ASSERT_TRUE(e.isAllowRetry() == true);
	}

	// rethrow retry
	try
	{
		try
		{
			IRSTD_THROW("test");
		}
		catch (const IrStd::Exception& e)
		{
			ASSERT_TRUE(e.isAllowRetry() == false);
			IrStd::Exception::rethrowRetry();
		}
	}
	catch (const IrStd::Exception& e)
	{
		ASSERT_TRUE(e.isAllowRetry() == true);
		ASSERT_TRUE(e.getNext() == nullptr);
	}

	// rethrow retry std::exception
	try
	{
		try
		{
			throw std::bad_alloc();
		}
		catch (...)
		{
			IrStd::Exception::rethrowRetry();
		}
	}
	catch (const IrStd::Exception& e)
	{
		ASSERT_TRUE(e.isAllowRetry() == true);
		ASSERT_TRUE(validateOutput(e.what(), "wrapper"));
		const auto pE2 = e.getNext();
		ASSERT_TRUE(pE2);
		ASSERT_TRUE(validateOutput(pE2->what(), "bad_alloc"));
		const auto pE3 = pE2.getNext();
		ASSERT_TRUE(!pE3);
	}
}

// ---- ExceptionTest::testPrint ----------------------------------------------

TEST_F(ExceptionTest, testPrint) {

	// Simple
	try
	{
		IRSTD_THROW("test");
	}
	catch (const IrStd::Exception& e)
	{
		std::stringstream stream;
		IrStd::Exception::print(stream);
		ASSERT_TRUE(validateOutput(stream.str().c_str(), "test"));
	}

	// Simple std::exception
	try
	{
		throw std::bad_alloc();
	}
	catch (const std::exception& e)
	{
		std::stringstream stream;
		IrStd::Exception::print(stream);
		ASSERT_TRUE(validateOutput(stream.str().c_str(), "bad_alloc"));
	}

	// Chain
	try
	{
		try
		{
			throw std::bad_alloc();
		}
		catch (...)
		{
			try
			{
				IRSTD_THROW("test1");
			}
			catch (...)
			{
				IRSTD_THROW("test2");
			}
		}
	}
	catch (const std::exception& e)
	{
		std::stringstream stream;
		IrStd::Exception::print(stream);
		ASSERT_TRUE(validateOutput(stream.str().c_str(), "test2[\\s\\S]*test1[\\s\\S]*bad_alloc"));
	}
}
