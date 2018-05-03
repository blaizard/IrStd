#include "../Test.hpp"
#include "../IrStd.hpp"

//#define DEBUG 1

IRSTD_TOPIC_USE(IrStdThread);

class ThreadTest : public IrStd::Test
{
public:
	void SetUp()
	{
		// Call parent function setup
		IrStd::Test::SetUp();

#if defined(DEBUG)
		// Enable specific traces
		IrStd::Logger::getDefault().addTopic(IrStd::Topic::IrStdThread);
#endif
	}

	void testCreateDeleteFctThread(const int id, int& storeId);
	void testStatusFctThread(std::mutex& lock);
	void testMultiFctThread(const size_t delayMs);
};

// ---- testCreateDelete ------------------------------------------------------

void ThreadTest::testCreateDeleteFctThread(const int id, int& storeId)
{
	storeId = id;
	while (IrStd::Threads::sleep(1000));
}

TEST_F(ThreadTest, testCreateDelete)
{
	constexpr int NB_THREADS = 10;
	struct
	{
		std::thread::id id;
		int store;
	} idList[NB_THREADS];

	// Create the threads
	for (int i = 0; i < NB_THREADS; i++)
	{
		idList[i].store = -1;
		idList[i].id = IrStd::Threads::create("testCreateDeleteFctThread",
				&ThreadTest::testCreateDeleteFctThread, this, i, std::ref(idList[i].store));
		ASSERT_TRUE(idList[i].id != std::thread::id());
		ASSERT_TRUE(IrStd::Threads::isRegistered(idList[i].id));
		ASSERT_TRUE(IrStd::Threads::isActive(idList[i].id));
	}

	// Delete them and make sure they have been running
	for (int i = 0; i < NB_THREADS; ++i)
	{
		IrStd::Threads::terminate(idList[i].id);
		ASSERT_TRUE(idList[i].store == i) << ".store=" << idList[i].store << ", expected " << i;
	}
}

// ---- testStatus ------------------------------------------------------

void ThreadTest::testStatusFctThread(std::mutex& lock)
{
	lock.lock();
}

TEST_F(ThreadTest, testStatus)
{
	{
		IrStd::Thread thread;
		ASSERT_TRUE(thread.isIdle());
	}

	{
		std::mutex lock;
		lock.lock();

		auto thread = IrStd::Thread::create("testStatusFctThread",
				&ThreadTest::testStatusFctThread, this, std::ref(lock));
		ASSERT_TRUE(thread->isActive());

		lock.unlock();

		// Wait until the thread is not active
		while (thread->isActive())
		{
			std::this_thread::yield();
		}

		ASSERT_TRUE(thread->isTerminated());
		thread->terminate();
	}
}

// ---- testConstructor -------------------------------------------------------

static bool testConstructorFctStaticFlag = false;
static void testConstructorFctStatic()
{
	testConstructorFctStaticFlag = true;
}

static bool testConstructorFctStaticCopyFlag = false;
static void testConstructorFctStaticCopy(int a, int b)
{
	ASSERT_TRUE(a == b);
	testConstructorFctStaticCopyFlag = true;
}

static void testConstructorFctStaticReference(int& a)
{
	ASSERT_TRUE(a == -15);
	a = 42;
}

static void testConstructorFctStaticPointer(int* pA)
{
	ASSERT_TRUE(pA != nullptr);
	ASSERT_TRUE(*pA == -15);
	*pA = 42;
}

TEST_F(ThreadTest, testConstructor)
{
	// Static no arguments
	{
		auto thread = IrStd::Thread::create("testConstructorFctStatic",
				testConstructorFctStatic);
		thread->terminate();
		ASSERT_TRUE(testConstructorFctStaticFlag);
	}
	// Static copy
	{
		int a = 1;
		auto thread = IrStd::Thread::create("testConstructorFctStaticCopy",
				testConstructorFctStaticCopy, a, 1);
		thread->terminate();
		ASSERT_TRUE(testConstructorFctStaticCopyFlag);
	}
	// Static reference
	{
		int a = -15;
		auto thread = IrStd::Thread::create("testConstructorFctStaticReference",
				testConstructorFctStaticReference, std::ref(a));
		thread->terminate();
		ASSERT_TRUE(a == 42);
	}
	// Static pointer
	{
		int a = -15;
		auto thread = IrStd::Thread::create("testConstructorFctStaticPointer",
				testConstructorFctStaticPointer, &a);
		thread->terminate();
		ASSERT_TRUE(a == 42);
	}
}

// ---- testMulti ------------------------------------------------------------

void ThreadTest::testMultiFctThread(const size_t delayMs)
{
	IrStd::Threads::sleep(delayMs);
}

TEST_F(ThreadTest, testMulti)
{
	{
		constexpr size_t NB_THREADS = 100;
		for (size_t i = 0; i<NB_THREADS; ++i)
		{
			std::string name("testMultiFctThread-");
			name.append(IrStd::Type::ShortString(i));
			IrStd::Threads::create(name.c_str(),
					&ThreadTest::testMultiFctThread, this, m_rand.getNumber<size_t>(0, NB_THREADS));
		}
		// This will ioncrease probability that some of the threads are
		// deleted by themselves and others delete with the terminate function
		std::this_thread::sleep_for(std::chrono::milliseconds(NB_THREADS / 2));

		IrStd::Threads::terminate();
	}
}

// ---- testPool --------------------------------------------------------------

TEST_F(ThreadTest, testPool)
{
	int counter = 0;
	{
		IrStd::ThreadPool<3> pool("testPool");
		std::mutex mutex;

		for (size_t i=0; i<100; ++i)
		{
			pool.addJob([&]() {
				std::unique_lock<std::mutex> lock(mutex);
				//std::cout << "Job done by " << std::this_thread::get_id() << " -> " << counter << std::endl;
				++counter;
			});
		}

		pool.waitForAllJobsToBeCompleted();
	}

	ASSERT_TRUE(counter == 100) << "counter=" << counter;
}
