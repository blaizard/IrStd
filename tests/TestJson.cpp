#include "../Test.hpp"
#include "../IrStd.hpp"

class JsonTest : public IrStd::Test
{
};

// ---- JsonTest::testBasicTypes ------------------------------------------------

TEST_F(JsonTest, testBasicTypes)
{
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
		ASSERT_TRUE(!json.isArray("number1"));
		ASSERT_TRUE(!json.isObject("number1"));

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
		ASSERT_TRUE(!json.isArray("number2"));
		ASSERT_TRUE(!json.isObject("number2"));

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
		ASSERT_TRUE(!json.isArray("bool"));
		ASSERT_TRUE(!json.isObject("bool"));

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
		ASSERT_TRUE(!json.isArray("null"));
		ASSERT_TRUE(!json.isObject("null"));

		json.getNull("null");
	}

	// Test String type
	{
		ASSERT_TRUE(json.isString("string"));
		ASSERT_TRUE(!json.isNumber("string"));
		ASSERT_TRUE(!json.isNull("string"));
		ASSERT_TRUE(!json.isBool("string"));
		ASSERT_TRUE(!json.isArray("string"));
		ASSERT_TRUE(!json.isObject("string"));

		auto& type = json.getString("string");
		ASSERT_TRUE(strcmp(type.val(), "mystring") == 0);
		ASSERT_TRUE(type.size() == strlen("mystring"));
		type.val(json, "yourstring", strlen("yourstring"));
		ASSERT_TRUE(strcmp(type.val(), "yourstring") == 0);
	}
}

// ---- JsonTest::testObject --------------------------------------------------

TEST_F(JsonTest, testObject)
{
	IrStd::Json json(
		"{"
			"\"object\": {"
				"\"item1\": 1,"
				"\"item2\": 2,"
				"\"item3\": 3"
			"}"
		"}");

	// Test Integer Number type
	{
		ASSERT_TRUE(!json.isNumber("object"));
		ASSERT_TRUE(!json.isBool("object"));
		ASSERT_TRUE(!json.isString("object"));
		ASSERT_TRUE(!json.isNull("object"));
		ASSERT_TRUE(!json.isArray("object"));
		ASSERT_TRUE(json.isObject("object"));

		auto& object = json.getObject("object");
		size_t nbItems = 0;
		for (const auto& item : object)
		{
			ASSERT_TRUE(validateOutput(item.first, "item[1-3]", RegexMatch::MATCH_ALL));
			ASSERT_TRUE(item.second.isNumber());
			ASSERT_TRUE(item.second.getNumber().val() >= 1 && item.second.getNumber().val() <= 3);
			nbItems++;
		}
		ASSERT_TRUE(nbItems == 3);
	}
}

// ---- JsonTest::testArray ---------------------------------------------------

TEST_F(JsonTest, testArray)
{
	IrStd::Json json(
		"{"
			"\"array\": [0, 1, 2, 3, 4]"
		"}");

	// Test Integer Number type
	{
		ASSERT_TRUE(!json.isNumber("array"));
		ASSERT_TRUE(!json.isBool("array"));
		ASSERT_TRUE(!json.isString("array"));
		ASSERT_TRUE(!json.isNull("array"));
		ASSERT_TRUE(json.isArray("array"));
		ASSERT_TRUE(!json.isObject("array"));

		const auto& array = json.getArray("array");
		ASSERT_TRUE(array.size() == 5);

		for (int i = 0; i<5; i++)
		{
			ASSERT_TRUE(array.isNumber(i));
			ASSERT_TRUE(IrStd::almostEqual(array.getNumber(i).val(), i));
		}
	}
}

// ---- JsonTest::testSerializeSimple -----------------------------------------

TEST_F(JsonTest, testSerializeSimple)
{
	// Empty
	{
		IrStd::Json json;
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\\}", RegexMatch::MATCH_ALL));
	}

	// Simple field
	{
		IrStd::Json json("{\"number\":1}");
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\"number\":1\\}", RegexMatch::MATCH_ALL));
	}

	// Change value
	{
		IrStd::Json json("{\"number\":1}");
		json.getNumber("number").val(2);
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\"number\":2.0\\}", RegexMatch::MATCH_ALL));
	}

	// Create a string value
	{
		IrStd::Json json;
		json.add("string", "test");
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\"string\":\"test\"\\}", RegexMatch::MATCH_ALL));
	}

	// Create a number value
	{
		IrStd::Json json;
		json.add("number", 42);
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\"number\":42\\}", RegexMatch::MATCH_ALL));
	}

	// Create a floating number value
	{
		IrStd::Json json;
		json.add("float", -1.25);
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\"float\":-1.25\\}", RegexMatch::MATCH_ALL));
	}

	// Create a boolean value
	{
		IrStd::Json json;
		json.add("bool", true);
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\"bool\":true\\}", RegexMatch::MATCH_ALL));
	}

	// Create a null value
	{
		IrStd::Json json;
		json.add("null");
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\"null\":null\\}", RegexMatch::MATCH_ALL));
	}

	// Create an empty array
	{
		IrStd::Json json;
		std::vector<int> vector;
		json.add("array", vector);
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\"array\":\\[\\]\\}", RegexMatch::MATCH_ALL));
	}

	// Create an empty object
	{
		IrStd::Json json;
		std::map<std::string, int> map;
		json.add("object", map);
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\"object\":\\{\\}\\}", RegexMatch::MATCH_ALL));
	}
}

// ---- JsonTest::testSerializeArray ------------------------------------------

TEST_F(JsonTest, testSerializeArray)
{
	{
		IrStd::Json json;
		std::vector<int> vector(3, 1);
		json.add("array", vector);
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\"array\":\\[1,1,1\\]\\}", RegexMatch::MATCH_ALL));
	}

	// Nested arrays
	{
		IrStd::Json json;
		std::vector<std::vector<bool>> vector(3, std::vector<bool>(2, true));
		json.add("array", vector);
		const auto str = json.serialize();
		ASSERT_TRUE(validateOutput(str, "\\{\"array\":\\[\\[true,true\\],\\[true,true\\],\\[true,true\\]\\]\\}", RegexMatch::MATCH_ALL));
	}
}

// ---- JsonTest::testSerializeObject -----------------------------------------

TEST_F(JsonTest, testSerializeObject)
{
	{
		IrStd::Json json;
		std::map<std::string, int> map;
		map["hello"] = 2;
		json.add("object", map);
		ASSERT_TRUE(validateOutput(json.serialize(), "\\{\"object\":\\{\"hello\":2\\}\\}", RegexMatch::MATCH_ALL));
		json.clear();
		ASSERT_TRUE(validateOutput(json.serialize(), "\\{\\}", RegexMatch::MATCH_ALL));
	}

	{
		IrStd::Json json({
			{"temp", 2},
			{"dsdsd", {47, false, {8, "45", false}}},
		});
		std::cout << json.serialize() << std::endl;
		json.clear();
		ASSERT_TRUE(validateOutput(json.serialize(), "\\{\\}", RegexMatch::MATCH_ALL));
	}
}
