#pragma once

#include <memory>
#include <algorithm>
#include <ostream>

#include "../Assert.hpp"
#include "Decimal.hpp"

namespace IrStd
{
	class Json;
	namespace Type
	{
		/**
		 * Generic static object representation (similar to json)
		 * It is a lightweight data-interchange format. It is easy for humans to read and write.
		 * It is easy for machines to parse and generate.
		 *
		 * It supports array, maps, numbers, booleans and empty values.
		 */
		class Gson
		{
		public:
			class Map;
			enum class Type
			{
				EMPTY,
				BOOL,
				NUMBER,
				STRING,
				ARRAY,
				MAP
			};

			/**
			 * Empty array node
			 */
			Gson();

			/**
			 * Number node
			 */
			template<class T>
			Gson(const T number)
					: m_type(Type::NUMBER)
					, m_number(number)
			{
			}

			/**
			 * Boolean node
			 */
			Gson(const bool boolean);

			/**
			 * String node
			 */
			Gson(const std::string& string);
			Gson(const char* const pStr);

			/**
			 * Array node
			 */
			Gson(const std::initializer_list<Gson>& array);

			/**
			 * Map node
			 */
			Gson(const Map& map);

			/**
			 * Copy constructor
			 */
			Gson(const Gson& node);

			Gson& operator=(const Gson& node);

			Type getType() const noexcept
			{
				return m_type;
			}

			/**
			 * Merge a node with this one
			 */
			void merge(const Gson& node);

			void toStream(std::ostream& os) const;

			template <class ... Keys>
			const Gson& get(Keys&& ... keys) const noexcept
			{
				return get({std::forward<Keys>(keys)...});
			}

			const Gson& get(const std::initializer_list<std::string> keyList) const noexcept;

			template <class ... Keys>
			IrStd::Type::Decimal getNumber(Keys&& ... keys) const noexcept
			{
				const auto& node = get(std::forward<Keys>(keys)...);
				IRSTD_ASSERT(node.m_type == Type::NUMBER, "Node is of type " << static_cast<int>(node.m_type));
				return node.m_number;
			}

			template <class ... Keys>
			bool getBool(Keys&& ... keys) const noexcept
			{
				const auto& node = get(std::forward<Keys>(keys)...);
				IRSTD_ASSERT(node.m_type == Type::BOOL, "Node is of type " << static_cast<int>(node.m_type));
				return node.m_bool;
			}

			template <class ... Keys>
			const std::string& getString(Keys&& ... keys) const noexcept
			{
				const auto& node = get(std::forward<Keys>(keys)...);
				IRSTD_ASSERT(node.m_type == Type::STRING, "Node is of type " << static_cast<int>(node.m_type));
				return node.m_string;
			}

			template <class ... Keys>
			const std::vector<Gson>& getArray(Keys&& ... keys) const noexcept
			{
				const auto& node = get(std::forward<Keys>(keys)...);
				IRSTD_ASSERT(node.m_type == Type::ARRAY, "Node is of type " << static_cast<int>(node.m_type));
				return *node.m_array;
			}

			template <class ... Keys>
			const std::map<std::string, Gson>& getMap(Keys&& ... keys) const noexcept
			{
				const auto& node = get(std::forward<Keys>(keys)...);
				IRSTD_ASSERT(node.m_type == Type::MAP, "Node is of type " << static_cast<int>(node.m_type));
				return node.m_map->m_map;
			}

		private:
			Type m_type;
			union
			{
				bool m_bool;
				IrStd::Type::Decimal m_number;
			};
			std::string m_string;
			std::unique_ptr<std::vector<Gson>> m_array;
			std::unique_ptr<Map> m_map;

		public:
			class Map
			{
			public:
				Map(const std::initializer_list<std::pair<std::string, Gson>> map);

				template<class T>
				Map(const std::map<std::string, T> map)
				{
					m_map = map;
				}

			private:
				friend Gson;
				std::map<std::string, Gson> m_map;
			};
		};
	}
}

std::ostream& operator<<(std::ostream& os, const IrStd::Type::Gson& on);
