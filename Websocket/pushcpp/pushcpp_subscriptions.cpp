#include "pushcpp_internal.h"

bool pushcpp::subscribe(
	const string &channel,
	ChannelEventHandler event,
	ChannelAuthHandler auth
)
{
	ChannelData d = m_channelData[channel];

	if (event != NULL)
		d.eventHandlers.push_back(event);

	if (auth != NULL)
		d.authHandler = auth;
	else
		d.authHandler = NULL;

	m_channelData[channel] = d;

	DEBUG("Subscribing to " << channel.c_str());

	if (connected())
		return sendSubscription(true, channel);
	else
		return false;
}

void pushcpp::unsubscribe(
	const std::string &channel
)
{
	m_channelData.erase(channel);

	DEBUG("Unsubscribing from " << channel.c_str());

	if (connected())
		sendSubscription(false, channel);
}

std::unordered_map<std::string, pushcpp::ChannelData> pushcpp::subscriptions(
	bool confirmedOnly
) const
{
	unordered_map<string, ChannelData> ret;

	for (
		auto it = m_channelData.begin();
		it != m_channelData.end();
		it++
	)
		if (!confirmedOnly || (confirmedOnly && it->second.subscribed))
			ret[it->first] = it->second;

	return ret;
}

bool pushcpp::sendSubscription(
	bool subscribe,
	const std::string &channel
)
{
	IrStd::Json json({
		{"event", subscribe ? "pusher:subscribe" : "pusher:unsubscribe"}
	});

	IrStd::Json data({
		{"channel", channel.c_str()}
	});

	if (subscribe) {
		auto chanData = m_channelData.find(channel);

		if (chanData != m_channelData.end() && chanData->second.authHandler != NULL) {
			assert(!this->m_socketId.empty());

			ChannelAuthentication authdata =
				chanData->second.authHandler(this->m_socketId, channel);

			string chdata = authdata.channelData;

			if (chdata == "")
				chdata = "{}";

			data.add("auth", authdata.auth.c_str());
			data.add("channel_data", chdata.c_str());
		}
	}

	json.add("data", data);
	return sendRaw(json.serialize());
}
