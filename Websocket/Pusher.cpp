#include "Pusher.hpp"
#include "pushcpp/pushcpp.h"

#include "../Assert.hpp"
#include "../Topic.hpp"

IRSTD_TOPIC_REGISTER(IrStd, Websocket);
IRSTD_TOPIC_USE_ALIAS(IrStdWebsocket, IrStd, Websocket);

namespace
{
	class PusherImplPushcpp : public IrStd::Websocket::Pusher::Impl
	{
	public:
		PusherImplPushcpp(const std::string& key)
				: m_pp(key, [this](const pushcpp::ConnectionEvent ev) {
					IRSTD_LOG_DEBUG(IrStdWebsocket, "ConnectEvent: " << static_cast<int>(ev));
					m_pp.send("channel", "pusher:subscribe", "lol");
				}, er_ev)
		{
		}

		void subscribe(
				const std::string& channel,
				const std::function<void(const std::string&, const std::string&)>& callback)
		{
			m_pp.subscribe(channel, [=](const std::string& c, const std::string& event, const std::string& data) {
				IRSTD_ASSERT(IrStdWebsocket, c == channel, "c=" << c << ", channel=" << channel);
				callback(event, data);
			});
		}

		void connect()
		{
			m_pp.connect();
			m_pp.join();
		}

		void disconnect()
		{
			m_pp.disconnect(/*wait*/false);
		}

		static void er_ev(const int code, const std::string &msg)
		{
			IRSTD_LOG_ERROR(IrStdWebsocket, "Error: code#" << code << ", "  << msg);
		}

	private:
		pushcpp m_pp;
	};
}

IrStd::Websocket::Pusher::Pusher(const std::string& key)
		: m_pImpl(new ::PusherImplPushcpp(key))
{
}

void IrStd::Websocket::Pusher::subscribe(
		const std::string& channel,
		const std::function<void(const std::string&, const std::string&)>& callback)
{
	IRSTD_ASSERT(m_pImpl);
	static_cast<::PusherImplPushcpp*>(m_pImpl.get())->subscribe(channel, callback);
}

void IrStd::Websocket::Pusher::connect()
{
	IRSTD_ASSERT(m_pImpl);
	static_cast<::PusherImplPushcpp*>(m_pImpl.get())->connect();
}

void IrStd::Websocket::Pusher::disconnect()
{
	IRSTD_ASSERT(m_pImpl);
	static_cast<::PusherImplPushcpp*>(m_pImpl.get())->disconnect();
}
