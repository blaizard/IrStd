#include <thread>

#include "../Test.hpp"
#include "../IrStd.hpp"

#define DEBUG 1

IRSTD_TOPIC_USE(IrStd, Event);

class EventTest : public IrStd::Test
{
public:
	void SetUp()
	{
		// Call parent function setup
		IrStd::Test::SetUp();

#if defined(DEBUG)
		// Enable specific traces
		IrStd::Logger::getDefault().addTopic(IRSTD_TOPIC(IrStd, Event));
#endif
	}

	void testWaitForNextFctThread(IrStd::Event& event);
	void testWaitForNextsFctThread(IrStd::Event& event, IrStd::Event& localEvent);
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

// ---- testWaitForNexts ------------------------------------------------------

void EventTest::testWaitForNextsFctThread(IrStd::Event& event, IrStd::Event& localEvent)
{
	event.waitForNext();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	localEvent.trigger();
}

TEST_F(EventTest, testWaitForNexts)
{
	constexpr size_t NB_THREADS = 100;
	std::thread threadList[NB_THREADS];
	std::array<IrStd::Event, NB_THREADS> eventList;
	IrStd::Event event;

	// Create events and its associated pointer list and also create the threads
	std::array<IrStd::Event*, NB_THREADS> pointerEventList;
	for (size_t i=0; i<NB_THREADS; i++)
	{
		pointerEventList[i] = &eventList[i];
		threadList[i] = std::thread(&EventTest::testWaitForNextsFctThread, this,
				std::ref(event), std::ref(eventList[i]));
	}

	// Ensure the thread reached the waitForNext statement
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	event.trigger();

	// Wait for all events
	const auto* pEvent = event.waitForNexts(/*timeoutMs*/1000, const_cast<const std::array<IrStd::Event*, NB_THREADS>&>(pointerEventList));
	ASSERT_TRUE(pEvent == nullptr) << "pEvent=" << *pEvent;

	for (size_t i=0; i<NB_THREADS; i++)
	{
		threadList[i].join();
	}
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
