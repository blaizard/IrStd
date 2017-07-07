#pragma once

#include <unordered_map>
#include <memory>
#include <array>

#include "ServerHTTP.hpp"

namespace IrStd
{
	class ServerREST : public ServerHTTP
	{
	public:
		template<class ... Args>
		ServerREST(Args&& ... args)
				: ServerHTTP(std::forward<Args>(args)...)
		{
		}

		enum class Capture
		{
			NONE,
			INTEGER,
			STRING
		};

		class Context : public IrStd::ServerHTTP::Context
		{
		public:
			Context(IrStd::ServerHTTP::Context& context);

			/**
			 * Maximum of matches supported per URI
			 */
			static constexpr size_t MAX_MATCHES = 10;

			/**
			 * Return the match at a specific index and returns its integer interpretation
			 */
			int64_t getMatchAsInteger(const size_t index) const;

			/**
			 * Return the match at a specific index and returns its string interpretation
			 */
			std::string getMatchAsString(const size_t index) const;

			/**
			 * Return the number of matches
			 */
			size_t getNbMatches() const noexcept;

		private:
			friend ServerREST;

			/**
			 * Add a match element to the array
			 */
			void addMatch(const size_t begin, const size_t end, const Capture type) noexcept;

			/**
			 * Matching array
			 * Use a fixed size to make sure there is no alloc (speed)
			 * \{
			 */
			struct Match
			{
				Match() = default;
				Match(const size_t begin, const size_t end, const Capture type);
				size_t m_begin;
				size_t m_end;
				Capture m_type;
			};
			std::array<Match, MAX_MATCHES> m_matchList;
			size_t m_nbMatches;
			/// \}
		};

		typedef std::function<void(Context&)> Callback;

		/**
		 * Create and register a route. Route are REST endpoints
		 * that are mapped to a callback.
		 *
		 * The URI string can match some arguments, to set them, use
		 * the following notation:
		 * - "/uri/test/{INTEGER}/match"
		 * - "/uri/test/{STRING}/match"
		 */
		void addRoute(const std::string uri, Callback callback);

		virtual void handleResponse(IrStd::ServerHTTP::Context& context) override;

		/**
		 * Dump infomration about the server
		 */
		void dump(std::ostream& os);

	private:
		class Node;
	public:
		class ParamType
		{
		public:
			template<class F>
			ParamType(F callback)
					: m_data(callback)
					, m_type(DataType::ENDPOINT)
					, m_hash(0)
			{
			}

			ParamType(const char* const pStr);
			ParamType(const char c);
			ParamType(const Capture capture);

			bool isEndPoint() const noexcept;
			bool isCapture() const noexcept;

			size_t getHash() const noexcept;

			bool operator==(const ParamType& other) const;

			void toStream(std::ostream& os) const;

		private:
			friend Node;

			class Data
			{
			public:
				template<class F>
				Data(F callback)
						: m_fct(callback)
						, m_pStr(nullptr)
				{
				}

				Data(const char c)
						: m_char(c)
				{
				}

				Data(const char* pStr)
						: m_pStr(pStr)
				{
				}

				Data(Capture catpure)
						: m_capture(catpure)
				{
				}

				const Callback m_fct;
				union
				{
					const char* m_pStr;
					const char m_char;
					const Capture m_capture;
				};
			};

			enum class DataType
			{
				CHARACTER,
				CAPTURE,
				ENDPOINT,
				STRING
			};

			const Data m_data;
			const DataType m_type;
			const std::size_t m_hash;
		};

	private:

		// The mapping object entry point
		class Node
		{
		public:
			Node();

			/**
			 * Add the new key and return the next node.
			 * If the key already exists, simply returns the next node.
			 */
			Node* add(const ParamType& key);

			/**
			 * Check if there is a match with the string passed into argument
			 * If not returns {0,nullptr}
			 */
			std::pair<const size_t, const Node*> getNextMatch(const std::string& str,
					const size_t index, Capture& capture) const;

			/**
			 * Get the endpoint if any, otherwise returns nullptr
			 */
			const Callback* getEndPoint() const noexcept;

			void dump(std::ostream& os, const size_t intialLevel = 0);

		private:
			class Hasher
			{
			public:
				size_t operator()(const ParamType& key) const
				{
					return key.getHash();
				}
			};
			std::unordered_map<ParamType, std::unique_ptr<Node>, Hasher> m_map;
		};
		Node m_node;
	};
}

std::ostream& operator<<(std::ostream& os, const IrStd::ServerREST::ParamType& param);
