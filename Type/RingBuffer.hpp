#pragma once

#include <mutex>
#include <array>
#include <atomic>

#include "../Assert.hpp"
#include "../Topic.hpp"

IRSTD_TOPIC_USE(IrStd, Type);

namespace IrStd
{
	namespace Type
	{
		/**
		 * Lock free data structure
		 */
		template<class T, size_t N>
		class RingBuffer
		{
		public:
			RingBuffer()
					: m_indexWrite(0)
					, m_indexRead(0)
			{
			}

			/**
			 * Access an element from the array.
			 * 0 is the current element, 1 is the previous element,
			 * 2 is the second previous...
			 */
			T head(const size_t index = 0) const noexcept
			{
				return loadForRead(m_indexRead.load() - index);
			}

			/**
			 * Access the tail of the array,
			 * in other word, the oldest element pushed.
			 */
			T tail(const size_t index = 0) const noexcept
			{
				size_t curIndex;
				T data;
				// Lock-free
				do
				{
					curIndex = m_indexRead.load();
					data = (curIndex < N) ? loadForRead(1 + index) : loadForRead(curIndex + 1 + index);
				} while (curIndex != m_indexRead.load());

				return data;
			}

			/**
			 * Return the number of valid data present in the buffer
			 */
			size_t size() const noexcept
			{
				return std::min(N, m_indexRead.load());
			}

			/**
			 * Clear the RingBuffer buffer
			 */
			void clear() noexcept
			{
				m_indexRead = 0;
				m_indexWrite = 0;
			}

			/**
			 * Add a new element to the head of the data structure
			 *
			 * \return the index of the element added
			 */
			size_t push(const T& element) noexcept
			{
				IRSTD_ASSERT(IRSTD_TOPIC(IrStd, Type), m_indexWrite >= m_indexRead,
						"m_indexWrite=" << m_indexWrite.load() << ", m_indexRead=" << m_indexRead.load());

				// Increase and reserve the index
				auto curIndex = m_indexWrite.load();
				while (!m_indexWrite.compare_exchange_weak(curIndex, curIndex + 1))
				{
					// Loop until the exchange works
				}

				// Store the value
				{
					auto& reference = loadForWrite(curIndex + 1);
					reference = element;
					++m_indexRead;
				}

				// Synchronization loop, make sure the write index of the current push
				// is now in sync with the read. No need for a mutex here, it will be overkill
				// The operation is exected to be very fast (atomic) in many cases, so a yield will do
				while (curIndex > m_indexRead)
				{
					std::this_thread::yield();
				}

				return curIndex + 1;
			}

			/**
			 * Get the last n entries
			 */
			bool read(
					const std::function<void(const T&)>& callback,
					const size_t nbEntries = N) const noexcept
			{
				const auto headIndex = m_indexRead.load();
				return readInterval(headIndex, (nbEntries > headIndex) ? 0 : (headIndex - nbEntries), callback);
			}

			/**
			 * \brief Read entries between 2 indexes (inclusive)
			 *
			 * Entries are read in ascending order if indexBegin is
			 * smaller than indexEnd, otherwise in descending order
			 *
			 * \param indexBegin The start index
			 * \param indexEnd The ending index
			 * \param callback The function to be called for processing the data
			 *
			 * \return false in case some data requested where not processed,
			 *         true if everything has been processed.
			 */
			bool readInterval(
					const size_t indexBegin,
					const size_t indexEnd,
					const std::function<void(const T&)>& callback) const noexcept
			{
				if (indexBegin > indexEnd)
				{
					size_t curIndex = std::min(indexBegin, m_indexRead.load());
					size_t nbProcessed = 0;
					while (curIndex >= indexEnd)
					{
						// Read the data and make a copy of it
						const auto data = loadForRead(curIndex);
						// Index can only become old, as we start from a valid index
						// hence make sure it did not turned old after we read the data
						if (isIndexTooOld(curIndex))
						{
							break;
						}
						callback(data);
						++nbProcessed;
						--curIndex;
					}
					return (nbProcessed == (indexBegin - indexEnd));
				}
				else if (indexBegin < indexEnd)
				{
					size_t curIndex = std::max(indexBegin, m_indexWrite.load() - size() + 1);
					size_t nbProcessed = 0;
					while (curIndex <= indexEnd && !isIndexTooNew(curIndex))
					{
						// Read the data and make a copy of it
						const auto data = loadForRead(curIndex);
						if (isIndexTooOld(curIndex))
						{
							if (nbProcessed)
							{
								IRSTD_LOG_FATAL(IRSTD_TOPIC(IrStd, Type), "Read overflow, writing speed is faster than reading speed"
										<< "curIndex=" << curIndex << ", m_indexRead=" << m_indexRead.load()
										<< ", m_indexWrite=" << m_indexWrite.load() << ", size=" << size());
								return false;
							}
							++curIndex;
							continue;
						}
						callback(data);
						++nbProcessed;
						++curIndex;
					}
					return (nbProcessed == (indexEnd - indexBegin));
				}

				return true;
			}

			const T& loadForRead(const size_t index) const noexcept
			{
				return m_data[index % N];
			}
			T& loadForWrite(const size_t index) noexcept
			{
				return m_data[index % N];
			}

			/**
			 * Return the latest index
			 */
			size_t getIndex() const noexcept
			{
				return m_indexRead.load();
			}

			/**
			 * Valid index are ciomprise between m_indexRead and
			 * m_indexWrite - size of the buffer
			 */
			bool isValidIndex(const size_t index) const noexcept
			{
				return !(isIndexTooNew(index) || isIndexTooOld(index));
			}

			bool isIndexTooNew(const size_t index) const noexcept
			{
				return (index > m_indexRead.load());
			}

			bool isIndexTooOld(const size_t index) const noexcept
			{
				return (index <= m_indexWrite.load() - size());
			}

		protected:
			std::atomic<size_t> m_indexWrite;
			std::atomic<size_t> m_indexRead;
			std::array<T, N> m_data;
		};
	}
}
