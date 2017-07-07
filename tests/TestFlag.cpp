#include "../Test.hpp"
#include "../IrStd.hpp"

class FlagTest : public IrStd::Test
{
public:
	void threadFlagLockThreadMulti(const size_t id, IrStd::FlagLockThread& flag, volatile size_t& sharedRessource);
};

TEST_F(FlagTest, testFlagBool)
{
	IrStd::FlagBool flag;
	ASSERT_TRUE(!flag.isSet());

	// Set from false to true
	{
		const bool prev = flag.setAndGet();
		ASSERT_TRUE(flag.isSet() && !prev);
	}

	// Set from true to true
	{
		const bool prev = flag.setAndGet();
		ASSERT_TRUE(flag.isSet() && prev);
	}

	// Set from true to false
	{
		const bool prev = flag.unsetAndGet();
		ASSERT_TRUE(!flag.isSet() && prev);
	}

	// Set from false to false
	{
		const bool prev = flag.unsetAndGet();
		ASSERT_TRUE(!flag.isSet() && !prev);
	}
}

TEST_F(FlagTest, testFlagLock)
{
	IrStd::FlagLock flag;
	ASSERT_TRUE(!flag.isSet());

	// Set from false to true
	{
		const bool prev = flag.setAndGet();
		ASSERT_TRUE(flag.isSet() && !prev);
	}

	// Set from true to true will result ot a deadlock

	// Set from true to false
	{
		const bool prev = flag.unsetAndGet();
		ASSERT_TRUE(!flag.isSet() && prev);
	}

	// Set from false to false is not permitted
}

// ---- IrStd::FlagLockThread -------------------------------------------------

TEST_F(FlagTest, testFlagLockThread)
{
	IrStd::FlagLockThread flag;
	ASSERT_TRUE(!flag.isSet());

	// Set from false to true
	{
		const bool prev = flag.setAndGet();
		ASSERT_TRUE(flag.isSet() && !prev);
	}

	// Set from true to true
	{
		const bool prev = flag.setAndGet();
		ASSERT_TRUE(flag.isSet() && prev);
	}

	// Unset from true to false
	{
		const bool prev = flag.unsetAndGet();
		ASSERT_TRUE(!flag.isSet() && prev);
	}

	// Unset from false to false
	{
		const bool prev = flag.unsetAndGet();
		ASSERT_TRUE(!flag.isSet() && !prev);
	}
}

void FlagTest::threadFlagLockThreadMulti(const size_t id, IrStd::FlagLockThread& flag, volatile size_t& sharedRessource)
{
	size_t loop = 50;
	while (loop--)
	{

	//	std::cout << id << "  SET1 " << loop << std::endl;

		flag.setAndGet();

	//	std::cout << id << "  SET2 " << loop << std::endl;

		sharedRessource = id;
		for (size_t i = 0; i<1000; ++i)
		{
			ASSERT_TRUE(sharedRessource == id) << "sharedRessource has been changed from "
					<< id << " to " << sharedRessource;
			std::this_thread::yield();
		}
	//	std::cout << id << "  UNSET1 " << loop << std::endl;

		flag.unsetAndGet();

	//	std::cout << id << "  UNSET2 " << loop << std::endl;
	}
}

TEST_F(FlagTest, testFlagLockThreadMulti)
{
	constexpr size_t NB_THREADS = 10;

	IrStd::FlagLockThread flag;
	size_t sharedRessource;
	std::thread t[NB_THREADS];

	for (size_t i = 0; i < NB_THREADS; ++i)
	{
		t[i] = std::thread(&FlagTest::threadFlagLockThreadMulti, this, i, std::ref(flag), std::ref(sharedRessource));
	}

	for (size_t i = 0; i < NB_THREADS; ++i)
	{
		t[i].join();
	}
}
