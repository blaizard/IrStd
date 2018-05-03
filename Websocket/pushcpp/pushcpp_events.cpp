#include "pushcpp_internal.h"

void pushcpp::WS_Dispatch(const string & message)
{
	DEBUG("<- " << message.c_str());

	const IrStd::Json json(message.c_str());

	const string event = json.getString("event").val();
	const string channel = json.isString("channel") ? json.getString("channel").val() : "";
	const string sdata = json.isString("data") ? json.getString("data").val() : "";

	if (event == "pusher:connection_established") {
		const IrStd::Json jdata(sdata.c_str());

		this->m_socketId = jdata.getString("socket_id").val();
		DEBUG("our socket id is: " << this->m_socketId.c_str());

		// Re/subscribe to all our channels!
		for (
			auto it = m_channelData.begin();
			it != m_channelData.end();
			it++
		) {
			sendSubscription(true, it->first);
		}

		if (m_connectionEventHandler)
			m_connectionEventHandler(ConnectionEvent::CONNECTED);
		return;
	}

	// { event, channel, data:str => {
	// 	ids:array[str], hash:hash=>{id=>user_data..}, count:int }}
	if (event == "pusher_internal:subscription_succeeded") {
		m_channelData[channel].subscribed = true;

		const IrStd::Json jdata(sdata.c_str());
		if (jdata.isArray("presence", "ids"))
		{
			const auto& arr = jdata.getArray("presence", "ids");
			for (int i = 0; i < arr.size(); i++)
				m_channelData[channel].presenceMemberIds.insert(
					arr.getString(i).val()
				);
		}

		// return;
	}

	// { event, channel, data:str => { user_id:str, user_info: hashornull }}
	if (event == "pusher_internal:member_added") {
		const IrStd::Json jdata(sdata.c_str());

		if (m_channelData.find(channel) != m_channelData.end())
			m_channelData[channel].presenceMemberIds.insert(
				jdata.getString("user_id").val()
			);

		// return;
	}

	// { event, channel, data:str => { user_id: str }}
	if (event == "pusher_internal:member_removed") {
		const IrStd::Json jdata(sdata.c_str());

		if (m_channelData.find(channel) != m_channelData.end())
			m_channelData[channel].presenceMemberIds.erase(
				jdata.getString("user_id").val()
			);

		// return;
	}

	if (event == "pusher:ping") {
		send("", "pusher:pong", "");
		return;
	}

	if (event == "pusher:error") {
		if (m_errorEventHandler) {
			int code = json.isNumber("data", "code") ? json.getNumber("data", "code").val() : 0;
			string message = json.getString("data", "message").val();
			m_errorEventHandler(code, message);
		}

		return;
	}

	bool is_pusher = !event.compare(0, 7, "pusher:", 0, 7);
	bool is_pusher_internal = !event.compare(0, 16, "pusher_internal:", 0, 16);

	if (
		// Optional: Don't show pusher_internal messages to callbacks.
		// !is_pusher && !is_pusher_internal &&
		json.is("channel")
	) {
		string data = "";

		if (json.isString("data"))
			data = json.getString("data").val();
		else {
			// We .. just re-encode as json here since we can't pass jansson
			// objects to the client. Boo.
			data = json.serialize();
		}

		auto it = m_channelData.find(channel);

		if (it != m_channelData.end())
			for (
				auto it2 = it->second.eventHandlers.begin();
				it2 != it->second.eventHandlers.end();
				it2++
			)
				(*it2)(channel, event, data);
	}
}
