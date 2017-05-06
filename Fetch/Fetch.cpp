#include "../Fetch.hpp"
#include "../Logger.hpp"

#include "FetchCurl.hpp"

IRSTD_TOPIC_REGISTER(IrStdFetch);

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

std::future<IrStd::Fetch::Status> IrStd::FetchUrl::process()
{
	FetchCurl curl(m_data);
	return std::async(std::launch::async, &IrStd::FetchCurl::url, curl, m_url); 
}

IrStd::Fetch::Status IrStd::FetchUrl::processSync()
{
	FetchCurl curl(m_data);
	return curl.url(m_url);
}
