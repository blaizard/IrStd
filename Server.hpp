#pragma once

#include <arpa/inet.h>
#include <vector>
#include <mutex>
#include <thread>

#include "Event.hpp"
#include "Server/ConnectionManager.hpp"

namespace IrStd
{
	class ClientInfo
	{
	public:
		explicit ClientInfo(const int socket);

		const char* getIp() const noexcept;
		void setIp(const char* const ip) noexcept;
		void toStream(std::ostream& os) const;
		int getSocket() const noexcept;
		std::string& getDataStr() noexcept;

	private:
		int m_socket;
		char m_ip[INET6_ADDRSTRLEN];
		std::string m_data;
	};
}
std::ostream& operator<<(std::ostream& os, const IrStd::ClientInfo& info);

namespace IrStd
{
	template <class T = ClientInfo>
	class Server
	{
	public:
		Server(const int port = 8080, const size_t maxConnections = 10,
				const int backLog = 128);

		/**
		 * Start the server
		 */
		void start();

		/**
		 * Stop the server
		 */
		void stop();

		/**
		 * This function will wait until the server is ready
		 * to receive incoming connections
		 */
		void waitUntilStarted(const uint64_t timeoutMs = 0);

		/**
		 * This function will wait until the server is stopped
		 */
		void waitUntilStopped(const uint64_t timeoutMs = 0);

		/**
		 * Return true if the server is started
		 */
		bool isStarted() const noexcept;
		bool isStopped() const noexcept;

		void toStream(std::ostream& os) const;

		/**
		 * Receive data from a socket. Append the data to the string passed into argument.
		 *
		 * \param socket The socket to receive the data from
		 * \param data The string used to append the data
		 * \param maxSize The maximum of data to receive
		 *
		 * \return The amount of data received
		 */
		size_t receive(const int socket, std::string& data, const size_t maxSize = RECEIVE_MAX_SIZE);

		/**
		 * Wait to receive a fixed amount of data
		 */
		size_t receiveSpecificAmount(const int socket, std::string& data, const size_t size);

	protected:
		enum class ProcessStatus
		{
			CLOSE_CONNECTION = 0,
			EXPECT_NEW_PACKET,
			CONTINUE_CURRENT_PACKET
		};

		/**
		 * Process an incoming connection
		 */
		virtual ProcessStatus processImpl(const int socket, T& info,
				std::string& data) = 0;

	private:
		enum class Status
		{
			IDLE = 0,
			STARTED,
			STOPPING,
			STOPPED
		};

		static constexpr size_t CLIENT_TIMEOUT_MS = 10000; // 10s
		static constexpr size_t RECEIVE_MAX_SIZE = 1024 * 1024; // 1MB

		IrStd::ServerImpl::ConnectionManager<T> m_manager;
		const uint16_t m_port;
		const uint16_t m_backLog;
		Event m_event;
		Status m_status;

		/**
		 * Process an incoming connection
		 */
		void process(const size_t index);

		/**
		 * Close a socket
		 */
		static void closeSocket(const int socket) noexcept;

		enum class SocketLink
		{
			CONNECT,
			BIND
		};
		int createSocket(const char* const pHostName, const uint16_t port, const SocketLink link);
	};
}

// Template definition
#include "Server/ServerTemplate.hpp"
