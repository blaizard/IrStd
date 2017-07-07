#include <unistd.h>

#include "../Assert.hpp"
#include "../Topic.hpp"

IRSTD_TOPIC_USE(IrStdServer);

namespace IrStd
{
	namespace ServerImpl
	{
		enum class Status
		{
			FREE = 0,
			IDLE,
			RECEIVING,
			RUNNING,
			TERMINATED
		};

		template<class T>
		class ConnectionManager
		{
		public:

			class Client
			{
			public:
				explicit Client(const int socket);
				Client(const Client&) = delete;
				// Move assignment
				Client(Client&& client);
				Client& operator=(Client&& client);

				Status m_status;
				std::thread m_thread;
				T m_info;
			};

			ConnectionManager(const size_t maxConnections);

			size_t allocateClient(const int socket) noexcept;

			void setStatus(const size_t index, const Status status) noexcept;

			/**
			 * Cleanup the the connection list
			 */
			void garbageCollection();

			/**
			 * Stop all current active connections
			 */
			void stop();

			/**
			 * Retrieve the socket from the client
			 */
			int getSocket(const size_t index) const noexcept;

			size_t size() const noexcept;

			template<class ... Args>
			void runTask(const size_t index, Args&& ... args)
			{
				IRSTD_ASSERT(isValidIndex(index));
				m_connections[index].m_thread = std::thread(std::forward<Args>(args)...);
			}

			/**
			 * Retrieve the client information
			 */
			T& getClientInfo(const size_t index) noexcept;

			void toStream(std::ostream& os) const;

			size_t getIndex(const int socket) const noexcept;

		private:
			size_t getFreeClientNoLock() const noexcept;
			bool isValidIndex(const size_t index) const noexcept;
			void deallocateClient(const size_t index) noexcept;

			const size_t m_maxConnections;
			size_t m_activeConnections;
			std::vector<Client> m_connections;
			mutable std::mutex m_lock;
			::fd_set m_fdMaster;
		};
	}
}



// ---- IrStd::ServerImpl::ConnectionManager<T>::Client -------------------------

template<class T>
IrStd::ServerImpl::ConnectionManager<T>::Client::Client(const int socket)
		: m_status(Status::FREE)
		, m_info(socket)
{
}

template<class T>
IrStd::ServerImpl::ConnectionManager<T>::Client::Client(Client&& client)
		: m_info(-1)
{
	*this = std::move(client);
}

template<class T>
typename IrStd::ServerImpl::ConnectionManager<T>::Client&
		IrStd::ServerImpl::ConnectionManager<T>::Client::operator=(typename IrStd::ServerImpl::ConnectionManager<T>::Client&& client)
{
	m_status = client.m_status;
	m_thread = std::move(client.m_thread);
	m_info = std::move(client.m_info);
	return *this;
}

// ---- IrStd::ServerImpl::ConnectionManager<T> ---------------------------------

template<class T>
IrStd::ServerImpl::ConnectionManager<T>::ConnectionManager(
		const size_t maxConnections)
		: m_maxConnections(maxConnections)
		, m_activeConnections(0)
{
	// Pre-allocate memory for the array and initialize
	for (size_t i=0; i<m_maxConnections; i++)
	{
		m_connections.push_back(std::move(Client(-1)));
	}
}

template<class T>
size_t IrStd::ServerImpl::ConnectionManager<T>::getFreeClientNoLock() const noexcept
{
	size_t i = 0;
	for (; i<m_connections.size(); i++)
	{
		if (m_connections[i].m_status == Status::FREE)
		{
			IRSTD_ASSERT(m_activeConnections < m_connections.size());
			break;
		}
	}
	IRSTD_ASSERT(i < m_connections.size() || m_activeConnections == m_connections.size());
	return i;
}

template<class T>
size_t IrStd::ServerImpl::ConnectionManager<T>::getIndex(const int socket) const noexcept
{
	size_t i = 0;
	for (; i<m_connections.size(); i++)
	{
		if (m_connections[i].m_status != Status::FREE
				&& m_connections[i].m_info.getSocket() == socket)
		{
			break;
		}
	}
	return i;
}

template<class T>
size_t IrStd::ServerImpl::ConnectionManager<T>::allocateClient(const int socket) noexcept
{
	size_t index = m_connections.size();
	// Register the connection
	{
		std::lock_guard<std::mutex> lock(m_lock);
		index = getFreeClientNoLock();
		// Make sure it is not reaching the connection limit
		if (index == m_connections.size())
		{
			IRSTD_LOG_WARNING(IrStd::Topic::IrStdServer, "The maximum number of connection ("
					<< m_maxConnections<< ") has been reached");
			return m_connections.size();
		}
		// Make sure this socket is not already registered
		IRSTD_ASSERT(IrStd::Topic::IrStdServer, getIndex(socket) == m_connections.size(),
				"Socket (" << socket << ") is already registered in connection #"
				<< getIndex(socket));
		// Register the client
		m_connections[index] = Client(socket);
		// Set the status of the client to IDLE
		m_connections[index].m_status = Status::IDLE;
		m_activeConnections++;
	}

	IRSTD_ASSERT(isValidIndex(index));
	IRSTD_LOG_TRACE(IrStd::Topic::IrStdServer, "Registered socket " << socket
			<< " with connection #" << index);

	return index;
}

template<class T>
size_t IrStd::ServerImpl::ConnectionManager<T>::size() const noexcept
{
	return m_connections.size();
}

template<class T>
void IrStd::ServerImpl::ConnectionManager<T>::deallocateClient(const size_t index) noexcept
{
	IRSTD_ASSERT(isValidIndex(index));
	IRSTD_LOG_TRACE(IrStd::Topic::IrStdServer, "Unregistering socket "
			<< m_connections[index].m_info.getSocket() << " with connection #" << index);
	IRSTD_ASSERT(m_connections[index].m_status != Status::FREE);
	IRSTD_ASSERT(m_connections[index].m_thread.joinable());

	// Wait until the thread completes
	m_connections[index].m_thread.join();
	{
		std::lock_guard<std::mutex> lock(m_lock);
		m_connections[index].m_status = Status::FREE;
		m_activeConnections--;
	}
}

template<class T>
T& IrStd::ServerImpl::ConnectionManager<T>::getClientInfo(const size_t index) noexcept
{
	std::lock_guard<std::mutex> lock(m_lock);
	IRSTD_ASSERT(isValidIndex(index));
	return m_connections[index].m_info;
}

template<class T>
int IrStd::ServerImpl::ConnectionManager<T>::getSocket(const size_t index) const noexcept
{
	std::lock_guard<std::mutex> lock(m_lock);
	IRSTD_ASSERT(isValidIndex(index));
	return m_connections[index].m_info.getSocket();
}

/*template<class T>
std::string& IrStd::ServerImpl::ConnectionManager<T>::getData(const size_t index) noexcept
{
	std::lock_guard<std::mutex> lock(m_lock);
	IRSTD_ASSERT(isValidIndex(index));
	return m_connections[index].m_data;
}*/

template<class T>
bool IrStd::ServerImpl::ConnectionManager<T>::isValidIndex(const size_t index) const noexcept
{
	IRSTD_ASSERT(IrStd::Topic::IrStdServer, index < m_connections.size(), "Index (" << index
			<< ") is out of range");
	IRSTD_ASSERT(IrStd::Topic::IrStdServer, m_connections[index].m_status != Status::FREE,
			"The status of the entry cannot be FREE");
	return true;
}

template<class T>
void IrStd::ServerImpl::ConnectionManager<T>::toStream(std::ostream& os) const
{
	std::lock_guard<std::mutex> lock(m_lock);
	os << "Open connection(s): " << m_activeConnections << "/"
			<< m_connections.size() << std::endl;
	// Calculate the total memory consumption
/*	{
		size_t totalMemory = 0;
		for (size_t i=0; i<m_connections.size(); i++)
		{
			totalMemory += m_connections[i].m_data.capacity();
		}
		os << "Memory consumption: " << totalMemory << std::endl;
	}*/
	for (size_t i=0; i<m_connections.size(); i++)
	{
		const auto& client = m_connections[i];
		// Show only active connections
		if (client.m_status == Status::FREE)
		{
			continue;
		}
		os << "\t#" << i << " status=";
		switch (client.m_status)
		{
		case Status::FREE:
			os << "FREE";
			break;
		case Status::IDLE:
			os << "IDLE";
			break;
		case Status::RECEIVING:
			os << "RECEIVING";
			break;
		case Status::RUNNING:
			os << "RUNNING";
			break;
		case Status::TERMINATED:
			os << "TERMINATED";
			break;
		default:
			IRSTD_UNREACHABLE(IrStd::Topic::IrStdServer);
		}
		os << " (" << client.m_info << ")" << std::endl;
	}
}

template<class T>
void IrStd::ServerImpl::ConnectionManager<T>::setStatus(const size_t index, const Status status) noexcept
{
	std::lock_guard<std::mutex> lock(m_lock);
	IRSTD_ASSERT(isValidIndex(index));
	m_connections[index].m_status = status;
}

template<class T>
void IrStd::ServerImpl::ConnectionManager<T>::garbageCollection()
{
	for (size_t i=0; i<m_connections.size(); i++)
	{
		// Remove only entries that can be terminated
		if (m_connections[i].m_status == Status::TERMINATED)
		{
			deallocateClient(i);
		}
	}
}

template<class T>
void IrStd::ServerImpl::ConnectionManager<T>::stop()
{
	IRSTD_LOG_TRACE(IrStd::Topic::IrStdServer, "Stopping remaining connections");
	for (size_t i=0; i<m_connections.size(); i++)
	{
		if (m_connections[i].m_status != Status::FREE)
		{
			deallocateClient(i);
		}
	}
}
