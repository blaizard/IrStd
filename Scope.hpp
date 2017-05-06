/**
 * A scope is a local object that has a global visibility uppon its lifetime.
 */

#pragma once

#include <mutex>
#include <atomic>

#include "Flag.hpp"

/**
 * \brief Register a global scope with an ID.
 * 
 * The scope is meant to be available globaly and can be referenced by its id.
 *
 * \param name The unique id to be given to this scope.
 */
#define IRSTD_SCOPE_REGISTER(name) _IRSTD_SCOPE_REGISTER(static, name)

/**
 * \brief Register a thread local scope with an ID.
 * 
 * The scope is meant to be available globaly and can be referenced by its id.
 *
 * \param name The unique id to be given to this scope.
 */
#define IRSTD_SCOPE_THREAD_REGISTER(name) _IRSTD_SCOPE_REGISTER(static thread_local, name)

#define _IRSTD_SCOPE_REGISTER(attr, name) \
	namespace IrStd \
	{ \
		namespace Flag \
		{ \
			extern IrStd::FlagImpl& name() noexcept; \
			IrStd::FlagImpl& name() noexcept \
			{ \
				attr IrStd::FlagImpl scopeFlag; \
				return scopeFlag; \
			} \
		} \
	}

#define IRSTD_SCOPE_USE(name) \
	namespace IrStd \
	{ \
		namespace Flag \
		{ \
			extern IrStd::FlagImpl& name() noexcept; \
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
#define IRSTD_SCOPE(...) IRSTD_GET_MACRO(_IRSTD_SCOPE, __VA_ARGS__)(static, __VA_ARGS__)

/**
 * \brief Create a local scope
 *
 * \param scope The name of the scope. it will create a scope instance with
 *              this name that can be used further in the function.
 * \param [id] The ID corresponding to scope to be used. This id must first be
 *             created with IRSTD_SCOPE_REGISTER.
 */
#define IRSTD_SCOPE_LOCAL(...) IRSTD_GET_MACRO(_IRSTD_SCOPE, __VA_ARGS__)(, __VA_ARGS__)

/**
 * \brief Create a thread local scope
 *
 * \param scope The name of the scope. it will create a scope instance with
 *              this name that can be used further in the function.
 * \param [id] The ID corresponding to scope to be used. This id must first be
 *             created with IRSTD_SCOPE_THREAD_REGISTER.
 */
#define IRSTD_SCOPE_THREAD(...) IRSTD_GET_MACRO(_IRSTD_SCOPE, __VA_ARGS__)(static thread_local, __VA_ARGS__)

#define _IRSTD_SCOPE1(attr, scope) \
	attr IrStd::FlagImpl scopeFlag; \
	IrStd::Scope scope(scopeFlag);

#define _IRSTD_SCOPE2(attr, scope, name) \
	IrStd::Scope scope(IrStd::Flag::name());

namespace IrStd
{
	class Scope
	{
	public:
		Scope(FlagImpl& flag)
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
			if (!m_flag.setAndGet())
			{
				m_isActivator = true;
			}
		}

		void deactivate() noexcept
		{
			m_flag.unset();
		}

		bool isActivator() const noexcept
		{
			return m_isActivator;
		}

	private:
		FlagImpl& m_flag;
		bool m_isActivator;
	};
}