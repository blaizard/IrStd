#include "../Compiler.hpp"

// Year
#define IRSTD_BUILD_YEAR __DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10]

// Month
#define IRSTD_BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define IRSTD_BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define IRSTD_BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define IRSTD_BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define IRSTD_BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define IRSTD_BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define IRSTD_BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define IRSTD_BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define IRSTD_BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define IRSTD_BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define IRSTD_BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define IRSTD_BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')
#define IRSTD_BUILD_MONTH \
	((IRSTD_BUILD_MONTH_IS_OCT || IRSTD_BUILD_MONTH_IS_NOV || IRSTD_BUILD_MONTH_IS_DEC) ? '1' : '0'), \
	( \
		(IRSTD_BUILD_MONTH_IS_JAN) ? '1' : \
		(IRSTD_BUILD_MONTH_IS_FEB) ? '2' : \
		(IRSTD_BUILD_MONTH_IS_MAR) ? '3' : \
		(IRSTD_BUILD_MONTH_IS_APR) ? '4' : \
		(IRSTD_BUILD_MONTH_IS_MAY) ? '5' : \
		(IRSTD_BUILD_MONTH_IS_JUN) ? '6' : \
		(IRSTD_BUILD_MONTH_IS_JUL) ? '7' : \
		(IRSTD_BUILD_MONTH_IS_AUG) ? '8' : \
		(IRSTD_BUILD_MONTH_IS_SEP) ? '9' : \
		(IRSTD_BUILD_MONTH_IS_OCT) ? '0' : \
		(IRSTD_BUILD_MONTH_IS_NOV) ? '1' : \
		(IRSTD_BUILD_MONTH_IS_DEC) ? '2' : \
		'?' \
	)

// Day
#define IRSTD_BUILD_DAY ((__DATE__[4] >= '0') ? (__DATE__[4]) : '0'), (__DATE__[ 5])

// Hours
#define IRSTD_BUILD_HOUR (__TIME__[0]), (__TIME__[1])

// Minute
#define IRSTD_BUILD_MIN (__TIME__[3]), (__TIME__[4])

// Second
#define IRSTD_BUILD_SEC (__TIME__[6]), (__TIME__[7])

const std::string& IrStd::Compiler::getBuildVersion() noexcept
{
	static const std::string buildVersion({IRSTD_BUILD_YEAR, IRSTD_BUILD_MONTH, IRSTD_BUILD_DAY,
			'.', IRSTD_BUILD_HOUR, IRSTD_BUILD_MIN, IRSTD_BUILD_SEC});
	return buildVersion;
}
