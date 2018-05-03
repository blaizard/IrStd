#include "../Topic.hpp"

// ---- IrStd::TopicImpl ------------------------------------------------------

IrStd::TopicImpl::TopicImpl(const char* const str)
		: m_str(str)
		, m_pParent(nullptr)
{
	m_topicList.push_back(this);
}

IrStd::TopicImpl::TopicImpl(const char* const str, const TopicImpl& parent)
		: m_str(str)
		, m_pParent(&parent)
{
	m_topicList.push_back(this);
}

IrStd::TopicImpl::Ref IrStd::TopicImpl::getRef() const noexcept
{
	return reinterpret_cast<Ref>(this);
}

const char* IrStd::TopicImpl::getStr() const noexcept
{
	return m_str;
}

const IrStd::TopicImpl* IrStd::TopicImpl::getParent() const noexcept
{
	return m_pParent;
}

void IrStd::TopicImpl::toStream(std::ostream& os) const
{
	os << getStr();
}

bool operator==(const IrStd::TopicImpl& lhs, const IrStd::TopicImpl& rhs)
{
	return lhs.getRef() == rhs.getRef();
}

bool operator!=(const IrStd::TopicImpl& lhs, const IrStd::TopicImpl& rhs)
{
	return lhs.getRef() != rhs.getRef();
}

std::ostream& operator<<(std::ostream& os, const IrStd::TopicImpl topic)
{
	topic.toStream(os);
	return os;
}

// ---- IrStd::TopicImpl (static) ---------------------------------------------

std::vector<IrStd::TopicImpl*> IrStd::TopicImpl::m_topicList;

void IrStd::TopicImpl::getList(std::function<void(const TopicImpl&)> callback) noexcept
{
	for (const auto topic : m_topicList)
	{
		callback(*topic);
	}
}
