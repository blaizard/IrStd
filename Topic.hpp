#pragma once

#include <vector>

#include "Utils.hpp"

#define IRSTD_TOPIC_REGISTER(...) IRSTD_GET_MACRO(_IRSTD_TOPIC_REGISTER, __VA_ARGS__)(__VA_ARGS__)

#define _IRSTD_TOPIC_REGISTER1(topic) \
		_IRSTD_TOPIC_REGISTER_WRAPPER(topic, IRSTD_QUOTE(topic))
#define _IRSTD_TOPIC_REGISTER2(parentTopic1, topic) \
		IRSTD_TOPIC_USE(parentTopic1); \
		_IRSTD_TOPIC_REGISTER_WRAPPER(_IRSTD_TOPIC(parentTopic1, topic), \
				IRSTD_QUOTE(parentTopic1) "::" IRSTD_QUOTE(topic), \
				parentTopic1)
#define _IRSTD_TOPIC_REGISTER3(parentTopic1, parentTopic2, topic) \
		IRSTD_TOPIC_USE(parentTopic1, parentTopic2); \
		_IRSTD_TOPIC_REGISTER_WRAPPER(_IRSTD_TOPIC(parentTopic1, parentTopic2, topic), \
				IRSTD_QUOTE(parentTopic1) "::" IRSTD_QUOTE(parentTopic2) "::" IRSTD_QUOTE(topic), \
				_IRSTD_TOPIC(parentTopic1, parentTopic2))

#define _IRSTD_TOPIC_REGISTER_WRAPPER(topic, ...) \
	namespace IrStd \
	{ \
		namespace Topic \
		{ \
			extern const IrStd::TopicImpl topic(__VA_ARGS__); \
		} \
	}

#define IRSTD_TOPIC_USE(...) \
	namespace IrStd \
	{ \
		namespace Topic \
		{ \
			extern const IrStd::TopicImpl _IRSTD_TOPIC(__VA_ARGS__); \
		} \
	}

#define IRSTD_TOPIC_USE_ALIAS(alias, ...) \
	IRSTD_TOPIC_USE(__VA_ARGS__); \
	namespace \
	{ \
		const auto& alias = IRSTD_TOPIC(__VA_ARGS__); \
	}

#define IRSTD_TOPIC(...) IrStd::Topic::_IRSTD_TOPIC(__VA_ARGS__)

#define _IRSTD_TOPIC(...) IRSTD_GET_MACRO(_IRSTD_TOPIC, __VA_ARGS__)(__VA_ARGS__)
#define _IRSTD_TOPIC1(topic1) topic1
#define _IRSTD_TOPIC2(topic1, topic2) IRSTD_PASTE(topic1, _, topic2)
#define _IRSTD_TOPIC3(topic1, topic2, topic3) IRSTD_PASTE(topic1, _, topic2, _, topic3)

namespace IrStd
{
	namespace Topic
	{
	}

	class TopicImpl
	{
	public:
		typedef std::uintptr_t Ref;

		TopicImpl(const char* const str);
		TopicImpl(const char* const str, const TopicImpl& parent);

		/**
		 * Returns a unique reference of the object
		 */
		Ref getRef() const noexcept;

		const char* getStr() const noexcept;

		const TopicImpl* getParent() const noexcept;

		void toStream(std::ostream& os) const;

		static void getList(std::function<void(const TopicImpl&)>) noexcept;

	private:
		const char* const m_str;
		const TopicImpl* m_pParent;

		static std::vector<TopicImpl*> m_topicList;
	};
}

bool operator==(const IrStd::TopicImpl& lhs, const IrStd::TopicImpl& rhs);
bool operator!=(const IrStd::TopicImpl& lhs, const IrStd::TopicImpl& rhs);
std::ostream& operator<<(std::ostream& os, const IrStd::TopicImpl topic);
