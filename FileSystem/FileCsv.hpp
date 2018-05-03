#pragma once

#include <string.h>
#include <string>
#include <mutex>
#include <array>

#include "FileStream.hpp"
#include "../Assert.hpp"
#include "../Topic.hpp"

IRSTD_TOPIC_USE(IrStd, Type)

namespace IrStd
{
	namespace FileSystem
	{
		class FileCsv : public FileStream
		{
		private:
			static constexpr char SEPARATOR = ';';
			static constexpr char NEWLINE[] = "\n";

		public:
			FileCsv(const std::string& path, const size_t readBufferSize = 4096)
					: FileStream(path, IrStd::FileMode::APPEND)
					, m_readSeek(0)
					, m_readBufferSize(readBufferSize)
			{
			}

			/**
			 * \brief Write content to the file
			 */
			template<class T>
			void write(const T& arg)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				writeNoLock(arg);
			}
			template<class T, class ... Args>
			void write(const T& firstArg, Args&&... args)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				writeNoLock(firstArg, std::forward<Args>(args)...);
			}

			void seekEnd();

			/**
			 * \brief Read data sequentially
			 */
			bool read(std::string& entry);

		private:
			template<class T>
			void writeNoLock(const T& arg)
			{
				getStream() << arg << SEPARATOR << NEWLINE;
			}
			template<class T, class ... Args>
			void writeNoLock(const T& firstArg, Args&&... args)
			{
				getStream() << firstArg << SEPARATOR;
				writeNoLock(std::forward<Args>(args)...);
			}

			/**
			 * Fetch data and store them into the read buffer
			 */
			void updateReadBufferNoLock();

			std::mutex m_mutex;
			std::string m_readBuffer;
			size_t m_readSeek;
			const size_t m_readBufferSize;
		};
	}
}
