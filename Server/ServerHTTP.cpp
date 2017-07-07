#include <cstring>
#include <cctype>
#include <algorithm>
#include <map>

#include "ServerHTTP.hpp"
#include "../Assert.hpp"
#include "../Topic.hpp"

IRSTD_TOPIC_USE(IrStdServer);

//#define DEBUG_SERVERHTTP 1

// ---- IrStd::ServerHTTPImpl::ClientInfo -------------------------------------

IrStd::ServerHTTPImpl::ClientInfo::ClientInfo(const int socket)
		: IrStd::ClientInfo(socket)
		, m_nextRequestIsOnlyData(false)
		, m_method(HTTPMethod::UNKNOWN)
		, m_pFetchData(nullptr)
		, m_fetchDataSize(0)
		, m_totalFetchDataSize(0)
		, m_totalDataSize(0)
{
}

size_t IrStd::ServerHTTPImpl::ClientInfo::getTotalDataSize() const noexcept
{
	return m_totalDataSize;
}

const char* IrStd::ServerHTTPImpl::ClientInfo::getData(size_t& size) const noexcept
{
	IRSTD_ASSERT(m_fetchDataSize || m_pFetchData == nullptr, "m_fetchDataSize="
			<< m_fetchDataSize << ", m_pFetchData=" << static_cast<const void* const>(m_pFetchData));
	size = m_fetchDataSize;
	return m_pFetchData;
}

void IrStd::ServerHTTPImpl::ClientInfo::clearData() noexcept
{
	m_fetchDataSize = 0;
	m_pFetchData = nullptr;
	getDataStr().clear();
}

IrStd::HTTPMethod IrStd::ServerHTTPImpl::ClientInfo::getMethod() const noexcept
{
	return m_method;
}

const std::string& IrStd::ServerHTTPImpl::ClientInfo::getURI() const noexcept
{
	return m_uri;
}

std::ostream& operator<<(std::ostream& os, const IrStd::ServerHTTPImpl::ClientInfo& info)
{
	os << static_cast<IrStd::ClientInfo>(info);

	// Print the method of the request
	switch (info.getMethod())
	{
	case IrStd::HTTPMethod::OPTIONS:
		os << ", method=OPTIONS";
		break;
	case IrStd::HTTPMethod::GET:
		os << ", method=GET";
		break;
	case IrStd::HTTPMethod::HEAD:
		os << ", method=HEAD";
		break;
	case IrStd::HTTPMethod::POST:
		os << ", method=POST";
		break;
	case IrStd::HTTPMethod::PUT:
		os << ", method=PUT";
		break;
	case IrStd::HTTPMethod::DELETE:
		os << ", method=DELETE";
		break;
	case IrStd::HTTPMethod::TRACE:
		os << ", method=TRACE";
		break;
	case IrStd::HTTPMethod::CONNECT:
		os << ", method=CONNECT";
		break;
	case IrStd::HTTPMethod::UNKNOWN:
		break;
	default:
		IRSTD_UNREACHABLE();
	}

	// Print the URI
	if (!info.getURI().empty())
	{
		os << ", uri=\"" << info.getURI() << "\"";
	}

	// Print the data size
	if (info.getTotalDataSize())
	{
		os << ", data=" << info.getTotalDataSize() << ".byte(s)";
	}

	return os;
}

// ---- IrStd::ServerHTTP::Context --------------------------------------------

IrStd::ServerHTTP::Context::Context(Response& response, Request& request)
		: m_response(response)
		, m_request(request)
{
}

IrStd::ServerHTTP::Response& IrStd::ServerHTTP::Context::getResponse() noexcept
{
	return m_response;
}

IrStd::ServerHTTP::Request& IrStd::ServerHTTP::Context::getRequest() noexcept
{
	return m_request;
}

const IrStd::ServerHTTP::Response& IrStd::ServerHTTP::Context::getResponse() const noexcept
{
	return m_response;
}

const IrStd::ServerHTTP::Request& IrStd::ServerHTTP::Context::getRequest() const noexcept
{
	return m_request;
}

// ---- IrStd::ServerHTTP -----------------------------------------------------



/*
		https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.20
		       request-header = Accept                   ; Section 14.1
                      | Accept-Charset           ; Section 14.2
                      | Accept-Encoding          ; Section 14.3
                      | Accept-Language          ; Section 14.4
                      | Authorization            ; Section 14.8
                      | Expect                   ; Section 14.20
                      | From                     ; Section 14.22
                      | Host                     ; Section 14.23
                      | If-Match                 ; Section 14.24
                      | If-Modified-Since        ; Section 14.25
                      | If-None-Match            ; Section 14.26
                      | If-Range                 ; Section 14.27
                      | If-Unmodified-Since      ; Section 14.28
                      | Max-Forwards             ; Section 14.31
                      | Proxy-Authorization      ; Section 14.34
                      | Range                    ; Section 14.35
                      | Referer                  ; Section 14.36
                      | TE                       ; Section 14.39
                      | User-Agent               ; Section 14.43
*/

IrStd::ServerHTTP::ProcessStatus IrStd::ServerHTTP::processImpl(
		const int socket,
		ServerHTTPImpl::ClientInfo& info,
		std::string& dataStr)
{
	// Ensure a response is sent 
	class ResponseRAII
	{
	public:
		ResponseRAII(const int socket)
				: m_response(500)
				, m_socket(socket)
				, m_sendOnDestruction(true)
		{
		}

		ProcessStatus sendAndClose()
		{
			m_response.addHeader("Connection", "close");
			return ProcessStatus::CLOSE_CONNECTION;
		}

		ProcessStatus sendAndWaitForNext()
		{
			return ProcessStatus::EXPECT_NEW_PACKET;
		}

		ProcessStatus continueCurrentPacket()
		{
			m_sendOnDestruction = false;
			return ProcessStatus::CONTINUE_CURRENT_PACKET;
		}

		~ResponseRAII()
		{
			if (m_sendOnDestruction)
			{
				m_response.send(m_socket);
			}
		}

		Response m_response;

	private:
		const int m_socket;
		bool m_sendOnDestruction;
	};
	ResponseRAII raii(socket);

	#ifdef DEBUG_SERVERHTTP
		IRSTD_LOG_DEBUG(IrStd::Topic::IrStdServer, "Request through socket " << socket << ":\n"
				<< dataStr);
	#endif

	try
	{
		// Parse the request
		if (!info.m_nextRequestIsOnlyData)
		{

			Data request(dataStr);
			request.parse();

			for (auto header : request.getHeaders())
			{
				// header.toStream(std::cout);
				if (header.is("content-length"))
				{
					std::string value;
					header.getValue(value);
					info.m_totalDataSize = std::stoi(value);
				}

				// header.toStream(std::cout);
				if (header.is("expect"))
				{
					if (header.isValue("100-continue"))
					{
						info.m_nextRequestIsOnlyData = true;
					}
				}
			}

			// Read the method type
			info.m_method = request.getMethod();

			// Read the URI
			request.getURI(info.m_uri);

			// Ignore the data is m_nextRequestIsOnlyData is on
			if (info.m_nextRequestIsOnlyData)
			{
				raii.m_response.setStatus(100);
				return raii.sendAndWaitForNext();
			}

			// Read the data
			info.m_pFetchData = request.getData(info.m_fetchDataSize);
			info.m_totalFetchDataSize += info.m_fetchDataSize;
		}
		else
		{
			info.m_pFetchData = dataStr.data();
			info.m_fetchDataSize = dataStr.size();
			info.m_totalFetchDataSize += info.m_fetchDataSize;
		}

		// Set the total amount of data to the number of data fetched in case it is not
		// explicitly specified in the headers
		info.m_totalDataSize = (info.m_totalDataSize) ?
				info.m_totalDataSize : info.m_fetchDataSize;

		IRSTD_THROW_ASSERT(IrStd::Topic::IrStdServer,
				info.m_totalFetchDataSize <= info.m_totalDataSize,
				"The amount of data fetched (" << info.m_totalFetchDataSize
				<< ") is larger that the total amount of data ("
				<< info.m_totalDataSize << ")");
	}
	catch (const Exception& e)
	{
		IRSTD_LOG_ERROR(IrStd::Topic::IrStdServer, "Error while parsing request ("
				<< info << "): " << e.what());
		raii.m_response.setStatus(400);
		return raii.sendAndClose();
	}

	IRSTD_LOG_TRACE(IrStd::Topic::IrStdServer, "HTTP Request: " << info);

	// Generate the response
	try
	{
		raii.m_response.setStatus(200);
		{
			Context context(raii.m_response, info);
			handleResponse(context);
		}
		// Discard the data that has not been pre-fetched
		readData(info, [](const char*, const size_t) {});
	}
	catch (const Exception& e)
	{
		IRSTD_LOG_ERROR(IrStd::Topic::IrStdServer, "Error while preparing response ("
				<< info << "): " << e.what());
		raii.m_response.setStatus(500);
		return raii.sendAndClose();
	}

	return raii.sendAndClose();
}

void IrStd::ServerHTTP::handleResponse(Context& context)
{
	IRSTD_LOG_INFO(IrStd::Topic::IrStdServer, "IrStd::ServerHTTP::handleResponse("
			<< context.getRequest().getTotalDataSize() << ")");
}

size_t IrStd::ServerHTTP::fetchData(Request& request, const size_t maxSize, const bool overwriteData)
{
	auto& data = request.getDataStr();

	// Clear the current data
	if (overwriteData)
	{
		request.clearData();
	}

	size_t nbDataRead = 0;
	if (request.m_totalFetchDataSize < request.m_totalDataSize)
	{
		const size_t nbDataToRead = std::min(request.m_totalDataSize - request.m_totalFetchDataSize, maxSize);

		IRSTD_LOG_TRACE(IrStd::Topic::IrStdServer, "Fetching " << nbDataToRead
				<< " byte(s) from " << request);

		nbDataRead = receiveSpecificAmount(request.getSocket(), data, nbDataToRead);
		request.m_fetchDataSize = data.size();
		request.m_pFetchData = data.c_str();
		request.m_totalFetchDataSize += nbDataRead;
	}

	return nbDataRead;
}

// ---- IrStd::ServerHTTP::Data -----------------------------------------------

constexpr char IrStd::ServerHTTP::Data::CRLF[];
constexpr char IrStd::ServerHTTP::Data::CRLF2[];
constexpr std::array<char, 5> IrStd::ServerHTTP::Data::TRIM_CHAR;

IrStd::ServerHTTP::Data::Data(const std::string& data)
		: m_data(data)
		, m_dataRequestBegin(m_data.size())
		, m_dataRequestEnd(m_data.size())
		, m_dataHeaderBegin(m_data.size())
		, m_dataHeaderEnd(m_data.size())
		, m_dataBodyBegin(m_data.size())
		, m_dataBodyEnd(m_data.size())
		, m_dataRequestMethodEnd(m_data.size())
		, m_dataRequestURIBegin(m_data.size())
		, m_dataRequestURIEnd(m_data.size())
{
}

size_t IrStd::ServerHTTP::Data::lTrim(size_t index) const noexcept
{
	while (index < m_data.size() &&
			std::any_of(TRIM_CHAR.cbegin(), TRIM_CHAR.cend(), [&](const char c){
				return m_data.at(index) == c;
			}))
	{
		index++;
	}
	return index;
}

size_t IrStd::ServerHTTP::Data::rTrim(size_t index) const noexcept
{
	IRSTD_ASSERT(index < m_data.size());
	while (index > 0 &&
			std::any_of(TRIM_CHAR.cbegin(), TRIM_CHAR.cend(), [&](const char c){
				return m_data.at(index) == c;
			}))
	{
		index--;
	}
	return index;
}

bool IrStd::ServerHTTP::Data::compare(
		const size_t indexBegin,
		const size_t indexEnd,
		const char* const str) const noexcept
{
	IRSTD_ASSERT(indexBegin < indexEnd);
	IRSTD_ASSERT(indexEnd <= m_data.size());

	const auto strLength = std::strlen(str);
	if (strLength != indexEnd - indexBegin)
	{
		return false;
	}
	for (size_t i = 0; i<strLength; i++)
	{
		if (std::tolower(str[i]) != std::tolower(m_data[indexBegin + i]))
		{
			return false;
		}
	}
	return true;
}

void IrStd::ServerHTTP::Data::parse()
{
	// In the interest of robustness, servers SHOULD ignore any empty line(s)
	// received where a Request-Line is expected. In other words, if the server
	// is reading the protocol stream at the beginning of a message and receives
	// a CRLF first, it should ignore the CRLF.
	{
		m_dataRequestBegin = lTrim(0);
		IRSTD_THROW_ASSERT(m_dataRequestBegin < m_data.size(), "No request found in data received, dump=" << m_data);
	}

	// Look for the double CRLF sequence
	{
		const auto pos = m_data.find(CRLF2, m_dataRequestBegin);
		IRSTD_THROW_ASSERT(pos != std::string::npos, "There must be a double CRLF sequence in the message");
		// Note: using CRLF instead of CRLF2 is intended here
		m_dataHeaderEnd = pos + std::strlen(CRLF);
		m_dataBodyBegin = pos + std::strlen(CRLF2);
		m_dataBodyEnd = m_data.size();
	}

	// Look for the headers section
	{
		const auto pos = m_data.find(CRLF, m_dataRequestBegin);
		IRSTD_THROW_ASSERT(pos != std::string::npos, "There must be a CRLF sequence in the message");
		m_dataHeaderBegin = pos + std::strlen(CRLF);
		m_dataRequestEnd = rTrim(pos);
	}

	// Parse the request line
	// METHOD <space> URI <space> VERSION <CRLF>
	{
		const auto pos = m_data.find(' ', m_dataRequestBegin);
		IRSTD_THROW_ASSERT(pos != std::string::npos, "Request is wrongly constructed, dump=" << m_data);
		IRSTD_THROW_ASSERT(pos < m_dataHeaderBegin, "Request is wrongly constructed, dump=" << m_data);
		IRSTD_THROW_ASSERT(m_dataRequestMethodEnd > 1, "Request is wrongly constructed, dump=" << m_data);
		m_dataRequestMethodEnd = pos - 1;

		// Isolate the URI
		m_dataRequestURIBegin = lTrim(m_dataRequestMethodEnd + 1);
		const auto posEnd = m_data.rfind("HTTP", m_dataRequestEnd);
		IRSTD_THROW_ASSERT(posEnd != std::string::npos, "Request is wrongly constructed, dump=" << m_data);
		IRSTD_THROW_ASSERT(posEnd > 1);
		m_dataRequestURIEnd = rTrim(posEnd-1);
		IRSTD_THROW_ASSERT(m_dataRequestURIBegin <= m_dataRequestURIEnd, "Request is wrongly constructed, dump=" << m_data);
	}

	// Sanity check
	IRSTD_THROW_ASSERT(m_dataHeaderBegin <= m_dataHeaderEnd, "Message is wrongly constructed, dump=" << m_data);
}

IrStd::ServerHTTP::Data::Headers IrStd::ServerHTTP::Data::getHeaders() const noexcept
{
	return Headers(*this);
}

const char* IrStd::ServerHTTP::Data::getData(size_t& length) const noexcept
{
	length = m_dataBodyEnd - m_dataBodyBegin;
	return (length) ? &m_data[m_dataBodyBegin] : nullptr;
}

IrStd::HTTPMethod IrStd::ServerHTTP::Data::getMethod() const
{
	class Map
	{
	public:
		static std::map<std::string, HTTPMethod> create()
		{
			std::map<std::string, HTTPMethod> methodMap;

			methodMap["OPTIONS"] = HTTPMethod::OPTIONS;
			methodMap["GET"] = HTTPMethod::GET;
			methodMap["HEAD"] = HTTPMethod::HEAD;
			methodMap["POST"] = HTTPMethod::POST;
			methodMap["PUT"] = HTTPMethod::PUT;
			methodMap["DELETE"] = HTTPMethod::DELETE;
			methodMap["TRACE"] = HTTPMethod::TRACE;
			methodMap["CONNECT"] = HTTPMethod::CONNECT;

			return methodMap;
		}
	};
	static const auto methodMap = Map::create();

	// Read the method string and store it in uppercase
	std::string methodStr = m_data.substr(m_dataRequestBegin, m_dataRequestMethodEnd - m_dataRequestBegin + 1);
	const auto it = methodMap.find(methodStr);
	IRSTD_THROW_ASSERT(it != methodMap.end(), "Unable to identify method type from '" << methodStr << "'");

	return it->second;
}

void IrStd::ServerHTTP::Data::getURI(std::string& uri) const noexcept
{
	uri = m_data.substr(m_dataRequestURIBegin, m_dataRequestURIEnd - m_dataRequestURIBegin + 1);
}

// ---- IrStd::ServerHTTP::Data::Header ---------------------------------------

IrStd::ServerHTTP::Data::Header::Header(const Data& data, const size_t index)
		: m_data(data)
		, m_dataHeader(index)
{
	// Trim white spaces
	m_fieldNameBegin = m_data.lTrim(m_dataHeader);

	// Look for the end of the header
	{
		const auto pos = data.m_data.find(CRLF, m_fieldNameBegin);
		IRSTD_ASSERT(pos != std::string::npos);
		m_valueEnd = m_data.rTrim(pos) + 1;
		IRSTD_ASSERT(m_fieldNameBegin < m_valueEnd, "Invalid header");
	}

	// Look for the value separator
	{
		const auto pos = data.m_data.find(":", m_fieldNameBegin);
		// Handle the case where there is no seprator
		if (pos == std::string::npos || pos > m_valueEnd)
		{
			m_fieldNameEnd = m_valueEnd;
			m_valueBegin = m_valueEnd;
		}
		else
		{
			m_fieldNameEnd = m_data.rTrim(pos - 1) + 1;
			m_valueBegin = m_data.lTrim(pos + 1);
			IRSTD_ASSERT(m_fieldNameBegin < m_fieldNameEnd);
			IRSTD_ASSERT(m_fieldNameEnd < m_valueBegin);
			IRSTD_ASSERT(m_valueBegin < m_valueEnd);
		}
	}
}

bool IrStd::ServerHTTP::Data::Header::is(const char* const header) const noexcept
{
	return m_data.compare(m_fieldNameBegin, m_fieldNameEnd, header);
}

bool IrStd::ServerHTTP::Data::Header::isValue(const char* const value) const noexcept
{
	return m_data.compare(m_valueBegin, m_valueEnd, value);
}

void IrStd::ServerHTTP::Data::Header::getValue(std::string& value) const noexcept
{
	value.assign(&m_data.m_data[m_valueBegin], m_valueEnd - m_valueBegin);
}

void IrStd::ServerHTTP::Data::Header::toStream(std::ostream& os) const
{
	os << "[";
	os.write(&m_data.m_data[m_fieldNameBegin], m_fieldNameEnd - m_fieldNameBegin);
	os << "] = ";
	os.write(&m_data.m_data[m_valueBegin], m_valueEnd - m_valueBegin);
	os << std::endl;
}

// ---- IrStd::ServerHTTP::Data::Headers --------------------------------------

IrStd::ServerHTTP::Data::Headers::Headers(const Data& data)
		: m_data(data)
{
}

IrStd::ServerHTTP::Data::Headers::iterator IrStd::ServerHTTP::Data::Headers::begin() const noexcept
{
	return iterator(m_data, m_data.m_dataHeaderBegin);
}

IrStd::ServerHTTP::Data::Headers::iterator IrStd::ServerHTTP::Data::Headers::end() const noexcept
{
	return iterator(m_data, m_data.m_dataHeaderEnd);
}

// ---- IrStd::ServerHTTP::Data::Headers::iterator ----------------------------

IrStd::ServerHTTP::Data::Headers::iterator::iterator(const Data& data, const size_t index)
		: m_data(data)
		, m_dataHeader(index)
{
}

IrStd::ServerHTTP::Data::Headers::iterator& IrStd::ServerHTTP::Data::Headers::iterator::operator++()
{
	const auto pos = m_data.m_data.find(CRLF, m_dataHeader);
	m_dataHeader = (pos == std::string::npos) ? m_data.m_dataHeaderEnd : pos + std::strlen(CRLF);
	return *this;
}

bool IrStd::ServerHTTP::Data::Headers::iterator::operator==(const iterator& other) const
{
	return m_dataHeader == other.m_dataHeader;
}

bool IrStd::ServerHTTP::Data::Headers::iterator::operator!=(const iterator& other) const
{
	return !(*this == other);
}

IrStd::ServerHTTP::Data::Header IrStd::ServerHTTP::Data::Headers::iterator::operator*() const
{
	return Header(m_data, m_dataHeader);
}

// ---- IrStd::ServerHTTP::Response -------------------------------------------

IrStd::ServerHTTP::Response::Response(const size_t code)
{
	setStatus(code);
}

void IrStd::ServerHTTP::Response::setStatus(const size_t code, const char* const pReason) noexcept
{
	switch (code)
	{
	case 100:
		m_pReason = "Continue";
		break;
	case 101:
		m_pReason = "Switching Protocols";
		break;
	case 200:
		m_pReason = "OK";
		break;
	case 201:
		m_pReason = "Created";
		break;
	case 202:
		m_pReason = "Accepted";
		break;
	case 203:
		m_pReason = "Non-Authoritative Information";
		break;
	case 204:
		m_pReason = "No Content";
		break;
	case 205:
		m_pReason = "Reset Content";
		break;
	case 206:
		m_pReason = "Partial Content";
		break;
	case 300:
		m_pReason = "Multiple Choices";
		break;
	case 301:
		m_pReason = "Moved Permanently";
		break;
	case 302:
		m_pReason = "Found";
		break;
	case 303:
		m_pReason = "See Other";
		break;
	case 304:
		m_pReason = "Not Modified";
		break;
	case 305:
		m_pReason = "Use Proxy";
		break;
	case 307:
		m_pReason = "Temporary Redirect";
		break;
	case 400:
		m_pReason = "Bad Request";
		break;
	case 401:
		m_pReason = "Unauthorized";
		break;
	case 402:
		m_pReason = "Payment Required";
		break;
	case 403:
		m_pReason = "Forbidden";
		break;
	case 404:
		m_pReason = "Not Found";
		break;
	case 405:
		m_pReason = "Method Not Allowed";
		break;
	case 406:
		m_pReason = "Not Acceptable";
		break;
	case 407:
		m_pReason = "Proxy Authentication Required";
		break;
	case 408:
		m_pReason = "Request Time-out";
		break;
	case 409:
		m_pReason = "Conflict";
		break;
	case 410:
		m_pReason = "Gone";
		break;
	case 411:
		m_pReason = "Length Required";
		break;
	case 412:
		m_pReason = "Precondition Failed";
		break;
	case 413:
		m_pReason = "Request Entity Too Large";
		break;
	case 414:
		m_pReason = "Request-URI Too Large";
		break;
	case 415:
		m_pReason = "Unsupported Media Type";
		break;
	case 416:
		m_pReason = "Requested range not satisfiable";
		break;
	case 417:
		m_pReason = "Expectation Failed";
		break;
	case 500:
		m_pReason = "Internal Server Error";
		break;
	case 501:
		m_pReason = "Not Implemented";
		break;
	case 502:
		m_pReason = "Bad Gateway";
		break;
	case 503:
		m_pReason = "Service Unavailable";
		break;
	case 504:
		m_pReason = "Gateway Time-out";
		break;
	case 505:
		m_pReason = "HTTP Version not supported";
		break;
	default:
		m_pReason = "";
	}

	m_code = code;
	if (pReason)
	{
		m_pReason = pReason;
	}
}

void IrStd::ServerHTTP::Response::addHeader(const char* const pField, const Type::ShortString value)
{
	m_headers.append(pField);
	m_headers.append(": ");
	m_headers.append(value);
	m_headers.append(Data::CRLF);
}

void IrStd::ServerHTTP::Response::addHeader(const char* const pStr)
{
	m_headers.append(pStr);
	m_headers.append(Data::CRLF);
}

void IrStd::ServerHTTP::Response::addData(const Type::ShortString value)
{
	m_data.append(value);
}

void IrStd::ServerHTTP::Response::addData(const char* const data, const size_t size)
{
	m_data.append(data, size);
}

void IrStd::ServerHTTP::Response::setData(const Type::ShortString value)
{
	m_data.assign(value);
}

void IrStd::ServerHTTP::Response::setData(const char* const data, const size_t size)
{
	m_data.assign(data, size);
}

/**
 * Response format:
 *
 *  Response      = Status-Line               ; Section 6.1
 *                  *(( general-header        ; Section 4.5
 *                   | response-header        ; Section 6.2
 *                   | entity-header ) CRLF)  ; Section 7.1
 *                  CRLF
 *                  [ message-body ]          ; Section 7.2		
 */
void IrStd::ServerHTTP::Response::send(const int socket) const
{
	// Build the header
	std::string response("HTTP/1.1 ");
	response.append(IrStd::Type::ShortString(m_code));
	response.append(" ");
	response.append(m_pReason);

	// If response is different from 200, print it
	IRSTD_LOG_TRACE(IrStd::Topic::IrStdServer, "Response (socket="
			<< socket << "): " << response);

	response.append(Data::CRLF);

	// Add the headers
	// Add the content-length header
	response.append(m_headers);
	response.append("Content-Length: ");
	response.append(IrStd::Type::ShortString(m_data.size()));
	response.append(Data::CRLF);
	response.append(Data::CRLF);

	// Add the data
	response.append(m_data);

	#ifdef DEBUG_SERVERHTTP
		IRSTD_LOG_DEBUG(IrStd::Topic::IrStdServer, "Response (socket=" << socket << ") (continue):\n"
				<< response);
	#endif

	const auto ret = ::send(socket, response.c_str(), response.size(), 0);
	IRSTD_THROW_ASSERT(IrStd::Topic::IrStdServer, ret != -1, "send: code="
			<< errno << ", str=" << ::strerror(errno));
}
