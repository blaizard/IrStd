#pragma once

#include <iostream>
#include <initializer_list>

#include "Exception.hpp"
#include "Topic.hpp"
#include "Assert.hpp"

IRSTD_TOPIC_USE(IrStdJson);

#define RAPIDJSON_ASSERT(condition) IRSTD_THROW_ASSERT(IrStd::Topic::IrStdJson, condition, #condition)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wstrict-overflow"		
#include "rapidjson/document.h"
#pragma GCC diagnostic pop

#define IRSTD_JSON_TYPE_HELPER(typeName) \
		template <typename... A> \
		bool IRSTD_PASTE(is, typeName)(A&&... a) \
		{ \
			return is<IrStd::Json::typeName>(std::forward<A>(a)...); \
		} \
		template <typename... A> \
		IrStd::Json::typeName& IRSTD_PASTE(get, typeName)(A&&... a) \
		{ \
			return get<IrStd::Json::typeName>(std::forward<A>(a)...); \
		}

namespace IrStd
{
	class Json
	{
	public:
		Json(const char* const str);
		void toStream(std::ostream& out) const;

		/**
		 * \brief Data types for a Json object
		 * \{
		 */
		// Forward declaration
		class Object;
		class Array;
		class String;
		class Number;
		class Bool;
		class Null;
		/// \}

		class Type : public rapidjson::Value
		{
		public:
			static constexpr const char* toString() noexcept
			{
				return "Type";
			}

			template <class T, typename... A>
			bool is(A&&... a)
			{
				rapidjson::Value* pElt = checkAndGet(this, {std::forward<A>(a)...});
				return (!pElt || !static_cast<T*>(pElt)->is()) ? false : true;
			}

			/**
			 * \brief Access an element of supported data types
			 *
			 * \param [...] Optional item names (const char*)
			 */
			template <class T, typename... A>
			T& get(A&&... a)
			{
				rapidjson::Value* pElt = checkAndGet(this, {std::forward<A>(a)...});
				if (!pElt)
				{
					IRSTD_THROW(IrStd::Topic::IrStdJson, "Cannot access element '" << pElt << "'");
				}
				if (!static_cast<T*>(pElt)->is())
				{
					IRSTD_THROW(IrStd::Topic::IrStdJson, "Element '" << pElt << "' is not of type '" << T::toString() << "'");
				}
				return static_cast<T&>(*pElt);
			}



			IRSTD_JSON_TYPE_HELPER(Object);
			IRSTD_JSON_TYPE_HELPER(Array);
			IRSTD_JSON_TYPE_HELPER(String);
			IRSTD_JSON_TYPE_HELPER(Number);
			IRSTD_JSON_TYPE_HELPER(Bool);
			IRSTD_JSON_TYPE_HELPER(Null);
		};

		/**
		 * Implement the various types
		 */
		class Object : public Type
		{
		public:
			bool is() const noexcept;

			/**
			 * Iterator
			 *
			 * \{
			 */
			class Iterator : public rapidjson::Value::MemberIterator
			{
			public:
				Iterator(const rapidjson::Value::MemberIterator& it, rapidjson::Value* val);
				std::pair<const char* const, Type&> operator*();
			private:
				rapidjson::Value* m_val;
			};

			Iterator begin();
			Iterator end();
			/// \}

			/**
			 * Const Iterator
			 *
			 * \{
			 */
			class ConstIterator : public rapidjson::Value::ConstMemberIterator
			{
			public:
				ConstIterator(const rapidjson::Value::ConstMemberIterator& it, const rapidjson::Value* val);
				std::pair<const char* const, const Type&> operator*();
			private:
				const rapidjson::Value* const m_val;
			};

			ConstIterator begin() const;
			ConstIterator end() const;
			/// \}
		};

		class Array : public Type
		{
		public:
			bool is() const noexcept;
			static constexpr const char* toString() noexcept
			{
				return "Array";
			}
		};

		class String : public Type
		{
		public:
			bool is() const noexcept;
			const char* val() const noexcept;
			void val(const char* const str, const size_t length) noexcept;
			size_t len() const noexcept;
			static constexpr const char* toString() noexcept
			{
				return "String";
			}
		};

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
		};

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
		};

		class Null : public Type
		{
		public:
			bool is() const noexcept;
			static constexpr const char* toString() noexcept
			{
				return "Null";
			}
		};

		template <class T, typename... A>
		bool is(A&&... a)
		{
			rapidjson::Value* pElt = checkAndGet({std::forward<A>(a)...});
			return (!pElt || !static_cast<T*>(pElt)->is()) ? false : true;
		}

		template <class T, typename... A>
		T& get(A&&... a)
		{
			rapidjson::Value* pElt = checkAndGet({std::forward<A>(a)...});
			if (!pElt)
			{
				IRSTD_THROW(IrStd::Topic::IrStdJson, "Cannot access element '" << pElt << "'");
			}
			if (!static_cast<T*>(pElt)->is())
			{
				IRSTD_THROW(IrStd::Topic::IrStdJson, "Element '" << pElt << "' is not of type " << T::toString());
			}
			return static_cast<T&>(*pElt);
		}

		IRSTD_JSON_TYPE_HELPER(Object);
		IRSTD_JSON_TYPE_HELPER(Array);
		IRSTD_JSON_TYPE_HELPER(String);
		IRSTD_JSON_TYPE_HELPER(Number);
		IRSTD_JSON_TYPE_HELPER(Bool);
		IRSTD_JSON_TYPE_HELPER(Null);

	private:
		/**
		 * Return the element or nullporter if it does not exists
		 */
		static rapidjson::Value* checkAndGet(rapidjson::Value* pElt, std::initializer_list<const char* const>&& attrList) noexcept;
		rapidjson::Value* checkAndGet(std::initializer_list<const char* const>&& attrList) const noexcept;

		rapidjson::Document m_document;
	};
}
