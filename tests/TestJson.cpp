#include "../Test.hpp"
#include "../IrStd.hpp"

class JsonTest : public IrStd::Test
{
};

// ---- JsonTest::testBasicTypes ------------------------------------------------

TEST_F(JsonTest, testBasicTypes) {
	IrStd::Json json(
		"{"
			"\"number1\": 42,"
			"\"number2\": -0.458,"
			"\"bool\": true,"
			"\"null\": null,"
			"\"string\": \"mystring\""
		"}");

	// Test Integer Number type
	{
		ASSERT_TRUE(json.isNumber("number1"));
		ASSERT_TRUE(!json.isBool("number1"));
		ASSERT_TRUE(!json.isString("number1"));
		ASSERT_TRUE(!json.isNull("number1"));

		auto& type = json.getNumber("number1");
		ASSERT_TRUE(IrStd::almostEqual(type.val(), 42));
		type.val(96);
		ASSERT_TRUE(IrStd::almostEqual(type.val(), 96));
	}

	// Test Decimal Number type
	{
		ASSERT_TRUE(json.isNumber("number2"));
		ASSERT_TRUE(!json.isBool("number2"));
		ASSERT_TRUE(!json.isString("number2"));
		ASSERT_TRUE(!json.isNull("number2"));

		auto& type = json.getNumber("number2");
		ASSERT_TRUE(IrStd::almostEqual(type.val(), -0.458));
		type.val(7879.62);
		ASSERT_TRUE(IrStd::almostEqual(type.val(), 7879.62));
	}

	// Test Bool type
	{
		ASSERT_TRUE(json.isBool("bool"));
		ASSERT_TRUE(!json.isNumber("bool"));
		ASSERT_TRUE(!json.isString("bool"));
		ASSERT_TRUE(!json.isNull("bool"));

		auto& type = json.getBool("bool");
		ASSERT_TRUE(type.val());
		type.val(false);
		ASSERT_TRUE(!type.val());
	}

	// Test Null type
	{
		ASSERT_TRUE(json.isNull("null"));
		ASSERT_TRUE(!json.isNumber("null"));
		ASSERT_TRUE(!json.isString("null"));
		ASSERT_TRUE(!json.isBool("null"));

		json.getNull("null");
	}

	// Test String type
	{
		ASSERT_TRUE(json.isString("string"));
		ASSERT_TRUE(!json.isNumber("string"));
		ASSERT_TRUE(!json.isNull("string"));
		ASSERT_TRUE(!json.isBool("string"));

		auto& type = json.getString("string");
		ASSERT_TRUE(strcmp(type.val(), "mystring") == 0);
		ASSERT_TRUE(type.len() == strlen("mystring"));
		type.val("yourstring", strlen("yourstring"));
		ASSERT_TRUE(strcmp(type.val(), "yourstring") == 0);
	}
}
