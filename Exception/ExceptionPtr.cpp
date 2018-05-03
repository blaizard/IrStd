#include "../Exception.hpp"
#include "../Assert.hpp"

IrStd::ExceptionPtr::ExceptionPtr()
		: m_pIrstd(nullptr)
		, m_pStd(nullptr)
{
}

IrStd::ExceptionPtr::ExceptionPtr(const IrStd::Exception* pE)
		: m_pIrstd(pE)
		, m_pStd(static_cast<const std::exception*>(pE))
{
}

IrStd::ExceptionPtr::ExceptionPtr(const std::exception* pE)
		: m_pIrstd(nullptr)
		, m_pStd(pE)
{
}

bool IrStd::ExceptionPtr::operator==(const ExceptionPtr& pE) const
{
	return (m_pIrstd == pE.m_pIrstd || m_pStd == pE.m_pStd);
}

bool IrStd::ExceptionPtr::operator==(const IrStd::Exception* pE) const
{
	return (m_pIrstd == pE);
}

bool IrStd::ExceptionPtr::operator==(const std::exception* pE) const
{
	return (m_pStd == pE);
}

bool IrStd::ExceptionPtr::operator==(std::nullptr_t) const
{
	return !isException();
}

bool IrStd::ExceptionPtr::operator!() const
{
	return !isException();
}

IrStd::ExceptionPtr IrStd::ExceptionPtr::fromExceptionPtr(const std::exception_ptr ptr)
{
	try
	{
		std::rethrow_exception(ptr);
	}
	catch (const IrStd::Exception& e)
	{
		return IrStd::ExceptionPtr(&e);
	}
	catch (const std::exception& e)
	{
		return IrStd::ExceptionPtr(&e);
	}
}

IrStd::ExceptionPtr::operator int() const
{
	return (operator!()) ? 0 : 1;
}

IrStd::ExceptionPtr IrStd::ExceptionPtr::getNext() const
{
	if (m_pStd == nullptr)
	{
		return ExceptionPtr();
	}
	return IrStd::Exception::getNext(m_pStd);
}

bool IrStd::ExceptionPtr::isIrStdException() const noexcept
{
	return (m_pIrstd) ? true : false;
}

bool IrStd::ExceptionPtr::isException() const noexcept
{
	return (m_pStd) ? true : false;
}

void IrStd::ExceptionPtr::toStream(std::ostream& out) const noexcept
{
	const std::function<void (const IrStd::ExceptionPtr, const bool)> printRec
			= [&](const IrStd::ExceptionPtr pE, const bool isFirst)
	{
		IRSTD_ASSERT(pE, "ExceptionPtr passed into argument is null");

		if (!isFirst)
		{
			out << ": ";
		}

		if (pE.isIrStdException())
		{
			const auto& e = pE.getIrStdException();
			out << e.what();
		}
		else
		{
			IRSTD_ASSERT(pE.isException(), "The exception must be of type std::exception");
			const auto& e = pE.getException();
			out << e.what();
		}

		// Handle chained exceptions
		if (auto pNexE = pE.getNext())
		{
			printRec(pNexE, false);
		}
	};

	printRec(*this, true);
}

const IrStd::Exception& IrStd::ExceptionPtr::getIrStdException() const
{
	IRSTD_THROW_ASSERT(m_pIrstd, "This exception is not of type IrStd::Exception");
	return *m_pIrstd;
}

const std::exception& IrStd::ExceptionPtr::getException() const
{
	IRSTD_THROW_ASSERT(m_pStd, "This exception is null");
	return *m_pStd;
}

const std::exception& IrStd::ExceptionPtr::operator*() const
{
	IRSTD_THROW_ASSERT(m_pStd, "This exception pointer is null");
	return *m_pStd;
}


const std::exception* IrStd::ExceptionPtr::operator->() const
{
	return &this->operator*();
}