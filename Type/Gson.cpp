#include "Gson.hpp"

// ---- IrStd::Type::Gson (constructors) --------------------------------------

IrStd::Type::Gson::Gson()
		: m_type(Type::ARRAY)
		, m_array(new std::vector<Gson>())
{
}

IrStd::Type::Gson::Gson(const bool boolean)
		: m_type(Type::BOOL)
		, m_bool(boolean)
{
}

IrStd::Type::Gson::Gson(const std::string& string)
		: m_type(Type::STRING)
		, m_string(string)
{
}

IrStd::Type::Gson::Gson(const char* const pStr)
		: m_type(Type::STRING)
		, m_string(pStr)
{
}

IrStd::Type::Gson::Gson(const std::initializer_list<Gson>& array)
		: m_type(Type::ARRAY)
		, m_array(new std::vector<Gson>(array.size()))
{
	std::copy(array.begin(), array.end(), m_array->begin());
}


IrStd::Type::Gson::Gson(const Map& map)
		: m_type(Type::MAP)
		, m_map(new Map(map))
{
}

// ---- IrStd::Type::Gson (copy constructors) ---------------------------------


IrStd::Type::Gson::Gson(const Gson& node)
{
	*this = node;
}

IrStd::Type::Gson& IrStd::Type::Gson::operator=(const Gson& node)
{
	m_type = node.m_type;
	switch (m_type)
	{
	case Type::EMPTY:
		break;
	case Type::BOOL:
		m_bool = node.m_bool;
		break;
	case Type::NUMBER:
		m_number = node.m_number;
		break;
	case Type::STRING:
		m_string = node.m_string;
		break;
	case Type::ARRAY:
		m_array = std::unique_ptr<std::vector<Gson>>(new std::vector<Gson>(node.m_array->size()));
		std::copy(node.m_array->begin(), node.m_array->end(), m_array->begin());
		break;
	case Type::MAP:
		m_map = std::unique_ptr<Map>(new Map(*node.m_map));
		break;
	default:
		IRSTD_UNREACHABLE();
	}
	return *this;
}

// ---- IrStd::Type::Gson (merge) ---------------------------------------------

void IrStd::Type::Gson::merge(const Gson& node)
{
	IRSTD_ASSERT(m_type == node.m_type);

	switch (node.m_type)
	{
	case Type::EMPTY:
		break;
	case Type::BOOL:
		m_bool = node.m_bool;
		break;
	case Type::NUMBER:
		m_number = node.m_number;
		break;
	case Type::STRING:
		m_string = node.m_string;
		break;
	case Type::ARRAY:
		for (const auto& element : *node.m_array)
		{
			m_array->push_back(element);
		}
		break;
	case Type::MAP:
		{
			for (const auto& element : node.m_map->m_map)
			{
				auto it = m_map->m_map.find(element.first);
				if (it == m_map->m_map.end())
				{
					m_map->m_map.insert(element);
				}
				else
				{
					it->second.merge(element.second);
				}
			}
		}
		break;

	default:
		IRSTD_UNREACHABLE();
	}
}

// ---- IrStd::Type::Gson (access) --------------------------------------------

const IrStd::Type::Gson& IrStd::Type::Gson::get(const std::initializer_list<std::string> keyList) const noexcept
{
	const auto* pNode = this;
	for (const auto& key : keyList)
	{
		IRSTD_ASSERT(pNode->m_type == Type::MAP, "Node of type "
				<< static_cast<int>(pNode->m_type));
		const auto it = pNode->m_map->m_map.find(key);
		IRSTD_ASSERT(it != pNode->m_map->m_map.end(), "Key \"" << key
				<< "\" is not present in " << *pNode);
		pNode = &it->second;
	}
	return *pNode;
}

// ---- IrStd::Type::Gson (print) ---------------------------------------------

void IrStd::Type::Gson::toStream(std::ostream& os) const
{
	switch (m_type)
	{
	case Type::EMPTY:
		os << "null";
		break;
	case Type::BOOL:
		os << ((m_bool) ? "true" : "false");
		break;
	case Type::NUMBER:
		os << m_number;
		break;
	case Type::STRING:
		os << "\"" << m_string << "\"";
		break;
	case Type::ARRAY:
		os << "[";
		for (size_t i=0; i<m_array->size(); i++)
		{
			if (i > 0)
			{
				os << ", ";
			}
			m_array->at(i).toStream(os);
		}
		os << "]";
		break;
	case Type::MAP:
		{
			os << "{";
			bool isFirst = true;
			for (const auto& element : m_map->m_map)
			{
				if (isFirst)
				{
					isFirst = false;
				}
				else
				{
					os << ", ";
				}
				os << "\"" << element.first << "\": ";
				element.second.toStream(os);
			}
			os << "}";
		}
		break;

	default:
		IRSTD_UNREACHABLE();
	}
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::Gson& on)
{
	on.toStream(os);
	return os;
}

// ---- IrStd::Type::Gson::Map ------------------------------------------------

IrStd::Type::Gson::Map::Map(const std::initializer_list<std::pair<std::string, Gson>> map)
{
	for (const auto& element : map)
	{
		m_map.insert(element);
	}
}
