#pragma once

#include <exception>
#include <sstream>

#include "Topic.hpp"
#include "Utils.hpp"
#include "Assert.hpp"

#define IRSTD_THROW(...) IRSTD_GET_MACRO(_IRSTD_THROW, __VA_ARGS__)(__VA_ARGS__)

#define IRSTD_THROW_RETRY(...) IRSTD_GET_MACRO(_IRSTD_THROW, true, __VA_ARGS__)(true, __VA_ARGS__)

#define _IRSTD_THROW1(message) \
	{ \
		IrStd::Exception irstdException(__LINE__, (strrchr(__FILE__, '/') ? \
				strrchr(__FILE__, '/') + 1 : __FILE__), __func__); \
		irstdException << message; \
		std::throw_with_nested(std::move(irstdException)); \
	}
#define _IRSTD_THROW2(a1, message) __IRSTD_THROW(message, a1)
#define _IRSTD_THROW3(a1, a2, message) __IRSTD_THROW(message, a1, a2)

#define __IRSTD_THROW(message, ...) \
	{ \
		IrStd::Exception irstdException(__LINE__, (strrchr(__FILE__, '/') ? \
				strrchr(__FILE__, '/') + 1 : __FILE__), __func__, __VA_ARGS__); \
		irstdException << message; \
		std::throw_with_nested(std::move(irstdException)); \
	}

#define IRSTD_HANDLE_RETRY(code, counter) \
	for (auto _retryCounter = 1; _retryCounter <= (counter) + 1; _retryCounter++) \
	{ \
		try \
		{ \
			{ code; } \
			break; \
		} \
		catch (const IrStd::Exception& e) \
		{ \
			if (!e.isAllowRetry() || _retryCounter == (counter) + 1) \
			{ \
				IrStd::Exception::rethrow(); \
			} \
			IRSTD_LOG_TRACE("Exception: " << e.what() << ", retrying... (" \
					<< _retryCounter << " out of " #counter ")"); \
			continue; \
		} \
	}

IRSTD_TOPIC_USE(None);

namespace IrStd
{
	class ExceptionPtr;
	class Exception : public std::exception
	{
	public:
		Exception(const size_t line, const char* const file, const char* const func, const IrStd::TopicImpl& topic = IrStd::Topic::None);
		Exception(const size_t line, const char* const file, const char* const func, const bool allowRetry, const IrStd::TopicImpl& topic = IrStd::Topic::None);

		/**
		 * Move constructor
		 */
		Exception(const Exception&& e);

		/**
		 * \brief Returns an explanatory string 
		 */
		const char* what() const noexcept;

		/**
		 * \brief Print some tracing information, useful for debug
		 */
		std::string trace() const;

		/**
		 * Get the exception topic
		 */
		const IrStd::TopicImpl& getTopic() const noexcept;

		/**
		 * Mark the current exception to allow retries
		 */
		void allowRetry(const bool retry = true) noexcept;

		/**
		 * Check if the exception is allowing retries
		 */
		bool isAllowRetry() const noexcept;

		/**
		 * Get the next chained exception (if any).
		 * Returns nullptr if none.
		 */
		ExceptionPtr getNext() const;
		static ExceptionPtr getNext(const std::exception* pE);

		/**
		 * \brief Stream operator to convienently append a message to the exception
		 */
		template<typename T>
		Exception& operator<<(const T& in)
		{
			m_stream << in;
			return *this;
		}

		static void callStack(std::ostream& out, const size_t skipFirstNb = 1) noexcept;

		/**
		 * \brief Print the exception or the chainned exception
		 */
		static void print(std::ostream& out, std::exception_ptr pE = nullptr) noexcept;

		/**
		 * \brief Rethrow the current exception(s) and wrap it if needed
		 */
		static void rethrow();

		/**
		 * \brief Rethrow the current exception(s) and apply the allowretry flag to it
		 */
		static void rethrowRetry();

	private:
		static void demangle(std::ostream& out, const char* const symbol);

		const size_t m_line;
		const char* const m_file;
		const char* const m_func;
		const IrStd::TopicImpl& m_topic;
		std::string m_message;
		IrStd::StringStream m_stream;
		bool m_allowRetry;
	};

	class ExceptionPtr
	{
	public:
		ExceptionPtr();
		ExceptionPtr(const IrStd::Exception* pE);
		ExceptionPtr(const std::exception* pE);

		static IrStd::ExceptionPtr fromExceptionPtr(const std::exception_ptr ptr);

		bool isIrStdException() const noexcept;
		const IrStd::Exception& getIrStdException() const;
		bool isException() const noexcept;
		const std::exception& getException() const;
		ExceptionPtr getNext() const;

		bool operator==(const ExceptionPtr& pE) const;
		bool operator==(const IrStd::Exception* pE) const;
		bool operator==(const std::exception* pE) const;
		bool operator==(std::nullptr_t) const;

		bool operator!() const;

		operator int() const;

		const std::exception& operator*() const;
		const std::exception* operator->() const;

	private:
		const IrStd::Exception* const m_pIrstd;
		const std::exception* const m_pStd;
	};
}
