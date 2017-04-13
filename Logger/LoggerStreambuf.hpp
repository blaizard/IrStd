#pragma once

#include <iostream>

#include <streambuf>
#include <vector>
#include <string>

#include "../Logger.hpp"

namespace IrStd
{
	void loggerGetLock();
	void loggerReleaseLock();

	template <typename char_type, typename traits = std::char_traits<char_type> >
	class BasicLoggerStreambuf: public std::basic_streambuf<char_type, traits>
	{
	public:
		typedef std::basic_streambuf<char_type, traits> Streambuf;
		typedef std::pair<Streambuf*, const IrStd::Logger::Format*> StreamDesc;

		explicit BasicLoggerStreambuf(const std::vector<StreamDesc>& streamDescList)
				: m_streamDescList(streamDescList)
		{
		}

		/**
		 * Called once a new entry needs to be written
		 */
		void start(const IrStd::Logger::Info& info)
		{
			loggerGetLock();
		}
		void end()
		{
			loggerReleaseLock();
		}
	protected:
		typedef typename traits::int_type int_type;

		virtual int_type overflow(int_type c)
		{
			if (traits::eq_int_type(c, traits::eof())) {
				return traits::not_eof(c);
			}

			bool isEof = false;
			const char_type ch = traits::to_char_type(c);
			// Write to all the registered stream buffers
			for (auto& streamDesc : m_streamDescList) {
				const int_type rc = streamDesc.first->sputc(ch);
				isEof |= traits::eq_int_type(rc, traits::eof());
			}
			return (isEof) ? traits::eof() : c;
		}

		int sync()
		{
			bool isError = false;
			// Sync all the registered stream buffers
			for (auto& streamDesc : m_streamDescList) {
				const int rc = streamDesc.first->pubsync();
				isError |= (rc != 0);
			}

			return (isError) ? -1 : 0;
		}

		const std::vector<StreamDesc> m_streamDescList;
	};

	typedef BasicLoggerStreambuf<char> LoggerStreambuf;
//	std::string m_buffer;

}
