#pragma once

#define IRSTD_TOPIC_REGISTER(topic, str) \
	namespace IrStd \
	{ \
		namespace Topic \
		{ \
			const IrStd::TopicImpl topic(str); \
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

namespace IrStd
{
	namespace Topic
	{
	}

	class TopicImpl
	{
	public:
		TopicImpl(const char* const str)
			: m_str(str)
		{
		}

		const char* const getStr() const noexcept
		{
			return m_str;
		}

	private:
		const char* const m_str;
	};
}

