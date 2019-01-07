//
//  array_object.tests.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-12-05.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

// Use rapidjson for validation
#include "document.h"     // rapidjson's DOM-style API
using namespace rapidjson;

#include "tsobject.hpp"
#include "tsstream.hpp"
using namespace tsjson;

#include "tsjson_cjson.hpp"
#include "tsjson_sajson.hpp"
#include "tsjson_rapidjson.hpp"
#include "tsjson_taocpp.hpp"
#include "tsjson_minijson.hpp"
#include "tsjson_nlohmannjson.hpp"

namespace
{
  std::string enumToString(tsjson::impl t)
  {
    switch (t)
    {
      case impl::cjson: return "impl::cjson";
      case impl::rapidjson: return "impl::rapidjson";
      case impl::sajson: return "impl::sajson";
      case impl::nlohmannj_son: return "impl::nlohmannj_son";
      case impl::minijson: return "impl::minijson";
      case impl::taojson: return "impl::taojson";
    }

    assert(false);
  }

  template <tsjson::impl T>
  class CtorStaticArrayTest : public ::testing::Test
  {
    struct simple : object<struct simple>
    {
      simple()
      : object<struct simple>(),
        alive(object<struct simple>::_, "alive", true),
        age(object<struct simple>::_, "age", 34u),
        ranking(object<struct simple>::_, "ranking", 0.125f),
        name(object<struct simple>::_, "name", "Hi"),
        booleans(object<struct simple>::_, "booleans"),
        integers(object<struct simple>::_, "integers"),
        numbers(object<struct simple>::_, "numbers"),
        strings(object<struct simple>::_, "strings") {}

      boolean alive;
      integer age;
      number  ranking;
      string  name;

      array<boolean, 3> booleans;
      array<integer> integers;
      array<number, 5> numbers;
      array<string> strings;
    };
    
  public:
    void TestBody()
    {
      simple v;

      EXPECT_EQ(v.alive, true) << enumToString(T);
      EXPECT_EQ(v.age, 34u) << enumToString(T);
      EXPECT_DOUBLE_EQ(v.ranking, 0.125f) << enumToString(T);
      EXPECT_EQ(v.name, "Hi") << enumToString(T);

      for (auto&& it : v.booleans)
      {
        EXPECT_TRUE(it.undefined()) << enumToString(T);
      }

      for (auto&& it : v.integers)
      {
        EXPECT_TRUE(it.undefined()) << enumToString(T);
      }

      for (auto&& it : v.numbers)
      {
        EXPECT_TRUE(it.undefined()) << enumToString(T);
      }

      for (auto&& it : v.strings)
      {
        EXPECT_TRUE(it.undefined()) << enumToString(T);
      }
    }
  };

  template <tsjson::impl T>
  void CompareSerializations(const std::string& json1, const std::string& json2)
  {
    Document document1;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
    EXPECT_FALSE(document1.Parse(json1.c_str()).HasParseError())  << enumToString(T);

    Document document2;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
    EXPECT_FALSE(document2.Parse(json2.c_str()).HasParseError()) << enumToString(T);

    if (document1 != document2)
    {
      FAIL() << json1 << std::endl << json2 << " :" << enumToString(T);
    }
  }

  template <tsjson::impl T>
  class SerializationTopLevelArrayTest : public ::testing::Test
  {
    struct simple : object<struct simple>
    {
      simple()
      : object<struct simple>(),
        alive(object<struct simple>::_, "alive", true),
        age(object<struct simple>::_, "age", 34u),
        ranking(object<struct simple>::_, "ranking", 0.125f),
        name(object<struct simple>::_, "name", "Hi") {}
      
      boolean alive;
      integer age;
      number  ranking;
      string  name;
    };
    
  public:
    void TestBody()
    {
      array<simple, 3> v;

      std::ostringstream ost;
      stream<T>(v).serialize(ost, false);
      CompareSerializations<T>(ost.str(), "[ {\"name\":\"Hi\", \"alive\":true, \"age\":34, \"ranking\":0.125 }, "
                                          "{\"name\":\"Hi\", \"alive\":true, \"age\":34, \"ranking\":0.125 }, "
                                          "{\"name\":\"Hi\", \"alive\":true, \"age\":34, \"ranking\":0.125 } ]");

      {
        array<simple,4> w;

        std::istringstream ist("[ {\"name\":\"Ho\", \"alive\":false, \"age\":56, \"ranking\":0.5 }, "
                               "{\"name\":\"Ho\", \"alive\":false, \"age\":56, \"ranking\":0.5 }, "
                               "{\"name\":\"Ho\", \"alive\":false, \"age\":56, \"ranking\":0.5 }, "
                               "{\"name\":\"Ho\", \"alive\":false, \"age\":56, \"ranking\":0.5 } ]");
        stream<T>(w).deserialize(ist);
   
        for (auto&& c : w)
        {
          EXPECT_TRUE(c.name == "Ho") << enumToString(T);
          EXPECT_FALSE(c.alive) << enumToString(T);
          EXPECT_EQ(c.age, 56) << enumToString(T);
          EXPECT_EQ(c.ranking, 0.5) << enumToString(T);
        }
      }

      {
        array<simple> w;
        
        std::istringstream ist("[ {\"name\":\"Ho\", \"alive\":false, \"age\":56, \"ranking\":0.5 }, "
                               "{\"name\":\"Ho\", \"alive\":false, \"age\":56, \"ranking\":0.5 } ]");
        stream<T>(w).deserialize(ist);

        EXPECT_EQ(w.size(), 2);
        for (auto&& c : w)
        {
          EXPECT_TRUE(c.name == "Ho") << enumToString(T);
          EXPECT_FALSE(c.alive) << enumToString(T);
          EXPECT_EQ(c.age, 56) << enumToString(T);
          EXPECT_EQ(c.ranking, 0.5) << enumToString(T);
        }
      }
    }
  };
  
  template <tsjson::impl T>
  class SerializationArrayTest : public ::testing::Test
  {
    struct simple : object<struct simple>
    {
      simple()
      : object<struct simple>(),
        alive(object<struct simple>::_, "alive", true),
        age(object<struct simple>::_, "age", 34u),
        ranking(object<struct simple>::_, "ranking", 0.125f),
        name(object<struct simple>::_, "name", "Hi"),
        booleans(object<struct simple>::_, "booleans"),
        integers(object<struct simple>::_, "integers"),
        numbers(object<struct simple>::_, "numbers"),
        strings(object<struct simple>::_, "strings") {}
      
      boolean alive;
      integer age;
      number  ranking;
      string  name;
      
      array<boolean, 3> booleans;
      array<integer> integers;
      array<number, 5> numbers;
      array<string> strings;
    };

  public:
    void TestBody()
    {
      simple v;

      // Add items to integers, strings
      v.integers.resize(4);
      for (auto&& it : v.integers)
      {
        it = 5;
      }

      v.strings.resize(6);
      for (auto&& it : v.strings)
      {
        it = "Me";
      }

      {
        std::ostringstream ost;
        stream<T>(v).serialize(ost, false);
        CompareSerializations<T>(ost.str(), "{ \"booleans\":[], "
                                            "\"integers\":[5, 5, 5, 5], "
                                            "\"numbers\":[], "
                                            "\"strings\":[\"Me\", \"Me\", \"Me\", \"Me\", \"Me\", \"Me\"], "
                                            "\"name\":\"Hi\", \"alive\":true, \"age\":34, \"ranking\":0.125 }");
      }
      
      {
        std::ostringstream ost;
        
        for (auto&& it : v.booleans)
        {
          it = true;
        }
        
        for (auto&& it : v.integers)
        {
          it += 10;
        }
        
        for (auto&& it : v.numbers)
        {
          it += 100.5;
        }
        
        for (auto&& it : v.strings)
        {
          it += "Hi";
        }

        stream<T>(v).serialize(ost, false);
        CompareSerializations<T>(ost.str(), "{ \"booleans\":[true, true, true], "
                                            "\"integers\":[15, 15, 15, 15], "
                                            "\"numbers\":[100.5, 100.5, 100.5, 100.5, 100.5], "
                                            "\"strings\":[\"MeHi\", \"MeHi\", \"MeHi\", \"MeHi\", \"MeHi\", \"MeHi\"], "
                                            "\"name\":\"Hi\", \"alive\":true, \"age\":34, \"ranking\":0.125 }");
      }
    }
  };

  template <tsjson::impl T>
  class DeserializationArrayTest : public ::testing::Test
  {
    struct simple : object<struct simple>
    {
      simple()
      : object<struct simple>(),
        alive(object<struct simple>::_, "alive", true),
        age(object<struct simple>::_, "age", 34u),
        ranking(object<struct simple>::_, "ranking", 0.125f),
        name(object<struct simple>::_, "name", "Hi"),
        booleans(object<struct simple>::_, "booleans"),
        integers(object<struct simple>::_, "integers"),
        numbers(object<struct simple>::_, "numbers"),
        strings(object<struct simple>::_, "strings") {}
      
      boolean alive;
      integer age;
      number  ranking;
      string  name;
      
      struct array<boolean, 3> booleans;
      struct array<integer> integers;
      struct array<number, 5> numbers;
      struct array<string> strings;
    };

  public:
    void TestBody()
    {
      simple v;

      std::istringstream ist("{ \"booleans\":[true, true, true], "
                             "\"integers\":[10, 10, 10, 10], "
                             "\"numbers\":[100.5, 100.5, 100.5, 100.5, 100.5], "
                             "\"strings\":[\"Hi\", \"Hi\", \"Hi\", \"Hi\", \"Hi\", \"Hi\"], "
                             "\"name\":\"Me\", \"alive\":false, \"age\":56, \"ranking\":0.5 }");
      
      stream<T>(v).deserialize(ist);
      EXPECT_TRUE(v.alive == false) << enumToString(T);
      EXPECT_TRUE(v.age == 56u) << enumToString(T);
      EXPECT_DOUBLE_EQ(v.ranking, .5f) << enumToString(T);
      EXPECT_TRUE(v.name == "Me") << enumToString(T);

      EXPECT_EQ(v.booleans.size(), 3);
      for (auto&& it : v.booleans)
      {
        EXPECT_TRUE(it == true) << enumToString(T);
      }
      
      EXPECT_EQ(v.integers.size(), 4);
      for (auto&& it : v.integers)
      {
        EXPECT_TRUE(it == 10) << enumToString(T);
      }
      
      EXPECT_EQ(v.numbers.size(), 5);
      for (auto&& it : v.numbers)
      {
        EXPECT_TRUE(it == 100.5) << enumToString(T);
      }
      
      EXPECT_EQ(v.strings.size(), 6);
      for (auto&& it : v.strings)
      {
        EXPECT_TRUE(it == "Hi") << enumToString(T);
      }
    }
  };

  
  template <tsjson::impl T>
  class ComplexArrayTest : public ::testing::Test
  {
    struct simple : object<struct simple>
    {
      struct inner : object<struct inner>
      {
        inner()
        : object<struct inner>(),
          alive(object<struct inner>::_, "alive"),
          age(object<struct inner>::_, "age"),
          ranking(object<struct inner>::_, "ranking") {}
        
        boolean alive;
        integer age;
        number  ranking;
      };

      simple()
      : object<struct simple>(),
        name(object<struct simple>::_, "name"),
        inners(object<struct simple>::_, "inners") {}
      
      string  name;
      array<array<array<inner, 2>>> inners;
    };
    
  public:
    void TestBody()
    {
      simple v;
      
      std::istringstream ist("{ \"inners\": [[[{\"alive\": true,\"age\": 34,\"ranking\": 0.125},{\"alive\": false,\"age\": 54,\"ranking\": 0.5}]]],\"name\": \"Me\"}");
      
      stream<T>(v).deserialize(ist);
      EXPECT_TRUE(v.name == "Me") << enumToString(T);
      EXPECT_EQ(v.inners.size(), 1) << enumToString(T);
      EXPECT_EQ(v.inners[0].size(), 1) << enumToString(T);
      EXPECT_EQ(v.inners[0][0].size(), 2) << enumToString(T);

      EXPECT_TRUE(v.inners[0][0][0].alive) << enumToString(T);
      EXPECT_FALSE(v.inners[0][0][1].alive) << enumToString(T);

      EXPECT_EQ(v.inners[0][0][0].age, 34) << enumToString(T);
      EXPECT_EQ(v.inners[0][0][1].age, 54) << enumToString(T);

      EXPECT_EQ(v.inners[0][0][0].ranking, 0.125) << enumToString(T);
      EXPECT_EQ(v.inners[0][0][1].ranking, 0.5) << enumToString(T);
    }
  };
}


TEST(array_object, ctor)
{
  CtorStaticArrayTest<impl::cjson>().TestBody();
  CtorStaticArrayTest<impl::rapidjson>().TestBody();
  CtorStaticArrayTest<impl::sajson>().TestBody();
  CtorStaticArrayTest<impl::nlohmannj_son>().TestBody();
  CtorStaticArrayTest<impl::minijson>().TestBody();
  CtorStaticArrayTest<impl::taojson>().TestBody();
}

TEST(array_deserialization, ctor)
{
  DeserializationArrayTest<impl::cjson>().TestBody();
  DeserializationArrayTest<impl::rapidjson>().TestBody();
  DeserializationArrayTest<impl::sajson>().TestBody();
  DeserializationArrayTest<impl::nlohmannj_son>().TestBody();
  DeserializationArrayTest<impl::minijson>().TestBody();
  DeserializationArrayTest<impl::taojson>().TestBody();
}


TEST(array_serialization, ctor)
{
  SerializationArrayTest<impl::cjson>().TestBody();
  SerializationArrayTest<impl::rapidjson>().TestBody();
  SerializationArrayTest<impl::nlohmannj_son>().TestBody();
  SerializationArrayTest<impl::minijson>().TestBody();
  SerializationArrayTest<impl::taojson>().TestBody();
}

TEST(top_array_serialization, ctor)
{
  SerializationTopLevelArrayTest<impl::cjson>().TestBody();
//  SerializationTopLevelArrayTest<impl::rapidjson>().TestBody();
  SerializationTopLevelArrayTest<impl::nlohmannj_son>().TestBody();
  SerializationTopLevelArrayTest<impl::minijson>().TestBody();
  SerializationTopLevelArrayTest<impl::taojson>().TestBody();
}

TEST(complex_array, ctor)
{
  ComplexArrayTest<impl::cjson>().TestBody();
  ComplexArrayTest<impl::rapidjson>().TestBody();
  ComplexArrayTest<impl::sajson>().TestBody();
  ComplexArrayTest<impl::nlohmannj_son>().TestBody();
  ComplexArrayTest<impl::minijson>().TestBody();
  ComplexArrayTest<impl::taojson>().TestBody();
}
