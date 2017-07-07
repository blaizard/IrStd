#pragma once

#include <cxxabi.h>

#include "Allocator.hpp"

/**
 * \brief Returns 1 if the argument is empty, 0 ortherwise
 */
#define IRSTD_IS_EMPTY(...) IRSTD_IS_EMPTY_X(1, 0, __VA_ARGS__)
/**
 * \brief Returns tvalue if the argument is empty, fvalue otherwise
 */
#define IRSTD_IS_EMPTY_X(tvalue, fvalue, ...) _IRSTD_IS_EMPTY_X(tvalue, fvalue, __VA_ARGS__)
#define _IRSTD_IS_EMPTY_X(tvalue, fvalue, first, ...) __IRSTD_IS_EMPTY_X(tvalue, fvalue, _IRSTD_IS_EMPTY_PROBE first ())
#define __IRSTD_IS_EMPTY_X(tvalue, fvalue, ...) ___IRSTD_IS_EMPTY_X(__VA_ARGS__, tvalue, fvalue)
#define ___IRSTD_IS_EMPTY_X(_0, _1, _2, ...) _2
#define _IRSTD_IS_EMPTY_PROBE(...) ,

/**
 * \brief Add quotes to the argument
 */
#define IRSTD_QUOTE(x) _IRSTD_Q(x)
#define _IRSTD_Q(x) #x

/**
 * \brief Count the number of arguments passed to the macro
 */
#define IRSTD_NARGS(...) _IRSTD_NARGS(__VA_ARGS__, _IRSTD_NARGS_SEQ_N(__VA_ARGS__))
#define _IRSTD_NARGS(...) __IRSTD_NARGS_N(__VA_ARGS__)
#define __IRSTD_NARGS_N( \
	_1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
	_11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
	_21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
	_31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
	_41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
	_51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
	_61,_62,_63,N,...) N
#define _IRSTD_NARGS_SEQ_N(x, ...) \
	63,62,61,60,                   \
	59,58,57,56,55,54,53,52,51,50, \
	49,48,47,46,45,44,43,42,41,40, \
	39,38,37,36,35,34,33,32,31,30, \
	29,28,27,26,25,24,23,22,21,20, \
	19,18,17,16,15,14,13,12,11,10, \
	9,8,7,6,5,4,3,2,IRSTD_IS_EMPTY_X(0, 1, x),0

/**
 * \brief Call a specific macro based on the number of arguments.
 */
#define IRSTD_GET_MACRO(macro, ...) _IRSTD_GET_MACRO(macro, IRSTD_NARGS(__VA_ARGS__))
#define _IRSTD_GET_MACRO(macro, nbArgs) __IRSTD_GET_MACRO(macro, nbArgs)
#define __IRSTD_GET_MACRO(macro, nbArgs) macro##nbArgs

/**
 * \brief Paste macro names together
 */
#define IRSTD_PASTE(...) IRSTD_GET_MACRO(_IRSTD_PASTE, __VA_ARGS__)(__VA_ARGS__)
#define _IRSTD_PASTE2(a, b) a ## b
#define _IRSTD_PASTE3(a, b, c) a ## b ## c
#define _IRSTD_PASTE4(a, b, c, d) a ## b ## c ## d
#define _IRSTD_PASTE5(a, b, c, d, e) a ## b ## c ## d ## e

/**
 * \brief Check if a variable is of a certain type
 */
#define IRSTD_TYPEOF(variable, ...) \
	(IRSTD_GET_MACRO(_IRSTD_TYPEOF, __VA_ARGS__)(variable, __VA_ARGS__))
#define _IRSTD_TYPEOF1(variable, refType1) std::is_same<decltype(variable), refType1>::value
#define _IRSTD_TYPEOF2(variable, refType1, ...) _IRSTD_TYPEOF1(variable, refType1) || _IRSTD_TYPEOF1(variable, __VA_ARGS__)
#define _IRSTD_TYPEOF3(variable, refType1, ...) _IRSTD_TYPEOF1(variable, refType1) || _IRSTD_TYPEOF2(variable, __VA_ARGS__)
#define _IRSTD_TYPEOF4(variable, refType1, ...) _IRSTD_TYPEOF1(variable, refType1) || _IRSTD_TYPEOF3(variable, __VA_ARGS__)
#define _IRSTD_TYPEOF5(variable, refType1, ...) _IRSTD_TYPEOF1(variable, refType1) || _IRSTD_TYPEOF4(variable, __VA_ARGS__)
#define _IRSTD_TYPEOF6(variable, refType1, ...) _IRSTD_TYPEOF1(variable, refType1) || _IRSTD_TYPEOF5(variable, __VA_ARGS__)
#define _IRSTD_TYPEOF7(variable, refType1, ...) _IRSTD_TYPEOF1(variable, refType1) || _IRSTD_TYPEOF6(variable, __VA_ARGS__)
#define _IRSTD_TYPEOF8(variable, refType1, ...) _IRSTD_TYPEOF1(variable, refType1) || _IRSTD_TYPEOF7(variable, __VA_ARGS__)

/**
 * \brief Usefull function for debugging
 */
#define IRSTD_DEBUG_TYPE(variable) getVariableType(variable)

namespace IrStd
{
	bool almostEqual(const double a, const double b) noexcept;

	template<class T>
	std::string getVariableType(const T& variable)
	{
		char* const name = abi::__cxa_demangle(typeid(variable).name(), 0, 0, NULL);
		const std::string typeStr(name);
		free(name);
		return typeStr;
	}

	template<typename A, typename T>
	class UniquePtr : public std::unique_ptr<T, std::function<void(T*)>>, public AllocatorImpl<A>
	{
	public:
		UniquePtr(T* ptr)
				: std::unique_ptr<T, std::function<void(T*)>>{static_cast<T*>(ptr), deleter}
		{
		}

	private:
		static void deleter(T* ptr)
		{
			IrStd::AllocatorObj<T, A> alloc;
			alloc.destroy(ptr);
			alloc.deallocate(ptr);
		}
	};

	/**
	 * Implementation of makeUnique
	 */
	template<typename A, typename T, typename... Args>
	UniquePtr<A, T> makeUnique(Args&&... args)
	{
		IrStd::AllocatorObj<T, A> alloc;
		T* ptr = alloc.allocate(sizeof(T));
		alloc.construct(ptr, std::forward<Args>(args)...);
		UniquePtr<A, T> pUnique{static_cast<T*>(ptr)};
		return std::move(pUnique);
	}

	/**
	 * Singleton class pattern
	 */
	template<class T>
	class SingletonImpl
	{
	public:
		static T& getInstance()
		{
			static T instance;
			return instance;
		}
		SingletonImpl(SingletonImpl const&) = delete;
		void operator=(SingletonImpl const&) = delete;
	protected:
		SingletonImpl() = default;
	};

	/**
	 * Singleton class pattern with living scope
	 */
	template<class T>
	class SingletonScopeImpl : public SingletonImpl<T>
	{
	public:
		SingletonScopeImpl()
		{
			m_isCreated = true;
		}
		~SingletonScopeImpl()
		{
			m_isCreated = false;
		}
		static bool isAlive()
		{
			return m_isCreated;
		}
	private:
		static bool m_isCreated;
	};
	template <class T>
	bool SingletonScopeImpl<T>::m_isCreated = false;

	/**
	 * Direct string to stream class
	 */
	class StringStream : public std::streambuf
	{
	public:
		explicit StringStream(std::string &str)
				: m_str(str)
				, m_out(this)
		{
		}

		template<typename T>
		StringStream& operator<<(const T& in)
		{
			m_out << in;
			return *this;
		}

	private:
		typedef typename std::char_traits<char_type> traits;
		int_type overflow(int_type c)
		{

			if (traits::eq_int_type(c, traits::eof()))
			{
				return traits::not_eof(c);
			}

			const char_type ch = traits::to_char_type(c);
			m_str.push_back(ch);
			return c;
		}

		std::streamsize xsputn(const char* s, std::streamsize n)
		{
			m_str.insert(m_str.end(), s, s + n);                                                                                 
			return n;
		}

		std::string& m_str;
		std::ostream m_out;
	};
}
