#pragma once

#include <string>
#include <future>
#include <memory>

namespace IrStd
{
	class Fetch
	{
	public:
		enum class Status
		{
			OK = 0,
			ERROR
		};

		/**
		 * \param data A buffer where to store the data fetch
		 */
		Fetch(std::string& data);

		/**
		 * Fetch data
		 */
		virtual std::future<Status> process() = 0;
		virtual Status processSync();

	protected:
		std::string& m_data;
	};

	class FetchUrl : public Fetch
	{
	public:
		FetchUrl(const char* const url, std::string& data)
				: Fetch(data)
				, m_url(url)
		{
		}

		/**
		 * Fetch data from a url
		 */
		std::future<Status> process();
		Status processSync();

	private:
		const char* const m_url;
	};


	class FetchRepeat
	{
	public:
		FetchRepeat(std::shared_ptr<Fetch> pFetch)
				: m_pFetch(pFetch)
		{
		}

		void everyTimeMs(const size_t time);

		bool isData();

	private:
		std::shared_ptr<Fetch> m_pFetch;
	};
}
