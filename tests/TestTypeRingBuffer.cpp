#include "../Test.hpp"
#include "../IrStd.hpp"

class TypeRingBufferTest : public IrStd::Test
{
public:
	static constexpr size_t NB_ELTS = 1000;
	void threadSortedRead(IrStd::Type::RingBufferSorted<size_t, size_t, NB_ELTS>& circular, size_t& nbRead);
};

// ---- TypeRingBufferTest::testSimple ------------------------------------------

TEST_F(TypeRingBufferTest, testSimple)
{
	constexpr size_t NB_ELTS = 10;
	IrStd::Type::RingBuffer<size_t, NB_ELTS> circular;

	// Push first element
	{
		ASSERT_TRUE(circular.size() == 0) << "size=" << circular.size();
		circular.push(42);
		ASSERT_TRUE(circular.head() == 42) << "head=" << circular.head();
		ASSERT_TRUE(circular.tail() == 42) << "tail=" << circular.tail();
	}

	// Push the next 9th element
	for (size_t expectedNb=1; expectedNb<NB_ELTS; expectedNb++)
	{
		ASSERT_TRUE(circular.size() == expectedNb) << "size=" << circular.size();
		circular.push(expectedNb);
		ASSERT_TRUE(circular.head() == expectedNb) << "head=" << circular.head();
		ASSERT_TRUE(circular.tail() == 42) << "tail=" << circular.tail();
		ASSERT_TRUE(circular.size() == expectedNb + 1) << "size=" << circular.size();
	}

	// Push one more, the loop should start
	{
		ASSERT_TRUE(circular.size() == NB_ELTS) << "size=" << circular.size();
		circular.push(100);
		ASSERT_TRUE(circular.size() == NB_ELTS) << "size=" << circular.size();
		ASSERT_TRUE(circular.head() == 100) << "head=" << circular.head();
		ASSERT_TRUE(circular.tail() == 1) << "tail=" << circular.tail();
	}
}

// ---- TypeRingBufferTest::testSorted ------------------------------------------

TEST_F(TypeRingBufferTest, testSorted)
{
	IrStd::Type::RingBufferSorted<size_t, size_t, 200> circular;

	// Read interval when empty
	{
		const auto isCompleted = circular.readIntervalByKey(0, 10, [&](const size_t key, const size_t element) {
			ASSERT_TRUE(0) << "key=" << key << ", element=" << element;
		});
		ASSERT_TRUE(isCompleted == true);
	}

	// Fill the data structure
	{
		for (size_t i=5; i<190; i++)
		{
			circular.push(i, i + 1000);
		}
		ASSERT_TRUE(circular.size() == 185) << "size=" << circular.size();
	}

	// Get the interval from the begining
	{
		size_t nbFound = 0;
		size_t expected = 189;
		const auto isCompleted = circular.readIntervalByKey(189, 180, [&](const size_t key, const size_t element) {
			ASSERT_TRUE(key == expected) << "key=" << key << ", expected=" << expected;
			ASSERT_TRUE(element == expected + 1000) << "element=" << element << ", expected=" << expected;
			nbFound++;
			expected--;
		});
		ASSERT_TRUE(nbFound == 10) << "nbFound=" << nbFound;
		ASSERT_TRUE(isCompleted == true);
	}

	// Get the interval from before the begining
	{
		size_t nbFound = 0;
		size_t expected = 189;
		const auto isCompleted = circular.readIntervalByKey(200, 180, [&](const size_t key, const size_t element) {
			ASSERT_TRUE(key == expected) << "key=" << key << ", expected=" << expected;
			ASSERT_TRUE(element == expected + 1000) << "element=" << element << ", expected=" << expected;
			nbFound++;
			expected--;
		});
		ASSERT_TRUE(nbFound == 10) << "nbFound=" << nbFound;
		ASSERT_TRUE(isCompleted == true);
	}

	// Get the interval from the middle
	{
		size_t nbFound = 0;
		size_t expected = 99;
		const auto isCompleted = circular.readIntervalByKey(99, 90, [&](const size_t key, const size_t element) {
			ASSERT_TRUE(key == expected) << "key=" << key << ", expected=" << expected;
			ASSERT_TRUE(element == expected + 1000) << "element=" << element << ", expected=" << expected;
			nbFound++;
			expected--;
		});
		ASSERT_TRUE(nbFound == 10) << "nbFound=" << nbFound;
		ASSERT_TRUE(isCompleted == true);
	}

	// Get only one element
	{
		size_t nbFound = 0;
		const auto isCompleted = circular.readIntervalByKey(10, 10, [&](const size_t key, const size_t element) {
			ASSERT_TRUE(key == 10) << "key=" << key;
			ASSERT_TRUE(element == 1010) << "element=" << element;
			nbFound++;
		});
		ASSERT_TRUE(nbFound == 1) << "nbFound=" << nbFound;
		ASSERT_TRUE(isCompleted == true);
	}

	// Get the interval from something too new
	{
		size_t nbFound = 0;
		const auto isCompleted = circular.readIntervalByKey(230, 200, [&](const size_t key, const size_t element) {
			ASSERT_TRUE(0) << "key=" << key << ", element=" << element;;
			nbFound++;
		});
		ASSERT_TRUE(nbFound == 0) << "nbFound=" << nbFound;
		ASSERT_TRUE(isCompleted == true);
	}

	// Get the interval from something too old
	{
		size_t nbFound = 0;
		const auto isCompleted = circular.readIntervalByKey(0, 0, [&](const size_t key, const size_t element) {
			ASSERT_TRUE(0) << "key=" << key << ", element=" << element;;
			nbFound++;
		});
		ASSERT_TRUE(nbFound == 0) << "nbFound=" << nbFound;
		ASSERT_TRUE(isCompleted == true);
	}

	// Get the interval from something in the interval until too old
	{
		size_t nbFound = 0;
		size_t expected = 10;
		const auto isCompleted = circular.readIntervalByKey(10, 0, [&](const size_t key, const size_t element) {
			ASSERT_TRUE(key == expected) << "key=" << key << ", expected=" << expected;
			ASSERT_TRUE(element == expected + 1000) << "element=" << element << ", expected=" << expected;
			nbFound++;
			expected--;
		});
		ASSERT_TRUE(nbFound == 6) << "nbFound=" << nbFound;
		ASSERT_TRUE(isCompleted == true);
	}
}

// ---- TypeRingBufferTest::testFind --------------------------------------------

TEST_F(TypeRingBufferTest, testFind)
{
	IrStd::Type::RingBufferSorted<size_t, size_t, 10> circular;

	// Find with no elements, do not care of the output, only ensure that it does not hang
	{
		ASSERT_TRUE(circular.find(0, /*oldest*/true) == 0) << circular.find(0, /*oldest*/true);
		ASSERT_TRUE(circular.find(0, /*oldest*/false) == 0) << circular.find(0, /*oldest*/false);
		ASSERT_TRUE(circular.find(10, /*oldest*/true) == 0) << circular.find(10, /*oldest*/true);
		ASSERT_TRUE(circular.find(10, /*oldest*/false) == 0) << circular.find(10, /*oldest*/false);
	}

	// Find exact match (unique key)
	{
		circular.clear();
		// With one element
		circular.push(1, 10);
		ASSERT_TRUE(circular.find(1, /*oldest*/true) == 1) << "pos=" << circular.find(1, /*oldest*/true);
		ASSERT_TRUE(circular.find(1, /*oldest*/false) == 1) << "pos=" << circular.find(1, /*oldest*/false);
		// multiple elements
		circular.push(2, 10);
		circular.push(4, 10);
		circular.push(5, 10);
		ASSERT_TRUE(circular.find(2, /*oldest*/true) == 2) << "pos=" << circular.find(2, /*oldest*/true);
		ASSERT_TRUE(circular.find(2, /*oldest*/false) == 2) << "pos=" << circular.find(2, /*oldest*/false);
		ASSERT_TRUE(circular.find(5, /*oldest*/true) == 4) << "pos=" << circular.find(5, /*oldest*/true);
		ASSERT_TRUE(circular.find(5, /*oldest*/false) == 4) << "pos=" << circular.find(5, /*oldest*/false);
		ASSERT_TRUE(circular.find(1, /*oldest*/true) == 1) << "pos=" << circular.find(1, /*oldest*/true);
		ASSERT_TRUE(circular.find(1, /*oldest*/false) == 1) << "pos=" << circular.find(1, /*oldest*/false);
		// at the limit
		circular.push(6, 10);
		circular.push(7, 10);
		circular.push(8, 10);
		circular.push(9, 10);
		circular.push(10, 10);
		circular.push(11, 10);
		ASSERT_TRUE(circular.find(11, /*oldest*/true) == 10) << "pos=" << circular.find(11, /*oldest*/true);
		ASSERT_TRUE(circular.find(11, /*oldest*/false) == 10) << "pos=" << circular.find(11, /*oldest*/false);
		ASSERT_TRUE(circular.find(1, /*oldest*/true) == 1) << "pos=" << circular.find(1, /*oldest*/true);
		ASSERT_TRUE(circular.find(1, /*oldest*/false) == 1) << "pos=" << circular.find(1, /*oldest*/false);
	}

	// Something that does not exists
	{
		circular.clear();
		circular.push(2, 10);
		circular.push(3, 10);
		circular.push(4, 10);
		circular.push(5, 10);

		// Something that does not exists (too small)
		ASSERT_TRUE(circular.find(1, /*oldest*/true) == 1) << "pos=" << circular.find(0, /*oldest*/true);
		ASSERT_TRUE(circular.find(1, /*oldest*/false) == 1) << "pos=" << circular.find(0, /*oldest*/false);
		// Something that does not exists (too high)
		ASSERT_TRUE(circular.find(6, /*oldest*/true) == circular.getIndex()) << "pos=" << circular.find(6, /*oldest*/true);
		ASSERT_TRUE(circular.find(6, /*oldest*/false) == circular.getIndex()) << "pos=" << circular.find(6, /*oldest*/false);
	}

	// Find exact match multiple same values
	{
		circular.clear();
		circular.push(6, 10);
		circular.push(6, 10);
		circular.push(7, 10);
		circular.push(9, 10);
		circular.push(9, 10);
		circular.push(10, 10);
		circular.push(10, 10);
		ASSERT_TRUE(circular.find(6, /*oldest*/true) == 1) << "pos=" << circular.find(6, /*oldest*/true);
		ASSERT_TRUE(circular.find(6, /*oldest*/false) == 2) << "pos=" << circular.find(6, /*oldest*/false);
		ASSERT_TRUE(circular.find(9, /*oldest*/true) == 4) << "pos=" << circular.find(9, /*oldest*/true);
		ASSERT_TRUE(circular.find(9, /*oldest*/false) == 5) << "pos=" << circular.find(9, /*oldest*/false);
		ASSERT_TRUE(circular.find(10, /*oldest*/true) == 6) << "pos=" << circular.find(10, /*oldest*/true);
		ASSERT_TRUE(circular.find(10, /*oldest*/false) == 7) << "pos=" << circular.find(10, /*oldest*/false);
	}

	// Find in between match
	{
		circular.clear();
		circular.push(6, 10);
		circular.push(7, 10);
		circular.push(9, 10);
		circular.push(10, 10);
		ASSERT_TRUE(circular.find(8, /*oldest*/true) == 2) << "pos=" << circular.find(8, /*oldest*/true);
		ASSERT_TRUE(circular.find(8, /*oldest*/false) == 3) << "pos=" << circular.find(8, /*oldest*/false);
	}
}

// ---- TypeRingBufferTest::testSortedMultiThread -------------------------------

void TypeRingBufferTest::threadSortedRead(IrStd::Type::RingBufferSorted<size_t, size_t, NB_ELTS>& circular, size_t& nbRead)
{
	while (nbRead < 1000)
	{
		// Standard algorithm (dichotomy)
		{
			const auto beginElement = circular.head();
			size_t expectedKey = beginElement.first;
			circular.readIntervalByKey(beginElement.first, (beginElement.first > 100) ? beginElement.first - 100 : 0,
					[&](const size_t key, const size_t) {
			//	std::cout << "key=" << key << ", expectedKey=" << expectedKey << std::endl;
				ASSERT_TRUE(key == expectedKey) << "key=" << key << ", expected=" << expectedKey;
				expectedKey--;
			});
		}

		nbRead++;
	}
}

TEST_F(TypeRingBufferTest, testSortedMultiThread)
{
	IrStd::Type::RingBufferSorted<size_t, size_t, NB_ELTS> circular;
	size_t nbRead = 0;

	// Create the thread and run it
	std::thread t = std::thread(&TypeRingBufferTest::threadSortedRead, this, std::ref(circular), std::ref(nbRead));

	// Fill the data structure constantly
	{
		size_t i = 0;
		while (nbRead < 1000)
		{
			circular.push(i, 0);
			i++;
		}
	}

	// Terminate the read thread
	t.join();
}
