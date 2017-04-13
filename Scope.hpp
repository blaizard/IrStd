/**
 * A scope is a local object that has a global visibility uppon its lifetime.
 */

#pragma once

#include <mutex>
#include <atomic>

/**
 * \brief Register a global scope with an ID.
 * 
 * The scope is meant to be available globaly and can be referenced by its id.
 *
 * \param id The unique id to be given to this scope.
 */
#define IRSTD_SCOPE_REGISTER(id) _IRSTD_SCOPE_REGISTER(static, , id)

/**
 * \brief Register a thread local scope with an ID.
 * 
 * The scope is meant to be available globaly and can be referenced by its id.
 *
 * \param id The unique id to be given to this scope.
 */
#define IRSTD_SCOPE_THREAD_REGISTER(id) _IRSTD_SCOPE_REGISTER(static thread_local, Thread, id)

#define _IRSTD_SCOPE_REGISTER(attr, prefix, id) \
	namespace IrStd \
	{ \
		IrStd::Scope::Flag& _IRSTD_SCOPE_NAME(prefix, id)() noexcept \
		{ \
			attr IrStd::Scope::Flag scopeFlag = ATOMIC_FLAG_INIT; \
			return scopeFlag; \
		} \
	}

/**
 * \brief Create a scope
 *
 * \param scope The name of the scope. it will create a scope instance with
 *              this name that can be used further in the function.
 * \param [id] The ID corresponding to scope to be used. This id must first be
 *             created with IRSTD_SCOPE_REGISTER.
 */
#define IRSTD_SCOPE(...) IRSTD_GET_MACRO(_IRSTD_SCOPE, __VA_ARGS__)(static, , __VA_ARGS__)

/**
 * \brief Create a thread local scope
 *
 * \param scope The name of the scope. it will create a scope instance with
 *              this name that can be used further in the function.
 * \param [id] The ID corresponding to scope to be used. This id must first be
 *             created with IRSTD_SCOPE_THREAD_REGISTER.
 */
#define IRSTD_SCOPE_THREAD(...) IRSTD_GET_MACRO(_IRSTD_SCOPE, __VA_ARGS__)(static thread_local, Thread, __VA_ARGS__)

#define _IRSTD_SCOPE1(attr, prefix, scope) \
	attr IrStd::Scope::Flag scopeFlag##prefix = ATOMIC_FLAG_INIT; \
	IrStd::Scope scope(scopeFlag##prefix);

#define _IRSTD_SCOPE2(attr, prefix, scope, id) \
	namespace IrStd \
	{ \
		IrStd::Scope::Flag& _IRSTD_SCOPE_NAME(prefix, id)() noexcept; \
	} \
	IrStd::Scope scope(_IRSTD_SCOPE_NAME(prefix, id)());

#define _IRSTD_SCOPE_NAME(prefix, id) __scope ## prefix ## Register ## id

namespace IrStd
{
	class Scope
	{
	public:
		typedef std::atomic_flag Flag;

		Scope(std::atomic_flag& flag)
				: m_flag(flag)
				, m_isActivator(false)
		{
			activate();
		}

		~Scope()
		{
			if (m_isActivator)
			{
				deactivate();
			}
		}

		void activate() noexcept
		{
			if (!m_flag.test_and_set())
			{
				m_isActivator = true;
			}
		}

		void deactivate() noexcept
		{
			m_flag.clear();
		}

		bool isActivator() const noexcept
		{
			return m_isActivator;
		}

	private:
		Flag& m_flag;
		bool m_isActivator;
	};

	/* TODO */
	class ScopeLock : public Scope
	{
	};
}