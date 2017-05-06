#pragma once

#include "Utils.hpp"

#define IRSTD_TOPIC_REGISTER(...) IRSTD_GET_MACRO(_IRSTD_TOPIC_REGISTER, __VA_ARGS__)(__VA_ARGS__)
#define _IRSTD_TOPIC_REGISTER1(topic) _IRSTD_TOPIC_REGISTER2(topic, IRSTD_QUOTE(topic))
#define _IRSTD_TOPIC_REGISTER2(topic, str) \
	namespace IrStd \
	{ \
		namespace Topic \
		{ \
			extern const IrStd::TopicImpl topic(str); \
		} \
	}

#define IRSTD_TOPIC_USE(topic) \
	namespace IrStd \
	{ \
		namespace Topic \
		{ \
			extern const IrStd::TopicImpl topic; \
		} \
	}

#define IRSTD_TOPIC(topic) IrStd::Topic::topic

namespace IrStd
{
	namespace Topic
	{
	}

	class TopicImpl
	{
	public:
		typedef std::uintptr_t Ref;

		TopicImpl(const char* const str)
			: m_str(str)
		{
		}

		/**
		 * Returns a unique reference of the object
		 */
		Ref getRef() const noexcept
		{
			return reinterpret_cast<Ref>(this);
		}

		const char* getStr() const noexcept
		{
			return m_str;
		}

	private:
		const char* const m_str;
	};
}

bool operator==(const IrStd::TopicImpl& lhs, const IrStd::TopicImpl& rhs);
bool operator!=(const IrStd::TopicImpl& lhs, const IrStd::TopicImpl& rhs);
