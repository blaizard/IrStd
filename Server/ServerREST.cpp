#include "ServerREST.hpp"

IRSTD_TOPIC_USE_ALIAS(IrStdServer, IrStd, Server);

// ---- IrStd::ServerREST::Context --------------------------------------------

constexpr size_t IrStd::ServerREST::Context::MAX_MATCHES; 

IrStd::ServerREST::Context::Context(IrStd::ServerHTTP::Context& context)
		: IrStd::ServerHTTP::Context(context.getResponse(), context.getRequest())
		, m_nbMatches(0)
{
}

IrStd::ServerREST::Context::Match::Match(
	const size_t begin,
	const size_t end,
	const Capture type)
		: m_begin(begin)
		, m_end(end)
		, m_type(type)
{
}

void IrStd::ServerREST::Context::addMatch(const size_t begin, const size_t end, const Capture type) noexcept
{
	IRSTD_ASSERT(m_nbMatches < MAX_MATCHES);
	const auto match = Match(begin, end, type);
	m_matchList[m_nbMatches] = match;
	m_nbMatches++;
}

int64_t IrStd::ServerREST::Context::getMatchAsInt(const size_t index) const
{
	const std::string str = getMatchAsString(index);
	return Type::Numeric<int64_t>::fromString(str.c_str());
}

uint64_t IrStd::ServerREST::Context::getMatchAsUInt(const size_t index) const
{
	const std::string str = getMatchAsString(index);
	return Type::Numeric<uint64_t>::fromString(str.c_str());
}

std::string IrStd::ServerREST::Context::getMatchAsString(const size_t index) const
{
	IRSTD_THROW_ASSERT(IrStdServer, index < m_nbMatches, "Index (" << index
			<< ") is out of bound (nbMatches=" << m_nbMatches << ")");
	const std::string& uri = getRequest().getURI();
	const auto& match = m_matchList[index];
	return uri.substr(match.m_begin, match.m_end - match.m_begin);
}

size_t IrStd::ServerREST::Context::getNbMatches() const noexcept
{
	return m_nbMatches;
}

// ---- IrStd::ServerREST -----------------------------------------------------

void IrStd::ServerREST::dump(std::ostream& os)
{
	os << *this;
	for (size_t i=0; i<m_nodeList.size(); i++)
	{
		os << "Routing map (" << i << "):" << std::endl;
		m_nodeList[i].dump(os, /*intialLevel*/1);
	}
}

void IrStd::ServerREST::handleResponse(IrStd::ServerHTTP::Context& context)
{
	const std::string& uri = context.getRequest().getURI();
	const auto method = context.getRequest().getMethod();
	const Node* pNode = &m_nodeList[static_cast<size_t>(method)];
	size_t index = 0;
	Context contextREST(context);

	while (index < uri.size() && pNode)
	{
		Capture capture = Capture::NONE;
		const auto result = pNode->getNextMatch(uri, index, capture);

		// If there is a capture
		if (capture != Capture::NONE)
		{
			IRSTD_ASSERT(result.first != 0);
			contextREST.addMatch(index, result.first, capture);
		}
		index = result.first;
		pNode = result.second;
	}

	const Callback* callback = nullptr;
	if (index == uri.size() && pNode && (callback = pNode->getEndPoint()))
	{
		(*callback)(contextREST);
	}
	else
	{
		routeNotFound(context);
	}
}

void IrStd::ServerREST::routeNotFound(IrStd::ServerHTTP::Context& context)
{
	context.getResponse().setStatus(404);
}

void IrStd::ServerREST::addRoute(const HTTPMethod method, const std::string uri, Callback callback)
{
	IRSTD_ASSERT(!isStarted(), "Cannot add routes when the server is running");
	IRSTD_ASSERT(static_cast<size_t>(method) < m_nodeList.size(), "Method is out of bound, got "
			<< static_cast<int>(method));

	size_t index = 0;
	size_t nbCapture = 0;
	Node* pNode = &m_nodeList[static_cast<size_t>(method)];

	// Consume the whole URI
	while (index < uri.size())
	{
		const auto pos = uri.find('{', index);

		// Register the begining of the string
		{
			const auto str = uri.substr(index, (pos == std::string::npos) ? std::string::npos : pos - index);
			ParamType param(str.c_str());
			pNode = pNode->add(param);
		}

		// If there is no capture preset
		if (pos == std::string::npos)
		{
			index = uri.size();
			break;
		}

		// Update the index
		index = pos + 1;

		const auto pos2 = uri.find('}', index);
		IRSTD_ASSERT(IrStdServer, pos2 != std::string::npos, "Incomplete capture preset starting at index "
				<< pos << " of \"" << uri << "\"");
		const auto preset = uri.substr(index, pos2 - index);

		// Make sure there is enough capture preset
		{
			IRSTD_ASSERT(IrStdServer, nbCapture < Context::MAX_MATCHES, "Too many capture for this URI ("
					<< __FUNCTION__ << "), max=" << Context::MAX_MATCHES)
			nbCapture++;
		}

		if (preset == "INT")
		{
			pNode = pNode->add(Capture::INT);
		}
		else if (preset == "UINT")
		{
			pNode = pNode->add(Capture::UINT);
		}
		else if (preset == "STRING")
		{
			pNode = pNode->add(Capture::STRING);
		}
		else
		{
			IRSTD_CRASH(IrStdServer, "Unknown capture preset \"" << preset << "\" from \"" << uri << "\"");
		}

		// Update the index
		index = pos2 + 1;
	}

	IRSTD_ASSERT(IrStdServer, index == uri.size(), "The URI (" << uri << ") was not fully consumed");

	// Save the callback
	pNode->add(ParamType(callback));
}

// ---- IrStd::ServerREST::ParamType ------------------------------------------

IrStd::ServerREST::ParamType::ParamType(const char c)
		: m_data(c)
		, m_type(DataType::CHARACTER)
		, m_hash(c)
{
}

IrStd::ServerREST::ParamType::ParamType(const Capture capture)
		: m_data(capture)
		, m_type(DataType::CAPTURE)
		, m_hash(static_cast<std::size_t>(capture))
{
}

IrStd::ServerREST::ParamType::ParamType(const char* const pStr)
		: m_data(pStr)
		, m_type(DataType::STRING)
		, m_hash(std::hash<std::string>()(pStr))
{
}

bool IrStd::ServerREST::ParamType::isEndPoint() const noexcept
{
	return m_type == DataType::ENDPOINT;
}

bool IrStd::ServerREST::ParamType::isCapture() const noexcept
{
	return m_type == DataType::CAPTURE;
}

size_t IrStd::ServerREST::ParamType::getHash() const noexcept
{
	return m_hash;
}

bool IrStd::ServerREST::ParamType::operator==(const ParamType& other) const
{
	if (m_type != other.m_type)
	{
		return false;
	}
	switch (m_type)
	{
	case DataType::CHARACTER:
		return m_data.m_char == other.m_data.m_char;
	case DataType::CAPTURE:
		return m_data.m_capture == other.m_data.m_capture;
	// There can be only one endpoint per map
	case DataType::ENDPOINT:
		return true;
	case DataType::STRING:
		return std::strcmp(m_data.m_pStr, other.m_data.m_pStr) == 0;
	default:
		IRSTD_UNREACHABLE(IrStdServer);
	}
}

void IrStd::ServerREST::ParamType::toStream(std::ostream& os) const
{
	switch (m_type)
	{
	case DataType::CHARACTER:
		os << m_data.m_char;
		break;
	case DataType::CAPTURE:
		os << "Capture (" << static_cast<int>(m_data.m_capture) << ")";
		break;
	case DataType::ENDPOINT:
		os << "ENDPOINT";
		break;
	case DataType::STRING:
		os << "String (" << m_data.m_pStr << ")";
		break;
	default:
		IRSTD_UNREACHABLE(IrStdServer);
	}
}

std::ostream& operator<<(std::ostream& os, const IrStd::ServerREST::ParamType& param)
{
	param.toStream(os);
	return os;
}

// ---- IrStd::ServerREST::Node -----------------------------------------------

IrStd::ServerREST::Node::Node()
{
}

IrStd::ServerREST::Node* IrStd::ServerREST::Node::add(const ParamType& key)
{
	Node* pNode = this;
	auto it = m_map.find(key);
	if (it == m_map.end())
	{
		if (key.m_type == ParamType::DataType::STRING)
		{
			const char *pStr = key.m_data.m_pStr;
			while (*pStr)
			{
				pNode = pNode->add(ParamType(*pStr));
				pStr++;
			}
		}
		else
		{
			auto pair = std::make_pair(key, std::unique_ptr<Node>());
			if (key.m_type != ParamType::DataType::ENDPOINT)
			{
				pair.second = std::unique_ptr<Node>(new Node());
			}
			const auto it2 = m_map.insert(std::move(pair));
			IRSTD_THROW_ASSERT(IrStdServer, it2.second, "Cannot insert the key into the map");
			pNode = it2.first->second.get();
		}
	}
	else
	{
		pNode = it->second.get();
	}

	return pNode;
}

void IrStd::ServerREST::Node::dump(std::ostream& os, const size_t intialLevel)
{
	const std::function<void(const Node*, const size_t)> printNode =
			[&](const Node* pNode, const size_t level) {
		for (auto& it : pNode->m_map)
		{
			os << std::string(level * 2, ' ');
			if (level != intialLevel)
			{
				os << "|- ";
			}

			auto pParam = &it.first;
			auto pNextNode = it.second.get();

			os << *pParam;

			// Try to print all consecutive characters (if single)
			while (pNextNode
					&& pParam->m_type == ParamType::DataType::CHARACTER
					&& pNextNode->m_map.size() == 1
					&& pNextNode->m_map.begin()->first.m_type == ParamType::DataType::CHARACTER)
			{
				const auto& map = pNextNode->m_map;
				pParam = &map.begin()->first;
				pNextNode = map.begin()->second.get();
				os << *pParam;
			}

			os << std::endl;

			if (pNextNode)
			{
				printNode(pNextNode, level + 1);
			}
		}
	};

	printNode(this, intialLevel);
}

const IrStd::ServerREST::Callback* IrStd::ServerREST::Node::getEndPoint() const noexcept
{
	static const auto paramFct([](Context&){});
	const auto it = m_map.find(paramFct);
	return (it == m_map.end()) ? nullptr : &it->first.m_data.m_fct;
}

std::pair<const size_t, const IrStd::ServerREST::Node*> IrStd::ServerREST::Node::getNextMatch(
		const std::string& str,
		const size_t index,
		Capture& capture) const
{
	IRSTD_ASSERT(IrStdServer, index < str.size(), "index=" << index << ", str.size=" << str.size());

	// Look if the character is registered
	{
		const ParamType param(str.at(index));
		const auto it = m_map.find(param);
		if (it != m_map.end())
		{
			return {index + 1, it->second.get()};
		}
	}

	// Look for INT capture preset
	{
		bool isValid = false;
		static const auto paramNumber(Capture::INT);
		const auto it = m_map.find(paramNumber);
		if (it != m_map.end())
		{
			size_t endIndex = index;
			// Sign + or -
			if (str.at(endIndex) == '-' || str.at(endIndex) == '+')
			{
				endIndex++;
			}
			// Must be number without coma
			while (endIndex < str.size()
					&& (str.at(endIndex) >= '0' && str.at(endIndex) <= '9'))
			{
				isValid = true;
				endIndex++;
			}
			// If an integer has been found
			if (isValid)
			{
				IRSTD_ASSERT(IrStdServer, index < endIndex, "index=" << index << ", endIndex=" << endIndex);
				capture = Capture::INT;
				return {endIndex, it->second.get()};
			}
		}
	}

	// Look for UINT capture preset
	{
		bool isValid = false;
		static const auto paramNumber(Capture::UINT);
		const auto it = m_map.find(paramNumber);
		if (it != m_map.end())
		{
			size_t endIndex = index;
			// Must be number without coma
			while (endIndex < str.size()
					&& (str.at(endIndex) >= '0' && str.at(endIndex) <= '9'))
			{
				isValid = true;
				endIndex++;
			}
			// If an integer has been found
			if (isValid)
			{
				IRSTD_ASSERT(IrStdServer, index < endIndex, "index=" << index << ", endIndex=" << endIndex);
				capture = Capture::UINT;
				return {endIndex, it->second.get()};
			}
		}
	}

	// Look for STRING capture preset
	{
		static const auto paramNumber(Capture::STRING);
		const auto it = m_map.find(paramNumber);
		if (it != m_map.end())
		{
			// Load the next map (there must be one)
			const auto pNode = it->second.get();
			IRSTD_ASSERT(IrStdServer, pNode, "Next node after a STRING cannot be empty");
			size_t endIndex = index;

			do
			{
				Capture unusedCapture;
				const auto ret = pNode->getNextMatch(str, endIndex, unusedCapture);
				// If there is a match, break
				if (ret.first != 0)
				{
					break;
				}
				endIndex++;
			} while (endIndex < str.size());

			// Return the matching string
			{
				capture = Capture::STRING;
				return {endIndex, it->second.get()};
			}
		}
	}

	return {0, nullptr};
}
