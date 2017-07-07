#pragma once

#include <limits>

#include "../Server.hpp"

namespace IrStd
{
	enum class HTTPMethod
	{
		UNKNOWN = 0,
		OPTIONS,
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		TRACE,
		CONNECT
	};

	class ServerHTTP;
	namespace ServerHTTPImpl
	{
		class ClientInfo : public IrStd::ClientInfo
		{
		public:
			explicit ClientInfo(const int socket);

			// Data related functions
			void clearData() noexcept;
			const char* getData(size_t& size) const noexcept;
			size_t getTotalDataSize() const noexcept;

			// Method of request
			HTTPMethod getMethod() const noexcept;

			const std::string& getURI() const noexcept;

		private:
			friend ServerHTTP;

			bool m_nextRequestIsOnlyData;

			HTTPMethod m_method;
			std::string m_uri;

			// Data related
			const char* m_pFetchData;
			size_t m_fetchDataSize;
			size_t m_totalFetchDataSize;
			size_t m_totalDataSize;
		};
	}

	class ServerHTTP : public Server<ServerHTTPImpl::ClientInfo>
	{
	protected:
		class Response
		{
		public:
			Response(const size_t code = 200);
			void setStatus(const size_t code, const char* const pReason = nullptr) noexcept;
			void send(const int socket) const;

			/**
			 * Add header to the response
			 */
			void addHeader(const char* const str);
			void addHeader(const char* const field, const Type::ShortString value);

			/**
			 * Set data to the response
			 */
			void setData(const Type::ShortString value);
			void setData(const char* const data, const size_t size);

			/**
			 * Add data to the response
			 */
			void addData(const Type::ShortString value);
			void addData(const char* const data, const size_t size);

		private:
			size_t m_code;
			const char* m_pReason;
			std::string m_headers;
			std::string m_data;
		};

		typedef ServerHTTPImpl::ClientInfo Request;

	public:
		template<class ... Args>
		ServerHTTP(Args&& ... args)
				: Server(std::forward<Args>(args)...)
		{
		}

		class Context
		{
		public:
			Context(Response& response, Request& request);
			Response& getResponse() noexcept;
			Request& getRequest() noexcept;
			const Response& getResponse() const noexcept;
			const Request& getRequest() const noexcept;
		private:
			Response& m_response;
			Request& m_request;
		};

		virtual void handleResponse(Context& context);

		/*
		 * Wrapper to go through the data.
		 * As this is a stream, reading consume the data.
		 */
		template<class Function>
		void readData(Request& request, Function processDataFct, const size_t prefetchChunkSize = 0)
		{
			const size_t chunkSize = (prefetchChunkSize) ? prefetchChunkSize :
					std::max(request.getDataStr().capacity(), static_cast<const size_t>(1024*1024));
			do
			{
				size_t size;
				const auto pData = request.getData(size);
				if (size)
				{
					processDataFct(pData, size);
				}
			} while (fetchData(request, chunkSize, /*overwriteData*/true));
		}

		size_t fetchData(Request& request, const size_t maxSize = std::numeric_limits<size_t>::max(),
				const bool overwriteData = false);

	private:
		ProcessStatus processImpl(const int socket, ServerHTTPImpl::ClientInfo& info, std::string& data) override;

		class Data
		{
		public:
			Data(const std::string& data);

			/**
			 * Left trim
			 */
			size_t lTrim(size_t index) const noexcept;

			/**
			 * Right trim
			 */
			size_t rTrim(size_t index) const noexcept;

			/**
			 * Compare two strings
			 */
			bool compare(const size_t indexBegin, const size_t indexEnd, const char* const str) const noexcept;

			/**
			 * Header format (RFC 2616):
			 *
			 * message-header = field-name ":" [ field-value ]
			 * field-name     = token
			 * field-value    = *( field-content | LWS )
			 * field-content  = <the OCTETs making up the field-value
			 *                  and consisting of either *TEXT or combinations
			 *                  of token, separators, and quoted-string>
			 */
			class Header
			{
			public:
				Header(const Data& data, const size_t index);

				/**
				 * Check if the header is equal to the one passed into argument
				 */
				bool is(const char* const header) const noexcept;

				/**
				 * Check if the value is equal to the one passed into argument
				 */
				bool isValue(const char* const value) const noexcept;

				/**
				 * Read the value
				 */
				void getValue(std::string& value) const noexcept;

				void toStream(std::ostream& os) const;

			private:
				const Data& m_data;
				const size_t m_dataHeader;
				size_t m_fieldNameBegin;
				size_t m_fieldNameEnd;
				size_t m_valueBegin;
				size_t m_valueEnd;
			};

			class Headers
			{
			public:
				class iterator
				{
				public:
					iterator(const Data& data, const size_t index);

					iterator& operator++();
					bool operator==(const iterator& other) const;
					bool operator!=(const iterator& other) const;
					Header operator*() const;

				private:
					const Data& m_data;
					size_t m_dataHeader;
				};

				Headers(const Data& data);

				iterator begin() const noexcept;
				iterator end() const noexcept;

			private:
				const Data& m_data;
			};

			/**
			 * Message format (RFC 2616):
			 *
			 * generic-message = start-line
			 *                   *(message-header CRLF)
			 *                   CRLF
			 *                   [ message-body ]
			 * start-line      = Request-Line | Status-Line
			 */
			void parse();

			Headers getHeaders() const noexcept;
			const char* getData(size_t& length) const noexcept;

			HTTPMethod getMethod() const;
			void getURI(std::string& uri) const noexcept;

			static constexpr char CRLF[] = "\r\n";
			static constexpr char CRLF2[] = "\r\n\r\n";
			static constexpr std::array<char, 5> TRIM_CHAR = {'\r', '\n', '\t', '\0', ' '};

		private:
			friend Headers;

			const std::string& m_data;

			// Information that will be set after parse
			size_t m_dataRequestBegin;
			size_t m_dataRequestEnd;
			size_t m_dataHeaderBegin;
			size_t m_dataHeaderEnd;
			size_t m_dataBodyBegin;
			size_t m_dataBodyEnd;
			size_t m_dataRequestMethodEnd;
			size_t m_dataRequestURIBegin;
			size_t m_dataRequestURIEnd;
		};
	};
}

std::ostream& operator<<(std::ostream& os, const IrStd::ServerHTTPImpl::ClientInfo& info);
