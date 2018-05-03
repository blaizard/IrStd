#include "../Fetch.hpp"
#include "../Logger.hpp"

#include "FetchCurl.hpp"

IRSTD_TOPIC_REGISTER(IrStd, Fetch);
IRSTD_TOPIC_USE_ALIAS(IrStdFetch, IrStd, Fetch);

IrStd::Fetch::Fetch(std::string& data)
		: m_data(data)
{
}

IrStd::Fetch::Status IrStd::Fetch::processSync()
{
	auto future = process();
	future.wait();
	return future.get();
}

// ---- IrStd::FetchUrl -------------------------------------------------------

IrStd::FetchUrl::FetchUrl(
		const char* const url,
		std::string& data)
		: Fetch(data)
		, m_url(url)
{
}

const std::string& IrStd::FetchUrl::getUrl() const noexcept
{
	return m_url;
}

std::future<IrStd::Fetch::Status> IrStd::FetchUrl::process()
{
	FetchCurl curl(m_data);
	return std::async(std::launch::async, &IrStd::FetchCurl::url, curl, *this); 
}

IrStd::Fetch::Status IrStd::FetchUrl::processSync()
{
	FetchCurl curl(m_data);
	return curl.url(*this);
}

void IrStd::FetchUrl::addPost(const char* const str)
{
	std::string paramStr;

	IrStd::FetchCurl::urlEncode(paramStr, str);

	if (!m_post.empty())
	{
		m_post.append("&");
	}
	m_post.append(paramStr);
}

void IrStd::FetchUrl::addPost(const char* const param, const Type::ShortString value)
{
	std::string paramStr, valueStr;

	IrStd::FetchCurl::urlEncode(paramStr, param);
	IrStd::FetchCurl::urlEncode(valueStr, value);

	if (!m_post.empty())
	{
		m_post.append("&");
	}
	m_post.append(paramStr);
	m_post.append("=");
	m_post.append(valueStr);
}

const std::string& IrStd::FetchUrl::getPost() const noexcept
{
	return m_post;
}

void IrStd::FetchUrl::addHeader(const char* const header)
{
	m_headerList.push_back(std::string(header));
}

void IrStd::FetchUrl::addHeader(const char* const param, const Type::ShortString value)
{
	std::string str(param);
	str.append(": ");
	str.append(value);
	m_headerList.push_back(str);
}
