#include "../Json.hpp"

#pragma GCC diagnostic push
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

IRSTD_TOPIC_REGISTER(IrStdJson);

IrStd::Json::Json(const char* const str)
{
	rapidjson::ParseResult result = m_document.Parse(str);
	if (!result)
	{
		IRSTD_THROW(IrStd::Topic::IrStdJson, "JSON parse error: "
				<< rapidjson::GetParseError_En(result.Code())
				<< " (offset=" << result.Offset() << ")");
	}
}

void IrStd::Json::print(std::ostream& out) const
{
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	m_document.Accept(writer);
	out << buffer.GetString() << std::endl;
}

rapidjson::Value* IrStd::Json::checkAndGet(std::initializer_list<const char* const>&& attrList) const noexcept
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

void IrStd::Json::String::val(const char* const str, const size_t length) noexcept
{
	SetString(str, static_cast<rapidjson::SizeType>(length));
}

size_t IrStd::Json::String::len() const noexcept
{
	return GetStringLength();
}

// ---- String Array ----------------------------------------------------------

bool IrStd::Json::Array::is() const noexcept
{
	return IsArray();
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
