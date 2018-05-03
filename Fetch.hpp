#pragma once

#include <string>
#include <future>
#include <memory>
#include <vector>
#include <sstream>

#include "Type/ShortString.hpp"

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

	// Forward declaration
	class FetchCurl;
	class FetchUrl : public Fetch
	{
	public:
		FetchUrl(const char* const url, std::string& data);

		const std::string& getUrl() const noexcept;

		/**
		 * Add post data to the request
		 */
		void addPost(const char* const str);
		void addPost(const char* const param, const Type::ShortString value);
		template<class Value>
		void addPost(const char* const param, const Value& value)
		{
			std::stringstream streamStr;
			streamStr << value;
			addPost(param, Type::ShortString(streamStr.str().c_str()));
		}

		/**
		 * Return the post string
		 */
		const std::string& getPost() const noexcept;

		/**
		 * Add a custom header to the request
		 */
		void addHeader(const char* const str);
		void addHeader(const char* const param, const Type::ShortString value);
		template<class Value>
		void addHeader(const char* const param, const Value& value)
		{
			std::stringstream streamStr;
			streamStr << value;
			addHeader(param, Type::ShortString(streamStr.str().c_str()));
		}

		/**
		 * Fetch data from a url
		 */
		std::future<Status> process();
		Status processSync();

	private:
		friend FetchCurl;
		std::string m_url;
		std::string m_post;
		std::vector<std::string> m_headerList;
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
