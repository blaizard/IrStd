/**
 * A scope is a local object that has a global visibility uppon its lifetime.
 */

#pragma once

#include <mutex>
#include <atomic>

#include "Flag.hpp"

/**
 * \brief Register a local scope with an ID.
 *
 * \param name The unique id to be given to this scope.
 */
#define IRSTD_SCOPE_LOCAL_REGISTER(name) IrStd::FlagBool name
#define IRSTD_SCOPE_LOCK_LOCAL_REGISTER(name) IrStd::FlagLock name

/**
 * \brief Register a local scope with an ID.
 *
 * \param name The unique id to be given to this scope.
 */
#define IRSTD_SCOPE_THREAD_REGISTER(name) notimplemented
#define IRSTD_SCOPE_LOCK_THREAD_REGISTER(name) IrStd::FlagLockThread name

/**
 * \brief Register a global scope with an ID.
 * 
 * The scope is meant to be available globaly and can be referenced by its id.
 *
 * \param name The unique id to be given to this scope.
 */
#define IRSTD_SCOPE_GLOBAL_REGISTER(name) _IRSTD_SCOPE_REGISTER(IrStd::FlagBool, static, name)
#define IRSTD_SCOPE_LOCK_GLOBAL_REGISTER(name) _IRSTD_SCOPE_REGISTER(IrStd::FlagLock, static, name)

/**
 * \brief Register a thread local scope with an ID.
 * 
 * The scope is meant to be available globaly and can be referenced by its id.
 *
 * \param name The unique id to be given to this scope.
 */
#define IRSTD_SCOPE_LOCALTHREAD_REGISTER(name) _IRSTD_SCOPE_REGISTER(IrStd::FlagBool, static thread_local, name)
#define IRSTD_SCOPE_LOCK_LOCALTHREAD_REGISTER(name) _IRSTD_SCOPE_REGISTER(IrStd::FlagLock, static thread_local, name)

#define _IRSTD_SCOPE_REGISTER(type, attr, name) \
	namespace IrStd \
	{ \
		namespace Flag \
		{ \
			IrStd::FlagInterface* IRSTD_PASTE(__, name)() noexcept \
			{ \
				attr type scopeFlag; \
				return &scopeFlag; \
			} \
			extern IrStd::FlagInterface* name; \
			IrStd::FlagInterface* name = IRSTD_PASTE(__, name)(); \
		} \
	}

/**
 * \brief Use a scope previously created by IRSTD_SCOPE_*_REGISTER
 *
 * \param name The name of the scope to be used
 */
#define IRSTD_SCOPE_USE(name) \
	namespace IrStd \
	{ \
		namespace Flag \
		{ \
			extern IrStd::FlagInterface* name; \
		} \
	}

/**
 * \brief Use an existing scope
 *
 * \param [scope] The name of the scope. it will create a scope instance with
 *                this name that can be used further in the function.
 * \param name The name of the scope to be used. This id must first be
 *           created with IRSTD_SCOPE_*_REGISTER.
 */
#define IRSTD_SCOPE(...) IRSTD_GET_MACRO(_IRSTD_SCOPE, __VA_ARGS__)(__VA_ARGS__)

#define _IRSTD_SCOPE1(name) \
	IrStd::Scope IRSTD_PASTE(__scope, __LINE__)(name);
#define _IRSTD_SCOPE2(scope, name) \
	IrStd::Scope scope(name);

/**
 * \brief Create and use local scope
 *
 * \param scope The name of the scope. it will create a scope instance with
 *              this name that can be used further in the function.
 */
#define IRSTD_SCOPE_LOCAL(scope) _IRSTD_SCOPE(IrStd::FlagBool, , scope)
#define IRSTD_SCOPE_LOCK_LOCAL(scope) _IRSTD_SCOPE(IrStd::FlagLock, , scope)

/**
 * \brief Create and use a global scope
 *
 * \param scope The name of the scope. it will create a scope instance with
 *              this name that can be used further in the function.
 */
#define IRSTD_SCOPE_GLOBAL(scope) _IRSTD_SCOPE(IrStd::FlagBool, static, scope)
#define IRSTD_SCOPE_LOCK_GLOBAL(scope) _IRSTD_SCOPE(IrStd::FlagLock, static, scope)

/**
 * \brief Create and use a thread local scope
 *
 * \param scope The name of the scope. it will create a scope instance with
 *              this name that can be used further in the function.
 */
#define IRSTD_SCOPE_THREAD(scope) _IRSTD_SCOPE(IrStd::FlagBool, static thread_local, scope)
#define IRSTD_SCOPE_LOCK_THREAD(scope) _IRSTD_SCOPE(IrStd::FlagLock, static thread_local, scope)


#define _IRSTD_SCOPE(type, attr, scope) \
	attr type scopeFlag; \
	IrStd::Scope scope(&scopeFlag);

namespace IrStd
{
	class Scope
	{
	public:
		Scope(FlagBool& flag, const bool activateScope = true)
				: Scope(&flag, activateScope)
		{
		}

		Scope(FlagLock& flag, const bool activateScope = true)
				: Scope(&flag, activateScope)
		{
		}

		Scope(FlagLockThread& flag, const bool activateScope = true)
				: Scope(&flag, activateScope)
		{
		}

		Scope(FlagInterface* flag, const bool activateScope = true)
				: m_flag(flag)
				, m_isActivator(false)
		{
			if (activateScope)
			{
				activate();
			}
		}

		/**
		 * Delete the copy constructor
		 */
		Scope(const Scope& scope) = delete;

		/**
		 * Move constructor
		 */
		Scope(Scope&& scope)
				: m_flag(scope.m_flag)
				, m_isActivator(scope.m_isActivator)
		{
			scope.m_isActivator = false;
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
			if (!m_flag->setAndGet())
			{
				m_isActivator = true;
			}
		}

		void deactivate() noexcept
		{
			m_flag->unsetAndGet();
		}

		bool isActivator() const noexcept
		{
			return m_isActivator;
		}

	private:
		FlagInterface* m_flag;
		bool m_isActivator;
	};
}
