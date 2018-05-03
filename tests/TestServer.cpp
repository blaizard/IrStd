#include <thread>

#include "../Test.hpp"
#include "../IrStd.hpp"

//#define DEBUG 1

IRSTD_TOPIC_USE(IrStdServer);

class ServerTest : public IrStd::Test
{
public:
	void SetUp()
	{
		// Call parent function setup
		IrStd::Test::SetUp();

#if defined(DEBUG)
		// Enable specific traces
		IrStd::Logger::getDefault().addTopic(IrStd::Topic::IrStdServer);
#endif
	}

	template<class T>
	struct ServerData
	{
		std::unique_ptr<T> m_pServer;
		std::thread m_thread;

		ServerData(T* pServer)
				: m_pServer(pServer)
		{
		}

		ServerData(ServerData&& data)
				: m_pServer(std::move(data.m_pServer))
				, m_thread(std::move(data.m_thread))
		{
		}

		~ServerData()
		{
			// Don't send sigabrt
			if (m_pServer && m_pServer->isStarted())
			{
				m_pServer->stop();
				m_pServer->waitUntilStopped();
			}
			if (m_thread.joinable())
			{
				m_thread.join();
			}
		}

		T& getServer() noexcept
		{
			return *m_pServer;
		}

		void serverThreadFct()
		{
			try
			{
				m_pServer->start();
			}
			catch (IrStd::Exception& e)
			{
				std::cerr << "Exception: " << e.what() << std::endl;
			}
		}

		void start()
		{
			m_thread = std::thread(&ServerData::serverThreadFct, this);
			m_pServer->waitUntilStarted();
		}

		void stop()
		{
			ASSERT_TRUE(m_pServer);
			ASSERT_TRUE(m_pServer->isStarted()) << "Server is already stopped";
			m_pServer->stop();
			m_pServer->waitUntilStopped();
			ASSERT_TRUE(m_pServer->isStopped());
			ASSERT_TRUE(m_thread.joinable());
			m_thread.join();
		}
	};

	// Need to use a HTTP server for testing as we are using curl for requests
	class EchoHTTPServer : public IrStd::ServerHTTP
	{
	public:
		EchoHTTPServer(const int port, const size_t delayMs = 0)
				: IrStd::ServerHTTP(port, /*maxConnections*/20)
				, m_delayMs(delayMs)
		{
		}

		void handleResponse(Context& context) override
		{
			if (context.getRequest().getTotalDataSize())
			{
				context.getResponse().addHeader("Content-Type", "text/html");
				readData(context.getRequest(), [&](const char* pData, const size_t size) {
					context.getResponse().addData(pData, size);
				});
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(m_delayMs));
		}
	private:
		const size_t m_delayMs;
	};

	template<class T, class ... Args>
	ServerData<T> createServer(Args&& ... args)
	{
		ServerData<T> data{new T(std::forward<Args>(args)...)};
		return std::move(data);
	}

	void testConnectionMultiFctThread();
	void testTooManyConnectionsFctThread();
};

// ---- testStartStop ---------------------------------------------------------

TEST_F(ServerTest, testStartStop)
{
	for (int i=0; i<100; i++)
	{
		auto serverData = createServer<EchoHTTPServer>(/*port*/12345);
		serverData.start();
		serverData.stop();
	}
}

// ---- testConnection --------------------------------------------------------

TEST_F(ServerTest, testConnection)
{
	auto serverData = createServer<EchoHTTPServer>(/*port*/12345);
	serverData.start();

	// Send a connection
	for (int i=0; i<100; i++)
	{
		std::string data;
		IrStd::FetchUrl fetch("http://127.0.0.1:12345", data);
		fetch.addPost("HelloWorld");
		fetch.processSync();
		ASSERT_TRUE(data == "HelloWorld");
	}

	serverData.stop();
}

// ---- testConnectionMulti ---------------------------------------------------

void ServerTest::testConnectionMultiFctThread()
{
	for (int i=0; i<100; i++)
	{
		std::string data;
		IrStd::FetchUrl fetch("http://127.0.0.1:12345", data);
		fetch.addPost("HelloWorld");
		fetch.processSync();
		ASSERT_TRUE(data == "HelloWorld");
	}
}

TEST_F(ServerTest, testConnectionMulti)
{
	constexpr size_t NB_THREADS = 10;
	std::thread t[NB_THREADS];
	auto serverData = createServer<EchoHTTPServer>(/*port*/12345);
	serverData.start();

	for (size_t i=0; i<NB_THREADS; i++)
	{
		t[i] = std::thread(&ServerTest::testConnectionMultiFctThread, this);
	}

	// Print the current server status
	std::cout << *serverData.m_pServer << std::endl;

	for (size_t i=0; i<NB_THREADS; i++)
	{
		t[i].join();
	}

	serverData.stop();
}

// ---- testTooManyConnections ------------------------------------------------

void ServerTest::testTooManyConnectionsFctThread()
{
	std::string data;
	IrStd::FetchUrl fetch("http://127.0.0.1:12345", data);
	fetch.addPost("HelloWorld");
	try
	{
		fetch.processSync();
		ASSERT_TRUE(data == "HelloWorld");
	}
	catch (IrStd::Exception& /*e*/)
	{
		// Failure expected if too many connections
	}
}

TEST_F(ServerTest, testTooManyConnections)
{
	auto serverData = createServer<EchoHTTPServer>(/*port*/12345, /*delayMs*/100);
	serverData.start();

	constexpr size_t NB_THREADS = 100;
	std::thread t[NB_THREADS];

	for (size_t i=0; i<NB_THREADS; i++)
	{
		t[i] = std::thread(&ServerTest::testTooManyConnectionsFctThread, this);
	}

	// Print the current server status
	std::cout << *serverData.m_pServer << std::endl;

	for (size_t i=0; i<NB_THREADS; i++)
	{
		t[i].join();
	}

	serverData.stop();
}

// ---- testLargePacket -------------------------------------------------------

TEST_F(ServerTest, testLargePacket)
{
	auto serverData = createServer<EchoHTTPServer>(/*port*/12345);
	serverData.start();

	for (auto dataSize : {1024, 4096, 1024*1024, 4096*1024, 20*1024*1024})
	{
		std::string data;
		IrStd::FetchUrl fetch("http://127.0.0.1:12345", data);

		// Create a huge string
		std::string str(dataSize, 'a');
		fetch.addPost(str.c_str());
		fetch.processSync();

		ASSERT_TRUE(data.size() == str.size()) << "dataSize=" << dataSize
				<< ", data.size=" << data.size() << ", str.size=" << str.size();
	}

	serverData.stop();

	std::cout << *serverData.m_pServer << std::endl;
}

// ---- testDataSize ----------------------------------------------------------

TEST_F(ServerTest, testDataSize)
{
	constexpr size_t TOTAL_DATA_SIZE = 20*1024*1024;

	class DataSizeHTTPServer : public IrStd::ServerHTTP
	{
	public:
		DataSizeHTTPServer(const int port)
				: IrStd::ServerHTTP(port)
		{
		}

		void handleResponse(Context& context) override
		{
			ASSERT_TRUE(context.getRequest().getMethod() == IrStd::HTTPMethod::POST);
			ASSERT_TRUE(context.getRequest().getURI() == "/");

			ASSERT_TRUE(TOTAL_DATA_SIZE == context.getRequest().getTotalDataSize()) << "getTotalDataSize()="
					<< context.getRequest().getTotalDataSize() << ", TOTAL_DATA_SIZE=" << TOTAL_DATA_SIZE;

			// Read the prefetched data
			{
				size_t size = 0;
				context.getRequest().getData(size);
				ASSERT_TRUE(size <= 1024*1024) << "size=" << size;
				context.getRequest().clearData();
				context.getRequest().getData(size);
				ASSERT_TRUE(size == 0) << "size=" << size;
			}

			// Fetch 5MB in chunk of 512KB
			constexpr size_t SMALL_CHUNK = 512*1024;
			for (size_t fetchDataSize = 0; fetchDataSize < 5*1024*1024; fetchDataSize += SMALL_CHUNK)
			{
				const auto nbDataRead = fetchData(context.getRequest(), SMALL_CHUNK, /*overwriteData*/false);
				ASSERT_TRUE(nbDataRead == SMALL_CHUNK) << "nbDataRead=" << nbDataRead
						<< ", SMALL_CHUNK=" << SMALL_CHUNK;

				size_t size = 0;
				auto pData = context.getRequest().getData(size);
				ASSERT_TRUE(size == strlen(pData)) << "size=" << size
						<< ", strlen(pData)=" << strlen(pData);
				ASSERT_TRUE(size == fetchDataSize + SMALL_CHUNK) << "size=" << size
						<< ", fetchDataSize=" << fetchDataSize;
			}

			// Discard the rest
		}
	};

	auto serverData = createServer<DataSizeHTTPServer>(/*port*/12345);
	serverData.start();

	std::string data;
	IrStd::FetchUrl fetch("http://127.0.0.1:12345", data);

	// Create a huge string
	std::string str(TOTAL_DATA_SIZE, 'b');
	fetch.addPost(str.c_str());
	fetch.processSync();

	serverData.stop();
}

// ---- testServerREST --------------------------------------------------------

TEST_F(ServerTest, testServerREST)
{
	auto serverData = createServer<IrStd::ServerREST>(/*port*/12345);

	serverData.getServer().addRoute(IrStd::HTTPMethod::GET, "/api/v1/login/", [](IrStd::ServerREST::Context& context){
		context.getResponse().setData("ok:1");
	});
	serverData.getServer().addRoute(IrStd::HTTPMethod::GET, "/get/{INT}", [](IrStd::ServerREST::Context& context){
		auto& response = context.getResponse();
		response.setData("ok:2");

		// Add the string
		auto str = context.getMatchAsString(0);
		response.addData(" string=");
		response.addData(str.c_str());

		// Add the string
		auto integer = context.getMatchAsInt(0);
		response.addData(" integer=");
		response.addData(integer);
	});
	serverData.getServer().addRoute(IrStd::HTTPMethod::GET, "/get/up/{STRING}/go", [](IrStd::ServerREST::Context& context){
		auto& response = context.getResponse();
		response.setData("ok:3");

		// Add the string
		auto str = context.getMatchAsString(0);
		response.addData(" string=");
		response.addData(str.c_str());
	});

	serverData.getServer().dump(std::cout);
	serverData.start();

	// Ok and callback sent
	{
		std::string data;
		IrStd::FetchUrl fetch("http://127.0.0.1:12345/api/v1/login/", data);
		fetch.processSync();
		ASSERT_TRUE(data == "ok:1") << "data=" << data;
	}

	// Not found
	{
		std::string data;
		IrStd::FetchUrl fetch("http://127.0.0.1:12345/api/v1/login", data);
		try
		{
			fetch.processSync();
		}
		catch (const IrStd::Exception& e)
		{
			ASSERT_TRUE(data.empty());
			ASSERT_TRUE(validateOutput(e.what(), "404"));
		}
	}

	// Capture integer
	{
		std::string data;
		IrStd::FetchUrl fetch("http://127.0.0.1:12345/get/126", data);
		fetch.processSync();
		ASSERT_TRUE(validateOutput(data, "ok:2"));
		ASSERT_TRUE(validateOutput(data, "string=126"));
		ASSERT_TRUE(validateOutput(data, "integer=126"));
	}

	// Capture string
	{
		std::string data;
		IrStd::FetchUrl fetch("http://127.0.0.1:12345/get/up/this.is.just.a.test/go", data);
		fetch.processSync();
		ASSERT_TRUE(validateOutput(data, "ok:3"));
		ASSERT_TRUE(validateOutput(data, "string=this.is.just.a.test"));
	}

	serverData.stop();
}
