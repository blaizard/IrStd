#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <initializer_list>

#include "Exception.hpp"
#include "Topic.hpp"
#include "Assert.hpp"
#include "Type/ShortString.hpp"
#include "Type/Gson.hpp"

IRSTD_TOPIC_USE(IrStd, Json);

#define RAPIDJSON_ASSERT(condition) IRSTD_THROW_ASSERT(IRSTD_TOPIC(IrStd, Json), condition, #condition)
#define RAPIDJSON_HAS_STDSTRING	1

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wstrict-overflow"
#include "rapidjson/document.h"
#pragma GCC diagnostic pop

#define IRSTD_JSON_TYPE_IS_HELPER(typeName) \
		template <class... A> \
		bool IRSTD_PASTE(is, typeName)(A&&... a) const noexcept \
		{ \
			return is<IrStd::Json::typeName>(std::forward<A>(a)...); \
		}

#define IRSTD_JSON_TYPE_GET_HELPER(typeName) \
		template <class... A> \
		IrStd::Json::typeName& IRSTD_PASTE(get, typeName)(A&&... a) \
		{ \
			return get<IrStd::Json::typeName>(std::forward<A>(a)...); \
		} \
		template <class... A> \
		const IrStd::Json::typeName& IRSTD_PASTE(get, typeName)(A&&... a) const \
		{ \
			return get<const IrStd::Json::typeName>(std::forward<A>(a)...); \
		}

#define IRSTD_JSON_TYPE_HELPER(typeName) \
		template <class... A> \
		bool IRSTD_PASTE(is, typeName)(A&&... a) const noexcept \
		{ \
			return is<IrStd::Json::typeName>(std::forward<A>(a)...); \
		} \
		template <class... A> \
		IrStd::Json::typeName& IRSTD_PASTE(get, typeName)(A&&... a) \
		{ \
			return get<IrStd::Json::typeName>(std::forward<A>(a)...); \
		} \
		template <class... A> \
		const IrStd::Json::typeName& IRSTD_PASTE(get, typeName)(A&&... a) const \
		{ \
			return get<const IrStd::Json::typeName>(std::forward<A>(a)...); \
		}

/**
 * \defgroup IrStd-Json
 * \brief Json decoder and encoder, based on RapidJson.
 * \ingroup IrStd
 */

namespace IrStd
{
	/**
	 * \ingroup IrStd-Json
	 */
	class Json
	{
	public:

		/**
		 * \name Constructors
		 * \{
		 */

		/**
		 * \brief Construct an empty Json object
		 * \ingroup IrStd-Json
		 */
		Json();

		/**
		 * \brief Construct a Json object from a string
		 * \ingroup IrStd-Json
		 *
		 * \param pStr Json serialized string
		 */
		explicit Json(const char* const pStr);

		/**
		 * \brief Construct a Json object from a \ref IrStd::Gson object,
		 * This gives the possiblity to create an Json object in a human readable way.
		 * \ingroup IrStd-Json
		 *
		 * \param gson Gson map object
		 */
		explicit Json(const Type::Gson::Map& gson);

		/**
		 * \brief Delete the copy constructor for performance concerns
		 */
		Json(const Json& json) = delete;

		/**
		 * \brief Move constructor
		 */
		Json(Json&& json);

		/**
		 * \}
		 */

		/**
		 * \brief Print a human readable representation of the json object
		 * (for debugging purpose)
		 * \ingroup IrStd-Json
		 *
		 * \param out The stream where to print the object
		 */
		void toStream(std::ostream& out) const;

		/**
		 * \brief Serialize the Json object into a string
		 * \ingroup IrStd-Json
		 *
		 * \return A string containing the serialized Json
		 */
		std::string serialize() const;

		/**
		 * \brief Clear the content of the Json object
		 * \ingroup IrStd-Json
		 */
		void clear();

		// Forward declaration
		class Object;
		class Array;
		class String;
		class Number;
		class Bool;
		class Null;

		class Type : public rapidjson::Value
		{
		public:
			static constexpr const char* toString() noexcept
			{
				return "Type";
			}

			template <class T, class... A>
			bool is(A&&... a) const noexcept
			{
				const rapidjson::Value* pElt = checkAndGet(this, {std::forward<A>(a)...});
				return (!pElt || !static_cast<const T*>(pElt)->is()) ? false : true;
			}

			/**
			 * \brief Access an element of supported data types
			 *
			 * \param [...] Optional item names (const char*)
			 */
			template <class T, class... A>
			T& get(A&&... a)
			{
				rapidjson::Value* pElt = checkAndGet(this, {std::forward<A>(a)...});
				if (!pElt)
				{
					IRSTD_THROW(IRSTD_TOPIC(IrStd, Json), "Cannot access element '" << pElt << "'");
				}
				if (!static_cast<T*>(pElt)->is())
				{
					IRSTD_THROW(IRSTD_TOPIC(IrStd, Json), "Element '" << pElt << "' is not of type '"
							<< T::toString() << "'");
				}
				return static_cast<T&>(*pElt);
			}

			template <class T, class... A>
			const T& get(A&&... a) const
			{
				const rapidjson::Value* pElt = checkAndGet(this, {std::forward<A>(a)...});
				if (!pElt)
				{
					IRSTD_THROW(IRSTD_TOPIC(IrStd, Json), "Cannot access element '" << pElt << "'");
				}
				if (!static_cast<const T*>(pElt)->is())
				{
					IRSTD_THROW(IRSTD_TOPIC(IrStd, Json), "Element '" << pElt << "' is not of type '"
							<< T::toString() << "'");
				}
				return static_cast<const T&>(*pElt);
			}

			IRSTD_JSON_TYPE_HELPER(Object);
			IRSTD_JSON_TYPE_HELPER(Array);
			IRSTD_JSON_TYPE_HELPER(String);
			IRSTD_JSON_TYPE_HELPER(Number);
			IRSTD_JSON_TYPE_HELPER(Bool);
			IRSTD_JSON_TYPE_HELPER(Null);
		};

		// Type describing an object
		class Object : public Type
		{
		public:
			/**
			 * \brief Assess if the instance is of type \ref Object
			 */
			bool is() const noexcept;

			/**
			 * \brief Return the a pointer to a null terminated string containing
			 * the type of the object
			 */
			static constexpr const char* toString() noexcept
			{
				return "Object";
			}

			// Iterator
			class Iterator : public rapidjson::Value::MemberIterator
			{
			public:
				Iterator(const rapidjson::Value::MemberIterator& it, rapidjson::Value* val);
				std::pair<const char* const, Type&> operator*();
			private:
				rapidjson::Value* m_val;
			};

			/**
			 * \brief Begin iterator
			 */
			Iterator begin();

			/**
			 * \brief End iterator
			 */
			Iterator end();

			// Const Iterator
			class ConstIterator : public rapidjson::Value::ConstMemberIterator
			{
			public:
				ConstIterator(const rapidjson::Value::ConstMemberIterator& it, const rapidjson::Value* val);
				std::pair<const char* const, const Type&> operator*();
			private:
				const rapidjson::Value* const m_val;
			};


			/**
			 * \brief Begin constant iterator
			 */
			ConstIterator begin() const;

			/**
			 * \brief End constant iterator
			 */
			ConstIterator end() const;
		};

		// Type describing an array
		class Array : public Type
		{
		public:
			bool is() const noexcept;
			static constexpr const char* toString() noexcept
			{
				return "Array";
			}
			size_t size() const noexcept;

			/**
			 * \brief Add a new member to the json object
			 */
			template <class T>
			void add(Json& json, T&& value)
			{
				json.addToArray(*this, json.m_document.GetAllocator(), std::forward<T>(value));
			}

		};

		// Type describing a string
		class String : public Type
		{
		public:
			bool is() const noexcept;
			const char* val() const noexcept;
			void val(Json& json, const char* const pStr) noexcept
			{
				val(json, pStr, std::strlen(pStr));
			}
			void val(Json& json, const std::string& str) noexcept
			{
				val(json, str.c_str(), str.size());
			}
			void val(Json& json, const char* const pStr, const size_t length) noexcept;
			size_t size() const noexcept;
			bool empty() const noexcept;
			static constexpr const char* toString() noexcept
			{
				return "String";
			}
			operator const char*() const noexcept
			{
				return GetString();
			}
		};

		// Type describing a number
		class Number : public Type
		{
		public:
			bool is() const noexcept;
			double val() const noexcept;
			void val(const double n) noexcept;
			static constexpr const char* toString() noexcept
			{
				return "Number";
			}
			operator double() const noexcept
			{
				return GetDouble();
			}
		};

		// Type describing a boolean
		class Bool : public Type
		{
		public:
			bool is() const noexcept;
			bool val() const noexcept;
			void val(const bool value) noexcept;
			static constexpr const char* toString() noexcept
			{
				return "Bool";
			}
			operator bool() const noexcept
			{
				return GetBool();
			}
		};

		// Type describing an empty value
		class Null : public Type
		{
		public:
			bool is() const noexcept;
			static constexpr const char* toString() noexcept
			{
				return "Null";
			}
		};

		/**
		 * \brief Merge a json object into this one.
		 * \ingroup IrStd-Json
		 *
		 * \param json The Json object to be merged.
		 */
		void merge(const Json& json);

		/**
		 * \brief Assess if a member exists
		 * \ingroup IrStd-Json
		 *
		 * \param ... List of keys to reach the element
		 *
		 * \return true if the element exists, false otherwise
		 */
		template <class... A>
		bool is(A&&... a) const noexcept
		{
			const rapidjson::Value* pElt = checkAndGet({std::forward<A>(a)...});
			return (pElt) ? true : false;
		}

		/**
		 * \brief Add a new member to the json object
		 */
		template <class T>
		void add(const IrStd::Type::ShortString key, T&& value)
		{
			rapidjson::Value* pElt = const_cast<rapidjson::Value*>(static_cast<const rapidjson::Value*>(&m_document));
			addToObject(*pElt, m_document.GetAllocator(), key, std::forward<T>(value));
		}

		/**
		 * \brief Add a new empty member to the json object
		 */
		void add(const IrStd::Type::ShortString key)
		{
			add(key, rapidjson::Value());
		}

		/**
		 * \name Accessors
		 * Set of functions to access Json elements. The functions are made to be easy to use,
		 * fast and intuitive. Accessing an element is of complexity O(1).\n
		 * Here is an example of usage, which asserts that a specific element element
		 * is a number
		 * \code
		 * const IrStd::Json json("{\"hello\": {\"world\": 2}}");
		 * assert(json.isNumber("hello", "world"));
		 * \endcode
		 * \{
		 */
		/**
		 * \brief Check that the element of type \ref Object exists
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_IS_HELPER(Object);
		/// \}
		/**
		 * \brief Retrieve the element of type \ref Object
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_GET_HELPER(Object);
		/// \}

		/**
		 * \brief Check that the element of type \ref Array exists
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_IS_HELPER(Array);
		/// \}
		/**
		 * \brief Retrieve the element of type \ref Array
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_GET_HELPER(Array);
		/// \}

		/**
		 * \brief Check that the element of type \ref String exists
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_IS_HELPER(String);
		/// \}
		/**
		 * \brief Retrieve the element of type \ref String
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_GET_HELPER(String);
		/// \}

		/**
		 * \brief Check that the element of type \ref Number exists
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_IS_HELPER(Number);
		/// \}
		/**
		 * \brief Retrieve the element of type \ref Number
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_GET_HELPER(Number);
		/// \}

		/**
		 * \brief Check that the element of type \ref Bool exists
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_IS_HELPER(Bool);
		/// \}
		/**
		 * \brief Retrieve the element of type \ref Bool
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_GET_HELPER(Bool);
		/// \}

		/**
		 * \brief Check that the element of type \ref Null exists
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_IS_HELPER(Null);
		/// \}
		/**
		 * \brief Retrieve the element of type \ref Null
		 * \ingroup IrStd-Json
		 * \{
		 */
		IRSTD_JSON_TYPE_GET_HELPER(Null);
		/// \}

		/// \}

	private:
		/**
		 * \brief Merge elt2 into elt1. Note that elt2 will be
		 * moved into elt1 if necessary.
		 */
		void merge(rapidjson::Value& elt1, const rapidjson::Value& elt2);

		/**
		 * \brief Assess if a member exists and is of a certain type
		 *
		 * \param ... List of keys to reach the element
		 *
		 * \return true if the element exists, false otherwise
		 */
		template <class T, class... A>
		bool is(A&&... a) const noexcept
		{
			const rapidjson::Value* pElt = checkAndGet({std::forward<A>(a)...});
			return (!pElt || !static_cast<const T*>(pElt)->is()) ? false : true;
		}

		/*
		 * Get an element of a specific type
		 */
		template <class T, class... A>
		T& get(A&&... a)
		{
			rapidjson::Value* pElt = checkAndGet({std::forward<A>(a)...});
			if (!pElt)
			{
				IRSTD_THROW(IRSTD_TOPIC(IrStd, Json), "Cannot access element '" << pElt << "'");
			}
			if (!static_cast<T*>(pElt)->is())
			{
				IRSTD_THROW(IRSTD_TOPIC(IrStd, Json), "Element '" << pElt << "' is not of type " << T::toString());
			}
			return static_cast<T&>(*pElt);
		}

		/*
		 * Get an element of a specific type
		 */
		template <class T, class... A>
		const T& get(A&&... a) const
		{
			const rapidjson::Value* pElt = checkAndGet({std::forward<A>(a)...});
			if (!pElt)
			{
				IRSTD_THROW(IRSTD_TOPIC(IrStd, Json), "Cannot access element '" << pElt << "'");
			}
			if (!static_cast<const T*>(pElt)->is())
			{
				IRSTD_THROW(IRSTD_TOPIC(IrStd, Json), "Element '" << pElt << "' is not of type " << T::toString());
			}
			return static_cast<const T&>(*pElt);
		}

		/*
		 * Return the element or nullporter if it does not exists
		 */
		static rapidjson::Value* checkAndGet(rapidjson::Value* pElt, const int index) noexcept;
		static rapidjson::Value* checkAndGet(rapidjson::Value* pElt, const size_t index) noexcept;
		static rapidjson::Value* checkAndGet(rapidjson::Value* pElt, std::initializer_list<const char* const>&& attrList) noexcept;
		static const rapidjson::Value* checkAndGet(const rapidjson::Value* pElt, const int index) noexcept;
		static const rapidjson::Value* checkAndGet(const rapidjson::Value* pElt, const size_t index) noexcept;
		static const rapidjson::Value* checkAndGet(const rapidjson::Value* pElt, std::initializer_list<const char* const>&& attrList) noexcept;
		rapidjson::Value* checkAndGet(std::initializer_list<const char* const>&& attrList) noexcept;
		const rapidjson::Value* checkAndGet(std::initializer_list<const char* const>&& attrList) const noexcept;

		/**
		 * Generic template
		 */
		template <class T>
		void create(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& /*alloc*/, T&& value)
		{
			jsonValue = rapidjson::Value(std::forward<T>(value));
		}

		/**
		 * Support for hard coded strings
		 */
		void create(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& alloc, const char* const value);

		/**
		 * Support for strings
		 */
		void create(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& alloc, std::string value);

		/**
		 * Support for generic static object
		 */
		void create(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& alloc, const IrStd::Type::Gson& node);

		/**
		 * Support for json object
		 */
		void create(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& alloc, const Json& json);
		void create(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& alloc, Json& json);

		/**
		 * Support arrays
		 */
		template <class T, size_t N>
		void create(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& alloc, const std::array<T, N> array)
		{
			jsonValue.SetArray();
			for (const auto& item : array)
			{
				addToArray(jsonValue, alloc, item);
			}
		}

		/**
		 * Support for vectors
		 */
		template <class T>
		void create(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& alloc, const std::vector<T> vector)
		{
			jsonValue.SetArray();
			for (const auto& item : vector)
			{
				addToArray(jsonValue, alloc, item);
			}
		}

		/**
		 * Support for lists
		 */
		template <class T>
		void create(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& alloc, const std::list<T> list)
		{
			jsonValue.SetArray();
			for (const auto& item : list)
			{
				addToArray(jsonValue, alloc, item);
			}
		}

		/**
		 * Support for sets
		 */
		template <class T>
		void create(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& alloc, const std::set<T> set)
		{
			jsonValue.SetArray();
			for (const auto& item : set)
			{
				addToArray(jsonValue, alloc, item);
			}
		}

		/**
		 * Support for maps
		 */
		template <class K, class T>
		void create(rapidjson::Value& jsonValue, rapidjson::Document::AllocatorType& alloc, const std::map<K, T> map)
		{
			jsonValue.SetObject();
			for (const auto& it : map)
			{
				std::string keyStr(it.first);
				addToObject(jsonValue, alloc, keyStr.c_str(), it.second);
			}
		}

		template <class T>
		void addToObject(rapidjson::Value& valueContainer, rapidjson::Document::AllocatorType& alloc, const char* const pKey, T&& value)
		{
			rapidjson::Value key(pKey, m_document.GetAllocator());
			rapidjson::Value jsonValue;
			create(jsonValue, alloc, std::forward<T>(value));
			valueContainer.AddMember(key, jsonValue, alloc);
		}

		template <class T>
		void addToArray(rapidjson::Value& valueContainer, rapidjson::Document::AllocatorType& alloc, T&& value)
		{
			rapidjson::Value jsonValue;
			create(jsonValue, alloc, std::forward<T>(value));
			valueContainer.PushBack(jsonValue, alloc);
		}

		rapidjson::Document m_document;
	};
}
