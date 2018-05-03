#include "../Test.hpp"
#include "../IrStd.hpp"

class RWLockTest : public IrStd::Test
{
public:
	void threadRead(IrStd::RWLock& lock, std::string& data);
	void threadWrite(IrStd::RWLock& lock, std::string& data, const char c);
};

// ---- RWLockTest::testSimple ------------------------------------------------

TEST_F(RWLockTest, testSimple)
{
	IrStd::RWLock lock;
	ASSERT_TRUE(lock.getCounter() == 0) << "counter=" << lock.getCounter();
	{
		auto scope = lock.readScope();
		ASSERT_TRUE(lock.getCounter() == 1) << "counter=" << lock.getCounter();
	}
	ASSERT_TRUE(lock.getCounter() == 0) << "counter=" << lock.getCounter();
	{
		auto scope1 = lock.readScope();
		auto scope2 = lock.readScope();
		ASSERT_TRUE(lock.getCounter() == 2) << "counter=" << lock.getCounter();
	}
	ASSERT_TRUE(lock.getCounter() == 0) << "counter=" << lock.getCounter();
	{
		auto scope = lock.writeScope();
		ASSERT_TRUE(lock.getCounter() == -1) << "counter=" << lock.getCounter();
	}
	ASSERT_TRUE(lock.getCounter() == 0) << "counter=" << lock.getCounter();
}

// ---- RWLockTest::testMultiThread -------------------------------------------

void RWLockTest::threadRead(IrStd::RWLock& lock, std::string& data)
{
	size_t loop = 500;
	while (loop--)
	{
		// Read the strign and make sure it contains only the same data
		auto scope = lock.readScope();
		//std::cout << "READ lock acquired Thread" << std::this_thread::get_id() << std::endl;
		const char charType = data[0];
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		ASSERT_TRUE(data.find_first_not_of(charType) == std::string::npos);
		//std::cout << "READ '" << charType << "' Thread" << std::this_thread::get_id() << std::endl;
	}
}

void RWLockTest::threadWrite(IrStd::RWLock& lock, std::string& data, const char charType)
{
	size_t loop = 50;
	while (loop--)
	{
		// Fill the string with the same data
		//std::cout << "WRITE lock Thread" << std::this_thread::get_id() << std::endl;
		auto scope = lock.writeScope();
		//std::cout << "WRITE lock acquired Thread" << std::this_thread::get_id() << std::endl;
		data.assign(data.size(), charType);
		//std::cout << "WRITE '" << charType << "' Thread" << std::this_thread::get_id() << std::endl;
	}
}

TEST_F(RWLockTest, testMultiThread)
{
	std::string data(10000, 'a');
	IrStd::RWLock lock;
	constexpr size_t NB_READ_THREADS = 2;
	constexpr size_t NB_WRITE_THREADS = 10;
	std::thread t[NB_READ_THREADS + NB_WRITE_THREADS];

	// Create the threads
	for (size_t i = 0; i < NB_READ_THREADS; ++i)
	{
		t[i] = std::thread(&RWLockTest::threadRead, this, std::ref(lock), std::ref(data));
	}
	for (size_t i = NB_READ_THREADS; i < NB_READ_THREADS + NB_WRITE_THREADS; ++i)
	{
		t[i] = std::thread(&RWLockTest::threadWrite, this, std::ref(lock), std::ref(data), 'b' + i);
	}

	// Terminate all threads
	for (size_t i = 0; i < NB_READ_THREADS + NB_WRITE_THREADS; ++i)
	{
		t[i].join();
	}
}
