#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <poll.h>
#include <algorithm>

#include "../Server.hpp"
#include "../Logger.hpp"
#include "../Assert.hpp"
#include "../Type/ShortString.hpp"

IRSTD_TOPIC_USE(IrStd, Server);

// ---- IrStd::Server ---------------------------------------------------------

template<class T>
IrStd::Server<T>::Server(
		const int port,
		const size_t maxConnections,
		const int backLog)
		: m_manager(maxConnections)
		, m_port(static_cast<uint16_t>(port))
		, m_backLog(static_cast<uint16_t>(backLog))
		, m_status(Status::IDLE)
{
}

template<class T>
size_t IrStd::Server<T>::receiveSpecificAmount(const int socket, std::string& data, const size_t size)
{
	size_t dataReceived = 0;
	while (dataReceived < size)
	{
		dataReceived += receive(socket, data, size - dataReceived);
	}

	IRSTD_ASSERT(dataReceived == size, "Size received (" << dataReceived
			<< ") is greated that size requested (" << size << ")");

	return dataReceived;
}

template<class T>
size_t IrStd::Server<T>::receive(const int socket, std::string& data, const size_t maxSize)
{
	constexpr size_t CHUNK_SIZE = 512;

	// Monitor socket for input
	struct ::pollfd fds[1];
	fds[0].fd = socket;
	fds[0].events = POLLIN;

	const size_t originalSize = data.size();
	size_t size = 0;
	size_t chunckSize = 0;

	while (isStarted())
	{
		// Adjust the chunck size
		chunckSize = std::min(maxSize - size, CHUNK_SIZE);
		if (chunckSize == 0)
		{
			break;
		}

		// Wait for an event
		const int retPoll = ::poll(fds, 1, CLIENT_TIMEOUT_MS);

		IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Server), retPoll != -1,
				"poll: code=" << errno << ", str=" << ::strerror(errno))

		IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Server), retPoll != 0,
				"Timeout while receiving data");

		if (fds[0].revents & POLLIN)
		{
			data.resize(originalSize + size + chunckSize);
			const auto sizeRecv = ::recv(socket, &data[originalSize + size], chunckSize, 0);

			IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Server), sizeRecv >= 0,
					"recv: code=" << errno << ", str=" << ::strerror(errno));

			size += sizeRecv;

			{
				int count;
				const auto retIoctl = ::ioctl(socket, FIONREAD, &count);
				IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Server), retIoctl != -1,
						"ioctl: code=" << errno << ", str=" << ::strerror(errno));

				if (sizeRecv == 0 || count == 0)
				{
					break;
				}
			}
		}
		else
		{
			IRSTD_UNREACHABLE();
		}
	}

	data.resize(originalSize + size);
	return size;
}

template<class T>
void IrStd::Server<T>::closeSocket(const int socket) noexcept
{
	IRSTD_ASSERT(IRSTD_TOPIC(IrStd, Server), socket >= 0, "socket=" << socket);

	try
	{
		// Clear any errors, which can cause close to fail
		{
			int err = 1;
			::socklen_t len = sizeof(err);
			const auto ret = ::getsockopt(socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&err), &len);
			IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Server), ret != -1, "::getsockopt returned " << ret
					<< ", code=" << errno << ", str=" << ::strerror(errno));
			// Set errno to the socket SO_ERROR
			if (err)
			{
				errno = err; 
			}
		}
		// Shutdown the socket (for read and write)
		{
			const auto ret = ::shutdown(socket, SHUT_RDWR);
			IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Server), ret >= 0 || errno == ENOTCONN || errno == EINVAL,
					"::shutdown returned " << ret << ", code=" << errno << ", str=" << ::strerror(errno));

		}
		/* Make sure there is no received data
		{
			char c;
			const auto ret = ::recv(socket, &c, 1, 0);
			IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Server), ret >= 0 || errno == ENOTCONN || errno == EINVAL,
					"::recv returned " << ret << ", code=" << errno << ", str=" << ::strerror(errno));
		}*/
	}
	catch (const IrStd::Exception& e)
	{
		IRSTD_LOG_ERROR(IRSTD_TOPIC(IrStd, Server), "Error while closing thread: " << e
				<< ", forcing close anyway");
	}

	// Close the damn socket, if it fails only print an error, there is nothing else we can do :(
	{
		const auto ret = ::close(socket);
		if (ret == -1)
		{
			IRSTD_LOG_FATAL(IRSTD_TOPIC(IrStd, Server), "::close returned " << ret
					<< ", code=" << errno << ", str=" << ::strerror(errno));
		}
	}
}

template<class T>
void IrStd::Server<T>::process(const size_t index)
{
	// Ensure that the status of this thread is set to TERMINATED
	// when exiting this function
	class RAII
	{
	public:
		RAII(IrStd::ServerImpl::ConnectionManager<T>& manager, const size_t index)
				: m_manager(manager)
				, m_index(index)
		{
		}
		~RAII()
		{
			const auto socket = m_manager.getSocket(m_index);
			IRSTD_LOG_TRACE(IRSTD_TOPIC(IrStd, Server), "Closing connection #" << m_index
					<< " with socket " << socket);
			m_manager.setStatus(m_index, ServerImpl::Status::TERMINATED);
			closeSocket(socket);
		}
	private:
		IrStd::ServerImpl::ConnectionManager<T>& m_manager;
		const size_t m_index;
	};
	const auto socket = m_manager.getSocket(index);
	RAII setTerminateStatus(m_manager, index);

	// As client info can be updated as long as the connection is open
	// make a local copy of this variable
	auto clientInfo = m_manager.getClientInfo(index);
	auto& data = clientInfo.getDataStr();

	ProcessStatus processStatus = ProcessStatus::EXPECT_NEW_PACKET;
	while (isStarted() && (
		processStatus == ProcessStatus::EXPECT_NEW_PACKET
		|| processStatus == ProcessStatus::CONTINUE_CURRENT_PACKET))
	{
		m_manager.setStatus(index, ServerImpl::Status::RECEIVING);
		IRSTD_LOG_TRACE(IRSTD_TOPIC(IrStd, Server), "Incoming connection #" << index);

		if (processStatus != ProcessStatus::CONTINUE_CURRENT_PACKET)
		{
			data.clear();
		}

		try
		{
			receive(socket, data);
		}
		catch (const Exception& e)
		{
			IRSTD_LOG_FATAL(IRSTD_TOPIC(IrStd, Server), "Error while receiving data on connection #"
					<< index << " (" << clientInfo << "): " << e);
			break;
		}

		if (isStarted())
		{
			m_manager.setStatus(index, ServerImpl::Status::RUNNING);
			IRSTD_LOG_INFO(IRSTD_TOPIC(IrStd, Server), "Processing connection #" << index
					<< ", data=" << data.size() << ".byte(s) (" << m_manager.getClientInfo(index) << ")");
			IRSTD_ASSERT(IRSTD_TOPIC(IrStd, Server), m_manager.getSocket(index) == socket,
					"The socket (" << m_manager.getSocket(index) << ") of connection #"
					<< index << ", does not match its initial value (" << socket << ")");
			processStatus = processImpl(socket, clientInfo, data);
		}
	}

	if (!isStarted())
	{
		IRSTD_LOG_INFO(IRSTD_TOPIC(IrStd, Server), "Aborting connection #" << index);
	}
}


template<class T>
int IrStd::Server<T>::getPort() const noexcept
{
	return m_port;
}

template<class T>
bool IrStd::Server<T>::isStarted() const noexcept
{
	return m_status == Status::STARTED;
}

template<class T>
bool IrStd::Server<T>::isStopped() const noexcept
{
	return m_status == Status::STOPPED;
}

template<class T>
void IrStd::Server<T>::waitUntilStarted(const uint64_t timeoutMs)
{
	m_event.waitForAtLeast(1, timeoutMs);
}

template<class T>
void IrStd::Server<T>::waitUntilStopped(const uint64_t timeoutMs)
{
	m_event.waitForAtLeast(3, timeoutMs);
}

template<class T>
void IrStd::Server<T>::start()
{
	IRSTD_ASSERT(IRSTD_TOPIC(IrStd, Server), m_status == Status::IDLE,
			"Server is not idle (status=" << static_cast<int>(m_status) << ")");

	auto sockFd = createSocket(nullptr, m_port, SocketLink::BIND);
	if (::listen(sockFd, m_backLog) == -1)
	{
		closeSocket(sockFd);
		IRSTD_THROW(IRSTD_TOPIC(IrStd, Server), "listen: code="
				<< errno << ", str=" << ::strerror(errno));
	}

	IRSTD_LOG_INFO(IRSTD_TOPIC(IrStd, Server), "Server started, waiting for connections on port " << m_port);

	// Notifiy that the sterver is started
	{
		IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Server), m_event.getCounter() == 0,
				"Server is already started or in an unknown state (state=" << m_event.getCounter() << ")");
		m_status = Status::STARTED;
		m_event.trigger();
	}

	while (isStarted())
	{
		struct ::sockaddr_storage clientAddr;
		::socklen_t sinSize = sizeof(clientAddr);

		const int clientFd = ::accept(sockFd, reinterpret_cast<struct ::sockaddr*>(&clientAddr), &sinSize);
		if (clientFd == -1)
		{
			IRSTD_LOG_ERROR(IRSTD_TOPIC(IrStd, Server), "accept: code=" << errno << ", str=" << ::strerror(errno));
			continue;
		}

		// Need to cleanup the completed tasks
		{
			m_manager.garbageCollection();
		}

		// If stopped request received, stop the server
		if (!isStarted())
		{
			closeSocket(clientFd);
			break;
		}

		// Register this file descriptor
		const auto index = m_manager.allocateClient(clientFd);
		if (index == m_manager.size())
		{
			closeSocket(clientFd);
			continue;
		}

		// Fill in the client structure
		{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
			auto& info = m_manager.getClientInfo(index);
			const static auto getInAddr = [](::sockaddr* sa) -> void* {
				if (sa->sa_family == AF_INET)
				{
					auto pSockaddr = reinterpret_cast<::sockaddr_in*>(sa);
					return &(pSockaddr->sin_addr);
				}
				auto pSockaddr = reinterpret_cast<::sockaddr_in6*>(sa);
				return &(pSockaddr->sin6_addr);
			};
#pragma GCC diagnostic pop
			{
				char ip[INET6_ADDRSTRLEN];
				if (::inet_ntop(clientAddr.ss_family, getInAddr(reinterpret_cast<::sockaddr*>(&clientAddr)),
						ip, sizeof(ip)) == nullptr)
				{
					IRSTD_LOG_ERROR(IRSTD_TOPIC(IrStd, Server), "inet_ntop: code="
							<< errno << ", str=" << ::strerror(errno));
					continue;
				}
				info.setIp(ip);
			}
		}

		m_manager.runTask(index, &IrStd::Server<T>::process, this, index);
	}

	closeSocket(sockFd);

	// Wait until all active connections are stopped
	m_manager.stop();

	m_status = Status::STOPPED;
	m_event.trigger();
	IRSTD_LOG_INFO(IRSTD_TOPIC(IrStd, Server), "Server stopped on port " << m_port);
}

template<class T>
void IrStd::Server<T>::stop()
{
	IRSTD_ASSERT(IRSTD_TOPIC(IrStd, Server), isStarted(),
			"Server is not running (status=" << static_cast<int>(m_status) << ")");
	IRSTD_LOG_TRACE(IRSTD_TOPIC(IrStd, Server), "Sending stop request to server on port "
			<< m_port);
	m_status = Status::STOPPING;
	m_event.trigger();

	try
	{
		auto sockFd = createSocket(nullptr, m_port, SocketLink::CONNECT);
		IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Server), sockFd >= 0, "Invalid socket");
		closeSocket(sockFd);
	}
	catch (const IrStd::Exception& e)
	{
		IRSTD_LOG_ERROR(IRSTD_TOPIC(IrStd, Server), "Ignoring as a workaround: "
				<< e);
		// Todo: need to remove this
	}
}

template<class T>
int IrStd::Server<T>::createSocket(const char* const pHostName, const uint16_t port, const SocketLink link)
{
	class ServerData
	{
	public:
		ServerData()
				: m_pServInfoList(nullptr)
				, m_socket(-1)
		{
		}
		~ServerData()
		{
			if (m_pServInfoList)
			{
				::freeaddrinfo(m_pServInfoList);
			}
			if (isValidSocket())
			{
				closeSocket(m_socket);
			}
		}

		bool isValidSocket() const noexcept
		{
			return m_socket != -1;
		}

		int getSocket() noexcept
		{
			int socket = m_socket;
			m_socket = -1;
			return socket;
		}

		struct ::addrinfo* m_pServInfoList;
		int m_socket;
	};

	struct ::addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (link == SocketLink::BIND)
	{
		hints.ai_flags = AI_PASSIVE;
	}

	ServerData serverData;
	{
		int rv;
		if ((rv = ::getaddrinfo(pHostName, Type::ShortString(port), &hints, &serverData.m_pServInfoList)) != 0)
		{
			IRSTD_THROW(IRSTD_TOPIC(IrStd, Server), "getaddrinfo(" << pHostName << ":" << port
					<< "): code=" << rv << ", str=" << ::gai_strerror(rv));
		}
	}

	// loop through all the results and use to the first valid
	struct ::addrinfo *pServInfo = serverData.m_pServInfoList;
	for (; pServInfo != nullptr; pServInfo = pServInfo->ai_next)
	{
		if ((serverData.m_socket = ::socket(pServInfo->ai_family, pServInfo->ai_socktype,
				pServInfo->ai_protocol)) == -1)
		{
			IRSTD_LOG_WARNING(IRSTD_TOPIC(IrStd, Server), "socket: code="
					<< errno << ", str=" << ::strerror(errno));
			continue;
		}

		if (link == SocketLink::BIND)
		{
			int yes = 1;
			if (::setsockopt(serverData.m_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1
					|| yes != 1)
			{
				IRSTD_THROW(IRSTD_TOPIC(IrStd, Server), "setsockopt: code="
						<< errno << ", str=" << ::strerror(errno));
			}
		}

		bool isError = false;
		switch (link)
		{

		case SocketLink::BIND:
			if (::bind(serverData.m_socket, pServInfo->ai_addr, pServInfo->ai_addrlen) == -1)
			{
				isError = true;
				IRSTD_LOG_ERROR(IRSTD_TOPIC(IrStd, Server), "Cannot bind to socket "
						<< serverData.m_socket << ", call=bind, code=" << errno
						<< ", str=" << ::strerror(errno));
			}
			break;

		// Todo: there is a bug where this function fails
		case SocketLink::CONNECT:
			if (::connect(serverData.m_socket, pServInfo->ai_addr, pServInfo->ai_addrlen) == -1)
			{
				isError = true;
			/*	IRSTD_LOG_ERROR(IRSTD_TOPIC(IrStd, Server), "Cannot connect to socket "
						<< serverData.m_socket << ", call=connect, code=" << errno
						<< ", str=" << ::strerror(errno));*/
			}
			break;

		default:
			IRSTD_UNREACHABLE(IRSTD_TOPIC(IrStd, Server));
		}

		if (isError)
		{
			closeSocket(serverData.m_socket);
			continue;
		}

		break;
	}

	// Ugly workaround to prevent an error message
	if (link == SocketLink::CONNECT && !pServInfo)
	{
		return -1;
	}

	IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Server), pServInfo, "Failed to bind/connect");
	IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Server), serverData.isValidSocket(),
			"Socket file descriptor is invalid");

	return serverData.getSocket();
}

// ---- IrStd::Server (stream) ------------------------------------------------

template<class T>
void IrStd::Server<T>::toStream(std::ostream& os) const
{
	os << "Server status=";
	switch (m_status)
	{
	case Status::IDLE:
		os << "IDLE";
		break;
	case Status::STARTED:
		os << "STARTED";
		break;
	case Status::STOPPING:
		os << "STOPPING";
		break;
	case Status::STOPPED:
		os << "STOPPED";
		break;
	default:
		IRSTD_UNREACHABLE(IRSTD_TOPIC(IrStd, Server));
	}
	os << std::endl;
	m_manager.toStream(os);
}

template<class T>
std::ostream& operator<<(std::ostream& os, const IrStd::Server<T>& server)
{
	server.toStream(os);
	return os;
}