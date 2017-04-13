#pragma once

#include <memory>
#include <vector>
#include <ctime>
#include <sys/time.h>
#include <string.h>
#include <set>
#include <sstream>

#include "Utils.hpp"
#include "Topic.hpp"
#include "Allocator.hpp"

/**
 * Logger
 * Usage:
 * Logger log();
 * log.addStream(...);
 * log.entry(LEVEL) << "";
 */

IRSTD_TOPIC_USE(None);

#define IRSTD_LOG_TRACE(...) IRSTD_GET_MACRO(_IRSTD_LOG, __VA_ARGS__)(IrStd::Logger::Level::Trace, __VA_ARGS__)
#define IRSTD_LOG_DEBUG(...) IRSTD_GET_MACRO(_IRSTD_LOG, __VA_ARGS__)(IrStd::Logger::Level::Debug, __VA_ARGS__)
#define IRSTD_LOG_INFO(...) IRSTD_GET_MACRO(_IRSTD_LOG, __VA_ARGS__)(IrStd::Logger::Level::Info, __VA_ARGS__)
#define IRSTD_LOG_WARNING(...) IRSTD_GET_MACRO(_IRSTD_LOG, __VA_ARGS__)(IrStd::Logger::Level::Warning, __VA_ARGS__)
#define IRSTD_LOG_ERROR(...) IRSTD_GET_MACRO(_IRSTD_LOG, __VA_ARGS__)(IrStd::Logger::Level::Error, __VA_ARGS__)
#define IRSTD_LOG_FATAL(...) IRSTD_GET_MACRO(_IRSTD_LOG, __VA_ARGS__)(IrStd::Logger::Level::Fatal, __VA_ARGS__)

#define _IRSTD_LOG1(level, message) _IRSTD_LOG2(level, None, message)
#define _IRSTD_LOG2(level, topic, message) IRSTD_LOG(level, topic, message)

#define IRSTD_LOG(level, topic, message) { \
		IrStd::Logger& log = IrStd::Logger::getDefault(); \
		timeval t; \
		gettimeofday(&t, 0); \
		*log.entry({level, IrStd::Topic::topic, __LINE__, (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__), __func__, t}) << message; \
	}

#define IRSTD_LOGGER_DEFINE_LEVEL(verbosity, id) (static_cast<uint32_t>(verbosity << 16) | static_cast<uint16_t>(id))

namespace IrStd
{
	class Logger : public AllocatorImpl<AllocatorRaw>
	{
	public:
		~Logger();

		/**
		 * This is used for the default logger.
		 * This logger is always present and is used by all
		 * the macros LOG_xxx
		 */
		static Logger& getDefault()
		{
			static Logger instance{std::cout};
			return instance;
		}	

		enum class Level : uint32_t
		{
			/**
			 * Designates finer-grained informational events than the DEBUG.
			 */
			Trace = IRSTD_LOGGER_DEFINE_LEVEL(0x0001, 't'),
			/**
			 * Designates fine-grained informational events that are most useful to debug an application.
			 */
			Debug = IRSTD_LOGGER_DEFINE_LEVEL(0x0002, 'd'),
			/**
			 * Designates informational messages that highlight the progress of the application at coarse-grained level.
			 */
			Info = IRSTD_LOGGER_DEFINE_LEVEL(0x0004, 'i'),
			/**
			 * Designates potentially harmful situations.
			 */
			Warning = IRSTD_LOGGER_DEFINE_LEVEL(0x0008, 'w'),
			/**
			 * Designates error events that might still allow the application to continue running.
			 */
			Error = IRSTD_LOGGER_DEFINE_LEVEL(0x0010, 'e'),
			/**
			 * Designates very severe error events that will presumably lead the application to abort.
			 */
			Fatal = IRSTD_LOGGER_DEFINE_LEVEL(0x0020, 'f')
		};

		struct Info
		{
			const Level level;
			const TopicImpl& topic;
			const size_t line;
			const char* const file;
			const char* const func;
			const timeval time;
		};

		/**
		 * \brief Stream formater
		 */
		class Format
		{
		public:
			virtual void header(std::ostream& out, const Info& info) const = 0;
			virtual void tail(std::ostream& out) const = 0;
		};

		class FormatRaw : public Format, public SingletonImpl<FormatRaw>
		{
		public:
			void header(std::ostream& out, const Info& info) const;
			void tail(std::ostream& out) const;
		};

		class FormatDefault : public Format, public SingletonImpl<FormatDefault>
		{
		public:
			void header(std::ostream& out, const Info& info) const;
			void tail(std::ostream& out) const;
		};

		/**
		 * \brief Stream filter
		 */
		class Filter
		{
		public:
			Filter(const Level minLevel = Level::Trace)
				: m_minLevel(minLevel)
			{
			}
		private:
			const Level m_minLevel;
			const std::set<const TopicImpl*> m_onlyTopics;
		};

		/**
		 * \brief Data structure that defines a stream
		 */
		struct Stream
		{
			/**
			 * Constructors
			 */
			Stream(std::ostream& os)
					: Stream(os, &FormatDefault::getInstance(), Filter())
			{
			}
			Stream(std::ostream& os, const Format* const pFormat)
					: Stream(os, pFormat, Filter())
			{
			}
			Stream(std::ostream& os, const Filter& filter)
					: Stream(os, &FormatDefault::getInstance(), filter)
			{
			}
			Stream(std::ostream& os, const Format* const pFormat, const Filter& filter)
					: m_os(os), m_pFormat(pFormat), m_filter(filter)
			{
			}

			/**
			 * Copy constructors
			 */
			Stream(const Stream& stream)
					: m_os(stream.m_os), m_pFormat(stream.m_pFormat), m_filter(stream.m_filter)
			{
			}

			std::ostream& m_os;
			const Format* const m_pFormat;
			const Filter m_filter;
		};

		typedef std::vector<Stream, AllocatorObj<Stream, AllocatorRaw>> StreamList;

		StreamList m_streamList;

		class OutputStream : public AllocatorImpl<AllocatorRaw>
		{
		public:
			OutputStream() = delete;
			/**
			 * 
			 */
			explicit OutputStream(const OutputStream& os) = default;
			OutputStream(const StreamList& streamList, const Info& info);

			/**
			 * Move constructor
			 */
			OutputStream(const OutputStream&& os);

			/**
			 * Used to unlock the mutex
			 */
			~OutputStream();

			template<typename T>
			OutputStream& operator<<(const T& in)
			{
				for (auto& stream : m_streamList)
				{
					m_bufferStream << in;
				}
				return *this;
			}

		private:
			const StreamList& m_streamList;
			const Info m_info;

			std::string m_buffer;
			std::stringstream m_bufferStream;
		};

		typedef std::unique_ptr<OutputStream, std::function<void(OutputStream*)>> OutputStreamPtr;

		/**
		 * Constructors
		 */
		Logger()
		{
		}
		explicit Logger(const Stream& stream)
		{
			m_streamList.push_back(stream);
		}

		/**
		 * Copy constructors
		 */
		Logger(const Logger& logger)
		{
			*this = logger;
		}
		void operator=(Logger const& logger)
		{
			m_streamList.empty();
			for (const auto& stream : logger.m_streamList)
			{
				m_streamList.push_back(Stream{stream});
			}
		}

		OutputStreamPtr entry(const Info& info);
		OutputStreamPtr entry();
	};
}
