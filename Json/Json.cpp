#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#include "../Json.hpp"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wstrict-overflow"	
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/error/en.h"
#include "../rapidjson/ostreamwrapper.h"
#include "../rapidjson/prettywriter.h"
#pragma GCC diagnostic pop

// Workaroudn for a strange bug in double rapidjson::internal::StrtodNormalPrecision(double, int)
#pragma GCC diagnostic ignored "-Wstrict-overflow"	

IRSTD_TOPIC_REGISTER(IrStd, Json);
IRSTD_TOPIC_USE_ALIAS(IrStdJson, IrStd, Json);

// ---- IrStd::Json (constructor) ---------------------------------------------

IrStd::Json::Json()
		: m_document(rapidjson::Type::kObjectType)
{
}

IrStd::Json::Json(const char* const str)
{
	rapidjson::ParseResult result = m_document.Parse(str);
	if (!result)
	{
		IRSTD_THROW(IrStdJson, "JSON parse error: "
				<< rapidjson::GetParseError_En(result.Code())
				<< " (offset=" << result.Offset() << "), dump=" << str);
	}
}

IrStd::Json::Json(const IrStd::Type::Gson::Map& map)
		: Json()
{
	IrStd::Type::Gson gson(map);
	for (const auto& element : gson.getMap())
	{
		add(element.first.c_str(), element.second);
	}
}

IrStd::Json::Json(Json&& json)
{
	m_document.Swap(json.m_document);
}

// ---- IrStd::Json -----------------------------------------------------------

void IrStd::Json::toStream(std::ostream& out) const
{
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	m_document.Accept(writer);
	out << buffer.GetString() << std::endl;
}

std::string IrStd::Json::serialize() const
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	m_document.Accept(writer);

	return std::string(buffer.GetString());
}

void IrStd::Json::clear()
{
	rapidjson::Document(rapidjson::kObjectType).Swap(m_document);
}

rapidjson::Value* IrStd::Json::checkAndGet(std::initializer_list<const char* const>&& attrList) noexcept
{
	// Navigate to the element
	rapidjson::Value* pElt = const_cast<rapidjson::Value*>(static_cast<const rapidjson::Value*>(&m_document));
	for (const char* const attr : attrList)
	{
		if (!pElt->IsObject() || !pElt->HasMember(attr))
		{
			return nullptr;
		}
		pElt = &(*pElt)[attr];
	}
	return pElt;
}

const rapidjson::Value* IrStd::Json::checkAndGet(std::initializer_list<const char* const>&& attrList) const noexcept
{
	// Navigate to the element
	const rapidjson::Value* pElt = static_cast<const rapidjson::Value*>(&m_document);
	for (const char* const attr : attrList)
	{
		if (!pElt->IsObject() || !pElt->HasMember(attr))
		{
			return nullptr;
		}
		pElt = &(*pElt)[attr];
	}
	return pElt;
}

rapidjson::Value* IrStd::Json::checkAndGet(rapidjson::Value* pElt, std::initializer_list<const char* const>&& attrList) noexcept
{
	for (const char* const attr : attrList)
	{
		if (!pElt->IsObject() || !pElt->HasMember(attr))
		{
			return nullptr;
		}
		pElt = &(*pElt)[attr];
	}
	return pElt;
}

rapidjson::Value* IrStd::Json::checkAndGet(rapidjson::Value* pElt, const size_t index) noexcept
{
	return checkAndGet(pElt, static_cast<int>(index));
}

rapidjson::Value* IrStd::Json::checkAndGet(rapidjson::Value* pElt, const int index) noexcept
{
	const auto formatedIndex = static_cast<rapidjson::SizeType>(index);
	if (!pElt->IsArray() || formatedIndex >= pElt->Size())
	{
		return nullptr;
	}
	pElt = &(*pElt)[formatedIndex];
	return pElt;
}

const rapidjson::Value* IrStd::Json::checkAndGet(const rapidjson::Value* pElt, std::initializer_list<const char* const>&& attrList) noexcept
{
	for (const char* const attr : attrList)
	{
		if (!pElt->IsObject() || !pElt->HasMember(attr))
		{
			return nullptr;
		}
		pElt = &(*pElt)[attr];
	}
	return pElt;
}

const rapidjson::Value* IrStd::Json::checkAndGet(const rapidjson::Value* pElt, const size_t index) noexcept
{
	return checkAndGet(pElt, static_cast<int>(index));
}

const rapidjson::Value* IrStd::Json::checkAndGet(const rapidjson::Value* pElt, const int index) noexcept
{
	const auto formatedIndex = static_cast<rapidjson::SizeType>(index);
	if (!pElt->IsArray() || formatedIndex >= pElt->Size())
	{
		return nullptr;
	}
	pElt = &(*pElt)[formatedIndex];
	return pElt;
}

void IrStd::Json::merge(const Json& json)
{
	// Navigate to the element and merge them
	rapidjson::Value* pElt1 = const_cast<rapidjson::Value*>(static_cast<const rapidjson::Value*>(&m_document));
	rapidjson::Value* pElt2 = const_cast<rapidjson::Value*>(static_cast<const rapidjson::Value*>(&json.m_document));
	merge(*pElt1, *pElt2);
}

void IrStd::Json::merge(rapidjson::Value& elt1, const rapidjson::Value& elt2)
{
	if (elt1.IsArray())
	{
		IRSTD_ASSERT(elt2.IsArray(), "Cannot merge an object with a non-array");

		for (auto it = elt2.Begin(); it != elt2.End(); ++it)
		{
			rapidjson::Value val(*it, m_document.GetAllocator());
			elt1.PushBack(val, m_document.GetAllocator());
		}
	}
	else if (elt1.IsObject())
	{
		IRSTD_ASSERT(elt2.IsObject(), "Cannot merge an object with a non-object");

		for (auto it = elt2.MemberBegin(); it != elt2.MemberEnd(); ++it)
		{
			auto dstIt = elt1.FindMember(it->name);
			// If not present in elt1, add it
			if (dstIt == elt1.MemberEnd())
			{
				rapidjson::Value key(it->name, m_document.GetAllocator());
				rapidjson::Value val(it->value, m_document.GetAllocator());
				elt1.AddMember(key, val, m_document.GetAllocator());
			}
			else
			{
				merge(dstIt->value, it->value);
			}
		}
	}
	else
	{
		elt1 = rapidjson::Value(elt2, m_document.GetAllocator());
	}
}

// ---- Number Type -----------------------------------------------------------

bool IrStd::Json::Number::is() const noexcept
{
	return IsNumber();
}

double IrStd::Json::Number::val() const noexcept
{
	return GetDouble();
}

void IrStd::Json::Number::val(const double n) noexcept
{
	SetDouble(n);
}

// ---- Bool Type -----------------------------------------------------------

bool IrStd::Json::Bool::is() const noexcept
{
	return IsBool();
}

bool IrStd::Json::Bool::val() const noexcept
{
	return GetBool();
}

void IrStd::Json::Bool::val(const bool value) noexcept
{
	SetBool(value);
}

// ---- Null Type -----------------------------------------------------------

bool IrStd::Json::Null::is() const noexcept
{
	return IsNull();
}

// ---- String Type -----------------------------------------------------------

bool IrStd::Json::String::is() const noexcept
{
	return IsString();
}

const char* IrStd::Json::String::val() const noexcept
{
	return GetString();
}

void IrStd::Json::String::val(Json& json, const char* const str, const size_t length) noexcept
{

	SetString(str, static_cast<rapidjson::SizeType>(length), json.m_document.GetAllocator());
}

size_t IrStd::Json::String::size() const noexcept
{
	return GetStringLength();
}

bool IrStd::Json::String::empty() const noexcept
{
	return (size() == 0);
}

// ---- String Array ----------------------------------------------------------

bool IrStd::Json::Array::is() const noexcept
{
	return IsArray();
}

size_t IrStd::Json::Array::size() const noexcept
{
	return Size();
}

// ---- String Object ---------------------------------------------------------

bool IrStd::Json::Object::is() const noexcept
{
	return IsObject();
}

// ---- String Object::Iterator -----------------------------------------------

IrStd::Json::Object::Iterator::Iterator(
		const rapidjson::Value::MemberIterator& it,
		rapidjson::Value* val)
		: rapidjson::Value::MemberIterator(it)
		, m_val(val)
{
}

std::pair<const char* const, IrStd::Json::Type&> IrStd::Json::Object::Iterator::operator*()
{
	const char* const pName = (*this)->name.GetString();
	rapidjson::Value& elt = (*m_val)[pName];
	return std::pair<const char* const, Type&>(pName, static_cast<Type&>(elt));
}

IrStd::Json::Object::Iterator IrStd::Json::Object::begin()
{
	return Iterator(MemberBegin(), this);
}

IrStd::Json::Object::Iterator IrStd::Json::Object::end()
{
	return Iterator(MemberEnd(), this);
}

// ---- String Object::ConstIterator ------------------------------------------

IrStd::Json::Object::ConstIterator::ConstIterator(
		const rapidjson::Value::ConstMemberIterator& it,
		const rapidjson::Value* val)
		: rapidjson::Value::ConstMemberIterator(it)
		, m_val(val)
{
}

std::pair<const char* const, const IrStd::Json::Type&> IrStd::Json::Object::ConstIterator::operator*()
{
	const char* const pName = (*this)->name.GetString();
	const rapidjson::Value& elt = (*m_val)[pName];
	return std::pair<const char* const, const Type&>(pName, static_cast<const Type&>(elt));
}

IrStd::Json::Object::ConstIterator IrStd::Json::Object::begin() const
{
	return ConstIterator(MemberBegin(), this);
}

IrStd::Json::Object::ConstIterator IrStd::Json::Object::end() const
{
	return ConstIterator(MemberEnd(), this);
}

// ---- IrStd::Json::create ---------------------------------------------------

void IrStd::Json::create(
		rapidjson::Value& jsonValue,
		rapidjson::Document::AllocatorType& alloc,
		const char* const value)
{
	jsonValue.SetString(value, static_cast<rapidjson::SizeType>(std::strlen(value)), alloc);
}

void IrStd::Json::create(
		rapidjson::Value& jsonValue,
		rapidjson::Document::AllocatorType& alloc,
		std::string value)
{
	jsonValue.SetString(value.c_str(), static_cast<rapidjson::SizeType>(value.length()), alloc);
}

void IrStd::Json::create(
		rapidjson::Value& jsonValue,
		rapidjson::Document::AllocatorType& alloc,
		const IrStd::Type::Gson& node)
{
	switch (node.getType())
	{
	case IrStd::Type::Gson::Type::EMPTY:
		break;
	case IrStd::Type::Gson::Type::BOOL:
		create(jsonValue, alloc, node.getBool());
		break;
	case IrStd::Type::Gson::Type::NUMBER:
		create(jsonValue, alloc, node.getNumber());
		break;
	case IrStd::Type::Gson::Type::STRING:
		create(jsonValue, alloc, node.getString());
		break;
	case IrStd::Type::Gson::Type::ARRAY:
		create(jsonValue, alloc, node.getArray());
		break;
	case IrStd::Type::Gson::Type::MAP:
		create(jsonValue, alloc, node.getMap());
		break;
	default:
		IRSTD_UNREACHABLE();
	}
}

void IrStd::Json::create(
		rapidjson::Value& jsonValue,
		rapidjson::Document::AllocatorType& alloc,
		const Json& json)
{
	jsonValue.CopyFrom(json.m_document, alloc);
}

void IrStd::Json::create(
		rapidjson::Value& jsonValue,
		rapidjson::Document::AllocatorType& alloc,
		Json& json)
{
	jsonValue.CopyFrom(json.m_document, alloc);
}
