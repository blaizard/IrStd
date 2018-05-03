#pragma once

#include <functional>
#include <string>
#include <memory>

namespace IrStd
{
	namespace Websocket
	{
		class Pusher
		{
		public:
			Pusher(const std::string& key);

			void subscribe(const std::string& channel, const std::function<void(const std::string&, const std::string&)>& callback);
			void connect();
			void disconnect();

			class Impl
			{
			};
		private:
			std::unique_ptr<Impl> m_pImpl;
		};
	}
}
