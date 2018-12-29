//
//  simple_object.tests.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-28.
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
  class CtorSimpleTest : public ::testing::Test
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
      simple v;

      EXPECT_EQ(v.alive, true) << enumToString(T);
      EXPECT_EQ(v.age, 34u) << enumToString(T);
      EXPECT_DOUBLE_EQ(v.ranking, 0.125f) << enumToString(T);
      EXPECT_EQ(v.name, "Hi") << enumToString(T);
    }
  };

  template <tsjson::impl T>
  class DeserializationSimpleTest : public ::testing::Test
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
      simple v;
      std::istringstream ist("{ \"name\":\"Me\", \"alive\":false, \"age\":56, \"ranking\":0.5 }");
      stream<T>(v).deserialize(ist);

      EXPECT_TRUE(v.alive == false) << enumToString(T);
      EXPECT_TRUE(v.age == 56u) << enumToString(T);
      EXPECT_DOUBLE_EQ(v.ranking, .5f) << enumToString(T);
      EXPECT_TRUE(v.name == "Me") << enumToString(T);
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
  class SerializationSimpleTest : public ::testing::Test
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
      simple v;

      {
        std::ostringstream ost;
        stream<T>(v).serialize(ost, false);
        CompareSerializations<T>(ost.str(), "{\"name\":\"Hi\",\"alive\":true,\"age\":34,\"ranking\":0.125}");
      }
      
      {
        std::ostringstream ost;
        
        v.alive = false;
        v.age = 56;
        v.ranking = .5f;
        v.name = "Me";
        
        stream<T>(v).serialize(ost, false);
        CompareSerializations<T>(ost.str(), "{\"name\":\"Me\",\"alive\":false,\"age\":56,\"ranking\":0.5}");
      }
    }
  };
}

TEST(simple_object, ctors)
{
  CtorSimpleTest<impl::cjson>().TestBody();
  CtorSimpleTest<impl::rapidjson>().TestBody();
  CtorSimpleTest<impl::sajson>().TestBody();
  CtorSimpleTest<impl::nlohmannj_son>().TestBody();
  CtorSimpleTest<impl::minijson>().TestBody();
  CtorSimpleTest<impl::taojson>().TestBody();
}

TEST(simple_deserialization, ctor)
{
  DeserializationSimpleTest<impl::cjson>().TestBody();
  DeserializationSimpleTest<impl::rapidjson>().TestBody();
  DeserializationSimpleTest<impl::sajson>().TestBody();
  DeserializationSimpleTest<impl::nlohmannj_son>().TestBody();
  DeserializationSimpleTest<impl::minijson>().TestBody();
  DeserializationSimpleTest<impl::taojson>().TestBody();
}

TEST(simple_serialization, ctor)
{
  SerializationSimpleTest<impl::cjson>().TestBody();
  SerializationSimpleTest<impl::rapidjson>().TestBody();
  SerializationSimpleTest<impl::nlohmannj_son>().TestBody();
  SerializationSimpleTest<impl::minijson>().TestBody();
  SerializationSimpleTest<impl::taojson>().TestBody();
}
