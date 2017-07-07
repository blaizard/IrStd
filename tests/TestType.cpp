#include "../Test.hpp"
#include "../IrStd.hpp"

class TypeTest : public IrStd::Test
{
};

TEST_F(TypeTest, testToString)
{
	// const char*
	{
		const auto str = IrStd::Type::ShortString("testString");
		ASSERT_TRUE(!strcmp(str, "testString")) << "str='" << str << "'";
	}
	// uint8_t
	{
		const auto str1 = IrStd::Type::ShortString(static_cast<uint8_t>(12));
		ASSERT_TRUE(!strcmp(str1, "12")) << "str='" << str1 << "'";
		const auto str2 = IrStd::Type::ShortString(static_cast<uint8_t>(0));
		ASSERT_TRUE(!strcmp(str2, "0")) << "str='" << str2 << "'";
		const auto str3 = IrStd::Type::ShortString(static_cast<uint8_t>(0xff));
		ASSERT_TRUE(!strcmp(str3, "255")) << "str='" << str3 << "'";
	}
	// uint16_t
	{
		const auto str1 = IrStd::Type::ShortString(static_cast<uint16_t>(42));
		ASSERT_TRUE(!strcmp(str1, "42")) << "str='" << str1 << "'";
		const auto str2 = IrStd::Type::ShortString(static_cast<uint16_t>(0));
		ASSERT_TRUE(!strcmp(str2, "0")) << "str='" << str2 << "'";
		const auto str3 = IrStd::Type::ShortString(static_cast<uint16_t>(0xffff));
		ASSERT_TRUE(!strcmp(str3, "65535")) << "str='" << str3 << "'";
	}
	// uint32_t
	{
		const auto str1 = IrStd::Type::ShortString(static_cast<uint32_t>(42));
		ASSERT_TRUE(!strcmp(str1, "42")) << "str='" << str1 << "'";
		const auto str2 = IrStd::Type::ShortString(static_cast<uint32_t>(0));
		ASSERT_TRUE(!strcmp(str2, "0")) << "str='" << str2 << "'";
		const auto str3 = IrStd::Type::ShortString(static_cast<uint32_t>(0xffffffff));
		ASSERT_TRUE(!strcmp(str3, "4294967295")) << "str='" << str3 << "'";
	}
	// uint64_t
	{
		const auto str1 = IrStd::Type::ShortString(static_cast<uint64_t>(42));
		ASSERT_TRUE(!strcmp(str1, "42")) << "str='" << str1 << "'";
		const auto str2 = IrStd::Type::ShortString(static_cast<uint64_t>(0));
		ASSERT_TRUE(!strcmp(str2, "0")) << "str='" << str2 << "'";
		const auto str3 = IrStd::Type::ShortString(static_cast<uint64_t>(0xffffffffffffffff));
		ASSERT_TRUE(!strcmp(str3, "18446744073709551615")) << "str='" << str3 << "'";
		ASSERT_TRUE(strlen(str3) < IrStd::Type::ShortString::SIZE);
	}
	// int8_t
	{
		const auto str1 = IrStd::Type::ShortString(static_cast<int8_t>(-12));
		ASSERT_TRUE(!strcmp(str1, "-12")) << "str='" << str1 << "'";
		const auto str2 = IrStd::Type::ShortString(static_cast<int8_t>(0));
		ASSERT_TRUE(!strcmp(str2, "0")) << "str='" << str2 << "'";
		const auto str3 = IrStd::Type::ShortString(static_cast<int8_t>(0x7f));
		ASSERT_TRUE(!strcmp(str3, "127")) << "str='" << str3 << "'";
		const auto str4 = IrStd::Type::ShortString(static_cast<int8_t>(0x80));
		ASSERT_TRUE(!strcmp(str4, "-128")) << "str='" << str4 << "'";
	}
	// int16_t
	{
		const auto str1 = IrStd::Type::ShortString(static_cast<int16_t>(-12));
		ASSERT_TRUE(!strcmp(str1, "-12")) << "str='" << str1 << "'";
		const auto str2 = IrStd::Type::ShortString(static_cast<int16_t>(0));
		ASSERT_TRUE(!strcmp(str2, "0")) << "str='" << str2 << "'";
		const auto str3 = IrStd::Type::ShortString(static_cast<int16_t>(0x7fff));
		ASSERT_TRUE(!strcmp(str3, "32767")) << "str='" << str3 << "'";
		const auto str4 = IrStd::Type::ShortString(static_cast<int16_t>(0x8000));
		ASSERT_TRUE(!strcmp(str4, "-32768")) << "str='" << str4 << "'";
	}
	// int32_t
	{
		const auto str1 = IrStd::Type::ShortString(static_cast<int32_t>(-12));
		ASSERT_TRUE(!strcmp(str1, "-12")) << "str='" << str1 << "'";
		const auto str2 = IrStd::Type::ShortString(static_cast<int32_t>(0));
		ASSERT_TRUE(!strcmp(str2, "0")) << "str='" << str2 << "'";
		const auto str3 = IrStd::Type::ShortString(static_cast<int32_t>(0x7fffffff));
		ASSERT_TRUE(!strcmp(str3, "2147483647")) << "str='" << str3 << "'";
		const auto str4 = IrStd::Type::ShortString(static_cast<int32_t>(0x80000000));
		ASSERT_TRUE(!strcmp(str4, "-2147483648")) << "str='" << str4 << "'";
	}
	// int64_t
	{
		const auto str1 = IrStd::Type::ShortString(static_cast<int64_t>(-12));
		ASSERT_TRUE(!strcmp(str1, "-12")) << "str='" << str1 << "'";
		const auto str2 = IrStd::Type::ShortString(static_cast<int64_t>(0));
		ASSERT_TRUE(!strcmp(str2, "0")) << "str='" << str2 << "'";
		const auto str3 = IrStd::Type::ShortString(static_cast<int64_t>(0x7fffffffffffffff));
		ASSERT_TRUE(!strcmp(str3, "9223372036854775807")) << "str='" << str3 << "'";
		ASSERT_TRUE(strlen(str3) < IrStd::Type::ShortString::SIZE);
		const auto str4 = IrStd::Type::ShortString(static_cast<int64_t>(0x8000000000000000));
		ASSERT_TRUE(!strcmp(str4, "-9223372036854775808")) << "str='" << str4 << "'";
		ASSERT_TRUE(strlen(str4) < IrStd::Type::ShortString::SIZE);
	}
	// doubleToString
	{
		char buffer[32];
		{
			ASSERT_TRUE(IrStd::Type::doubleToString(buffer, 5, 0) == 2);
			ASSERT_TRUE(!strcmp(buffer, "0")) << "str='" << buffer << "'";
		}
		{
			ASSERT_TRUE(IrStd::Type::doubleToString(buffer, 5, 1.12) == 5);
			ASSERT_TRUE(!strcmp(buffer, "1.12")) << "str='" << buffer << "'";
		}
		{
			ASSERT_TRUE(IrStd::Type::doubleToString(buffer, 5, 1.1234) == 5);
			ASSERT_TRUE(!strcmp(buffer, "1.12")) << "str='" << buffer << "'";
		}
		{
			ASSERT_TRUE(IrStd::Type::doubleToString(buffer, 10, -1.234) == 7);
			ASSERT_TRUE(!strcmp(buffer, "-1.234")) << "str='" << buffer << "'";
		}
		{
			ASSERT_TRUE(IrStd::Type::doubleToString(buffer, 5, -1.1234) == 5);
			ASSERT_TRUE(!strcmp(buffer, "-1.1")) << "str='" << buffer << "'";
		}
		{
			ASSERT_TRUE(IrStd::Type::doubleToString(buffer, 5, -1) == 3);
			ASSERT_TRUE(!strcmp(buffer, "-1")) << "str='" << buffer << "'";
		}
		{
			ASSERT_TRUE(IrStd::Type::doubleToString(buffer, 10, 0.001) == 6);
			ASSERT_TRUE(!strcmp(buffer, "0.001")) << "str='" << buffer << "'";
		}
		// IrStd::TypeFormat::FLAG_FLOOR
		{
			ASSERT_TRUE(IrStd::Type::doubleToString(buffer, 10, 1.56, 1, IrStd::TypeFormat::FLAG_FLOOR) == 4);
			ASSERT_TRUE(!strcmp(buffer, "1.5")) << "str='" << buffer << "'";
		}
		// IrStd::TypeFormat::FLAG_CEIL
		{
			ASSERT_TRUE(IrStd::Type::doubleToString(buffer, 10, 1.56, 1, IrStd::TypeFormat::FLAG_CEIL) == 4);
			ASSERT_TRUE(!strcmp(buffer, "1.6")) << "str='" << buffer << "'";
		}
		// IrStd::TypeFormat::FLAG_ROUND
		{
			ASSERT_TRUE(IrStd::Type::doubleToString(buffer, 10, 1.56, 1, IrStd::TypeFormat::FLAG_ROUND) == 4);
			ASSERT_TRUE(!strcmp(buffer, "1.6")) << "str='" << buffer << "'";
		}
	}
	// float
	{
		const auto str1 = IrStd::Type::ShortString(static_cast<float>(1.782531));
		ASSERT_TRUE(!strcmp(str1, "1.782531")) << "str='" << str1 << "'";
		const auto str2 = IrStd::Type::ShortString(static_cast<float>(-12.548993));
		ASSERT_TRUE(!strcmp(str2, "-12.548993")) << "str='" << str2 << "'";
		const auto str3 = IrStd::Type::ShortString(static_cast<float>(0));
		ASSERT_TRUE(!strcmp(str3, "0")) << "str='" << str3 << "'";
		const auto str4 = IrStd::Type::ShortString(static_cast<float>(-15.123456789), 3);
		ASSERT_TRUE(!strcmp(str4, "-15.123")) << "str='" << str4 << "'";
	}
	// double
	{
		const auto str1 = IrStd::Type::ShortString(static_cast<double>(1.782531));
		ASSERT_TRUE(!strcmp(str1, "1.782531")) << "str='" << str1 << "'";
		const auto str2 = IrStd::Type::ShortString(static_cast<double>(-12.548993));
		ASSERT_TRUE(!strcmp(str2, "-12.548993")) << "str='" << str2 << "'";
		const auto str3 = IrStd::Type::ShortString(static_cast<double>(0));
		ASSERT_TRUE(!strcmp(str3, "0")) << "str='" << str3 << "'";
		const auto str4 = IrStd::Type::ShortString(static_cast<double>(-1.123456789), 9);
		ASSERT_TRUE(!strcmp(str4, "-1.123456789")) << "str='" << str4 << "'";
	}
}
