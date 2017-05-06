#include "../Topic.hpp"

bool operator==(const IrStd::TopicImpl& lhs, const IrStd::TopicImpl& rhs)
{
	return lhs.getRef() == rhs.getRef();
}

bool operator!=(const IrStd::TopicImpl& lhs, const IrStd::TopicImpl& rhs)
{
	return lhs.getRef() != rhs.getRef();
}