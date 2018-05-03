#pragma once

#include "RingBuffer.hpp"
#include "../Assert.hpp"
#include "../Topic.hpp"

IRSTD_TOPIC_USE(IrStd, Type);

namespace IrStd
{
	namespace Type
	{
		/**
		 * \brief Circular data structure with sorted element over
		 * a sorted key.
		 *
		 * Sorted means that a new element must have a key greater
		 * or equal than its predecessor.
		 */
		template<class K, class T, size_t N>
		class RingBufferSorted : public RingBuffer<std::pair<K, T>, N>
		{
		private:
			typedef RingBuffer<std::pair<K, T>, N> Base;

		public:
			size_t push(const T& element) noexcept = delete;

			/**
			 * Return the latest key pushed
			 */
			K getLastestKey() const noexcept
			{
				return Base::loadForRead(Base::m_indexRead.load()).first;
			}

			/**
			 * Add a new element, make sure that the key is sorted
			 *
			 * \return The index of the element added
			 */
			size_t push(const K& key, const T& element) noexcept
			{
				if (Base::m_indexRead.load() > 0)
				{
					const auto& prevKey = Base::head().first;
					IRSTD_ASSERT(IRSTD_TOPIC(IrStd, Type), key >= prevKey,
							"The element pushed into the RingBufferSorted is not sorted");
				}
				return Base::push({key, element});
			}

			/**
			 * \brief Return the data between an interval (keyBegin, keyEnd)
			 * \return true if all the elements requested have been delivered, false otherwise.
			 */
			bool readIntervalByKey(const K& keyBegin, const K& keyEnd,
					const std::function<void(const K&, const T&)>& callback) const noexcept
			{
				// Read in the descending order, from the newest to the oldest
				if (keyBegin > keyEnd)
				{
					auto curIndex = find(keyBegin, /*oldest*/false);
					while (true)
					{
						// Read the data and make a copy of it
						const auto data = Base::loadForRead(curIndex);
						// Index can only become old, as we start from a valid index
						// hence make sure it did not turned old after we read the data
						if (Base::isIndexTooOld(curIndex) || data.first < keyEnd)
						{
							break;
						}
						callback(data.first, data.second);
						--curIndex;
					}
				}
				else
				{
					auto curIndex = find(keyBegin, /*oldest*/true);
					size_t nbProcessed = 0;
					while (!Base::isIndexTooNew(curIndex))
					{
						// Read the data and make a copy of it
						const auto data = Base::loadForRead(curIndex);
						if (Base::isIndexTooOld(curIndex))
						{
							if (nbProcessed)
							{
								IRSTD_LOG_FATAL(IRSTD_TOPIC(IrStd, Type), "Read overflow, writing speed is faster than reading speed: "
										<< "curIndex=" << curIndex << ", m_indexRead=" << Base::m_indexRead.load()
										<< ", m_indexWrite=" << Base::m_indexWrite.load() << ", size=" << Base::size());
								return false;
							}
							++curIndex;
							continue;
						}
						if (data.first > keyEnd)
						{
							break;
						}
						callback(data.first, data.second);
						++nbProcessed;
						++curIndex;
					}
				}

				return true;
			}

			/**
			 * Find the element with the specific key or if not available, the closest to this key.
			 * The index returned is the absolute index that do not depends on the head nor the tail
			 * hance it can be used even if the list is altered. Note that it can become too old
			 * once if the list wraps.
			 *
			 * \note This function has not been teste against race conditions
			 *
			 * \param key The key to identify
			 * \param oldest point to the oldest element that matches
			 *        For example give the following list: 1 2 4 5 5 6, with respectively
			 *        the following absolute indexes:      1 2 3 4 5 6
			 *        find(3, true) = 2
			 *        find(3, false) = 3
			 *        find(5, true) = 4
			 *        find(5, false) = 5
			 *
			 * \return Return the absolute index of the element or the closest match.
			 *         If the buffer empty, returns 0. If too high, returns the index
			 *         of the head element, If too low, returns the index of the last
			 *         valid element.
			 */
			size_t find(const K& key, const bool oldest = true) const noexcept
			{
				size_t index = 0;
				{
					size_t indexEnd = Base::size();
					while (indexEnd > index && indexEnd - index > 1)
					{
						const auto indexMiddle = (index + indexEnd) / 2;
						const K keyMiddle = Base::head(indexMiddle).first;
						if (key >= keyMiddle)
						{
							indexEnd = indexMiddle;
						}
						else
						{
							index = indexMiddle;
						}
					}
					IRSTD_ASSERT(IRSTD_TOPIC(IrStd, Type), indexEnd >= index,
							"it seems that elements inside the buffer are not sorted: index="
							<< index << ", indexEnd=" << indexEnd);
				}

				// The list is empty
				if (Base::size() == 0)
				{
					return 0;
				}
				// If the element is too new
				if (index == 0 && Base::head(0).first < key)
				{
					return Base::getIndex();
				}

				// At this point index, is exclusive and points to the far right of the element (higher)
				auto absoluteIndex = Base::m_indexRead.load() - index;
				{
					if (oldest)
					{
						auto olderAbsoluteIndex = absoluteIndex;
						while (!Base::isIndexTooOld(--olderAbsoluteIndex) && Base::loadForRead(olderAbsoluteIndex).first >= key)
						{
							absoluteIndex = olderAbsoluteIndex;
						}
						return (Base::loadForRead(absoluteIndex).first > key && !Base::isIndexTooOld(olderAbsoluteIndex)) ?
								olderAbsoluteIndex : absoluteIndex;
					}
					else
					{
						auto olderAbsoluteIndex = absoluteIndex;
						while (!Base::isIndexTooOld(--olderAbsoluteIndex) && Base::loadForRead(olderAbsoluteIndex).first > key)
						{
							absoluteIndex = olderAbsoluteIndex;
						}
						return (Base::loadForRead(absoluteIndex).first > key && Base::loadForRead(olderAbsoluteIndex).first == key && !Base::isIndexTooOld(olderAbsoluteIndex)) ?
								olderAbsoluteIndex : absoluteIndex;
					}
				}
			}

			/**
			 * \copydoc Base::read
			 */
			bool read(const std::function<void(const K&, const T&)>& callback, const size_t nbEntries = N) const noexcept
			{
				return Base::read([&callback](const std::pair<K, T>& entry) {
					callback(entry.first, entry.second);
				}, nbEntries);
			}

			/**
			 * \copydoc Base::readInterval
			 */
			bool readInterval(
					const size_t indexBegin,
					const size_t indexEnd,
					const std::function<void(const K&, const T&)>& callback) const noexcept
			{
				return Base::readInterval(indexBegin, indexEnd, [&callback](const std::pair<K, T>& entry) {
					callback(entry.first, entry.second);
				});
			}
		};
	}
}
