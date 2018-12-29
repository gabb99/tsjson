# tsjson
C++ typesafe bindings for json. Supports rapidjson and others 3rd party json libs as backends.


# Supported backends for serialization
* [cJSON GitHub](https://github.com/DaveGamble/cJSON.git)
* [minijson](GitHub]https://github.com/giacomodrago/)
* [nlohmann's json](GitHub]https://github.com/nlohmann/json.git)
* [RapidJSON GitHub](https://github.com/Tencent/rapidjson/)
* [saJson GitHub](https://github.com/chadaustin/sajson.git)
* [taocpp GitHub](https://github.com/taocpp/json.git)

# Usage at a glance

This simple example parses a JSON string into a real c++ structure/class.

~~~~~~~~~~cpp
#include "tsobject.hpp"
#include "tsstream.hpp"
using namespace tsjson;

// Include your backend for (de)serialization
#include "tsjson_rapidjson.hpp"

int main()
{
	const auto json = "{ \"a_null\" : null, "
						"\"a_boolean\" : true, "
						"\"an_int\" : 456, "
						"\"a_number\" : 0.234, "
						"\"a_string\" : \"Test\", "
					    "\"an_array\" : [1, 2, 3, 4, 5], "
					"}";

	struct example : object<struct example>
	{
		example()
		: object<struct example>(),
			a_nullable(_, "a_null"),
			a_boolean(_, "a_boolean"),
			an_int(_, "an_int"),
			a_number(_, "a_number"),
			a_string(_, "a_string"),
			an_array(_, "an_array") {}

		integer a_nullable;	// Any type can be null

		boolean a_boolean;
		integer an_int;
		number  a_number;
		string  a_string;
		array<integer> an_array;
	} an_example;


	std::istringstream ist(json);
	stream<impl::rapidjson>(an_example).deserialize(ist);
// or
// rapidjson::deserialize(ist, an_example);

	std::cout << an_example.a_nullable.null() << ", " << an_example.a_boolean << ", " << an_example.an_int << ", "
						<< an_example.a_number << ", " << an_example.a_string << std::endl;

	std::cout << "[ ";
	for (auto&& c : an_example.an_array)
		std::cout << c << ", ";
	std::cout << " ]" << std::endl;

	an_example.a_nullable = 25;
	an_example.a_boolean.null(true);
	an_example.an_int = an_example.an_int * 2;
	an_example.a_number = an_example.a_number * 2.;
	an_example.a_string += " again";

	an_example.an_array.resize(an_example.an_array.size() + 1);
	an_example.an_array[an_example.an_array.size() - 1] = 10;

	std::ostringstream ost;
	stream<impl::rapidjson>(an_example).serialize(ost);
// or
// rapidjson::serialize(ost, true, an_example);
	std::cout << ost.str() << std::endl;
}
~~~~~~~~~~
