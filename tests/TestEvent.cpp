#include <thread>

#include "../Test.hpp"
#include "../IrStd.hpp"

#define DEBUG 1

IRSTD_TOPIC_USE(IrStdEvent);

class EventTest : public IrStd::Test
{
public:
	void SetUp()
	{
		// Call parent function setup
		IrStd::Test::SetUp();

#if defined(DEBUG)
		// Enable specific traces
		IrStd::Logger::getDefault().addTopic(IrStd::Topic::IrStdEvent);
#endif
	}

	void testWaitForNextFctThread(IrStd::Event& event);
	void testWaitForAtLeastFctThread(IrStd::Event& event, const size_t nbThreads, bool& isBeforeWait);
};

// ---- testWaitForNext -------------------------------------------------------

void EventTest::testWaitForNextFctThread(IrStd::Event& event)
{
	event.waitForNext();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	event.trigger();
}

TEST_F(EventTest, testWaitForNext)
{
	IrStd::Event event;
	std::thread t(&EventTest::testWaitForNextFctThread, this, std::ref(event));

	// Ensure the thread reached the waitForNext statement
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	event.trigger();

	// Wait for the thread to trigger
	event.waitForNext();

	t.join();
}

// ---- testWaitForAtLeast ----------------------------------------------------

void EventTest::testWaitForAtLeastFctThread(IrStd::Event& event, const size_t nbThreads, bool& isBeforeWait)
{
	ASSERT_TRUE(isBeforeWait == true);
	event.trigger();

	// Wait until the main program triggers the start to all pending threads
	event.waitForAtLeast(nbThreads + 1);
	ASSERT_TRUE(isBeforeWait == false);
}

TEST_F(EventTest, testWaitForAtLeast)
{
	constexpr size_t NB_THREADS = 100;
	std::thread threadList[NB_THREADS];
	IrStd::Event event;
	bool isBeforeWait = true;

	for (size_t i=0; i<NB_THREADS; i++)
	{
		threadList[i] = std::thread(&EventTest::testWaitForAtLeastFctThread, this,
				std::ref(event), NB_THREADS, std::ref(isBeforeWait));
	}

	// Wait until all thread have been started
	event.waitForAtLeast(NB_THREADS);
	isBeforeWait = false;

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	event.trigger();

	for (size_t i=0; i<NB_THREADS; i++)
	{
		threadList[i].join();
	}
}
