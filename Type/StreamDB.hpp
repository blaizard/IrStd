/**
 * Stream DataBase or StreamDB is a flow of data that is always writen with the oldest first.
 * It is made to handle a large amoutn of data.
 */
#pragma once

#include "RingBufferSorted.hpp"
#include "../FileSystem.hpp"

namespace IrStd
{
	namespace Type
	{
		template<class T>
		struct StreamDBEntryContextType
		{
			T compute(const T& value) noexcept
			{
				return value;
			}
		};

		template<class T>
		struct StreamDBEntryContextTypeNumeric
		{
		public:
			T compute(const T& value) noexcept
			{
				m_sum += value;
				m_min = std::min(m_min, value);
				m_max = std::max(m_max, value);

				return m_sum;
			}

			T getMin() const noexcept
			{
				return m_min;
			}

			T getMax() const noexcept
			{
				return m_max;
			}

		private:
			// Doesn't matter if it wraps
			T m_sum = 0;
			T m_min = std::numeric_limits<int>::max();
			T m_max = std::numeric_limits<int>::min();
		};

		template<>
		struct StreamDBEntryContextType<int> : StreamDBEntryContextTypeNumeric<int>
		{
		};

		template <class ... Types>
		class StreamDBEntryContext
		{
		public:
			template<size_t Pos>
			using type = typename std::tuple_element<Pos, std::tuple<Types...>>::type;

			template<size_t Pos, class T>
			void set(const T& value) noexcept
			{
				std::get<Pos>(m_args) = value;
			}

			template<size_t Pos>
			type<Pos> get() const noexcept
			{
				return std::get<Pos>(m_args);
			}

		private:
			std::tuple<StreamDBEntryContextType<Types>...> m_args;
		};

		template<class ... Types>
		class StreamDBEntry
		{
		public:
			typedef std::tuple<StreamDBEntryContextType<Types>...> Context;

			template<size_t Pos>
			using type = typename std::tuple_element<Pos, std::tuple<Types...>>::type;

			template<size_t Pos>
			using typeContext = typename std::tuple_element<Pos, Context>::type;

			template<class T>
			StreamDBEntry(const T& /*entry*/, Context& /*context*/)
			{
				IRSTD_UNREACHABLE("Missing specialization");
			}

			template<size_t Pos, class T>
			void set(const T& value, Context& context) noexcept
			{
				std::get<Pos>(m_args) = std::get<Pos>(context).compute(value);
			}

			template<size_t Pos>
			type<Pos> get(Context& /*context*/) const noexcept
			{
				return std::get<Pos>(m_args);
			}

		protected:
			StreamDBEntry() = default;

		private:
			std::tuple<Types...> m_args;
		};

		template<class Entry, class EntryCache, size_t NB_DATA = 256, size_t CACHE = 1024 * 1024>
		class StreamDB
		{
		private:
			static constexpr size_t NB_CACHE_ENTRIES = CACHE / sizeof(EntryCache) + 1;

		public:
			StreamDB(const std::string path)
					: m_csv(path)
					, m_bufferIndexNotFlushed(0)
			{
				// Fill the cache with current data
				fillCache();
			}

			~StreamDB()
			{
				// Flush remaining data
				flush();
			}

			/**
			 * Push new data to the stream
			 */
			template<class ... Args>
			void push(const IrStd::Type::Timestamp timestamp, Args&& ... args)
			{
				const Entry entry(std::forward<Args>(args)...);
				m_buffer.push(timestamp, entry);

				// Push it to the cache if in sync
				if (isCacheInSync())
				{
					pushToCache(timestamp, entry);
				}
			}

			/**
			 * Flush data to the persistent device
			 */
			void flush()
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				size_t lastIndex = m_buffer.getIndex();
				m_buffer.readInterval(m_bufferIndexNotFlushed, lastIndex,
					[&](const IrStd::Type::Timestamp& timestamp, const Entry& entry) {
					Entry::write(m_csv, timestamp, entry);
				});
				m_bufferIndexNotFlushed = lastIndex + 1;
			}

			/**
			 * \brief Access an element from the cache
			 */
			template<size_t Pos>
			typename EntryCache::template typeContext<Pos> get() const noexcept
			{
				return std::get<Pos>(m_cache.m_context);
			}

		private:
			void pushToCache(const IrStd::Type::Timestamp timestamp, const Entry& entry) noexcept
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				const EntryCache cache(entry, m_cache.m_context);
				m_cache.m_buffer.push(timestamp, cache);
			}

			bool isCacheInSync() const noexcept
			{
				return true;
			}

			void fillCache() noexcept
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				// Clear the buffer and reserve all the entries
				for (size_t i = 0; i<NB_CACHE_ENTRIES; ++i)
				{
					m_cache.m_buffer.push(0, EntryCache());
				}

				// Update the content
				size_t nbData = 0;
				{
					size_t index = m_cache.m_buffer.getIndex();
					std::string entryStr;
					IrStd::Type::Timestamp timestamp;
					m_csv.seekEnd();
					while (index > 0 && m_csv.read(entryStr))
					{
						const EntryCache cache(entryStr, timestamp, m_cache.m_context);
						m_cache.m_buffer.loadForWrite(index--) = std::make_pair(timestamp, cache);
					}
					nbData = m_cache.m_buffer.getIndex() - index - 1;
				}

				// Print the shit
				{
					std::cout << " ----------------- " << std::endl;
					m_cache.m_buffer.read([&](const IrStd::Type::Timestamp& timestamp, const EntryCache& cache) {
						std::cout << timestamp << "  " << cache.get<0>(m_cache.m_context) << "  " << cache.get<1>(m_cache.m_context) << std::endl;
					}, nbData + 1);
				}
			}

			std::mutex m_mutex;
			IrStd::FileSystem::FileCsv m_csv;
			size_t m_bufferIndexNotFlushed;
			IrStd::Type::RingBufferSorted<IrStd::Type::Timestamp, Entry, NB_DATA> m_buffer;

			// Cache related information
			class Cache
			{
			public:
				IrStd::Type::RingBufferSorted<IrStd::Type::Timestamp, EntryCache, NB_CACHE_ENTRIES> m_buffer;
				typename EntryCache::Context m_context;
			};
			Cache m_cache;
		};
	}
}
