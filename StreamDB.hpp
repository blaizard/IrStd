/**
 * Stream DataBase or StreamDB is a flow of data that is always writen with the oldest first.
 * It is made to handle a large amoutn of data.
 */
#pragma once

#include "Type/RingBufferSorted.hpp"

namespace IrStd
{
	template<class Entry, size_t NB_DATA = 256, size_t CACHE = 1024 * 1024>
	class StreamDB
	{
	public:
		~StreamDB()
		{
			// Flush remaining data
			flush();
		}

		/**
		 * Push new data to the stream
		 */
		void push(IrStd::Type::Timestamp, const Entry& entry);

		/**
		 * Flush data to the persistent device
		 */
		void flush();

	private:
		IrStd::Type::RingBufferSorted<IrStd::Type::Timestamp, Entry, NB_DATA> m_currentData;
		IrStd::Type::RingBufferSorted<IrStd::Type::Timestamp, Entry, (CACHE / sizeof(Entry) + 1)> m_cache;
	};
}
