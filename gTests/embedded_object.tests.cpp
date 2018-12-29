//
//  embedded_object.tests.cpp
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
  class CtorObjectTest : public ::testing::Test
  {
    struct simple : object<struct simple>
    {
      simple(bindings &_bindings, const std::string& name)
      : object<struct simple>(_bindings, name),
        alive(object<struct simple>::_, "alive", true),
        age(object<struct simple>::_, "age", 34u),
        ranking(object<struct simple>::_, "ranking", 0.125f),
        name(object<struct simple>::_, "name", "Hi") {}
      
      boolean alive;
      integer age;
      number  ranking;
      string  name;
    };

    struct embedded : object<struct embedded>
    {
      embedded()
      : object<struct embedded>(),
        alive(object<struct embedded>::_, "alive", false),
        age(object<struct embedded>::_, "age", 56u),
        ranking(object<struct embedded>::_, "ranking", 0.5f),
        name(object<struct embedded>::_, "name", "Me"),
        simple(object<struct embedded>::_, "simple") {}
      
      boolean alive;
      integer age;
      number  ranking;
      string  name;
      
      simple  simple;
    };

  public:
    void TestBody()
    {
      struct embedded v;
      
      EXPECT_TRUE(v.alive == false) << enumToString(T);
      EXPECT_TRUE(v.age == 56u) << enumToString(T);
      EXPECT_DOUBLE_EQ(v.ranking, .5f) << enumToString(T);
      EXPECT_TRUE(v.name == "Me") << enumToString(T);
      
      EXPECT_EQ(v.simple.alive, true) << enumToString(T);
      EXPECT_EQ(v.simple.age, 34u) << enumToString(T);
      EXPECT_DOUBLE_EQ(v.simple.ranking, 0.125f) << enumToString(T);
      EXPECT_EQ(v.simple.name, "Hi") << enumToString(T);
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
  class SerializationEmbeddedTest : public ::testing::Test
  {
    struct simple : object<struct simple>
    {
      simple(bindings &_bindings, const std::string& name)
      : object<struct simple>(_bindings, name),
        alive(object<struct simple>::_, "alive", true),
        age(object<struct simple>::_, "age", 34u),
        ranking(object<struct simple>::_, "ranking", 0.125f),
        name(object<struct simple>::_, "name", "Hi") {}
      
      boolean alive;
      integer age;
      number  ranking;
      string  name;
    };
    
    struct embedded : object<struct embedded>
    {
      embedded()
      : object<struct embedded>(),
        alive(object<struct embedded>::_, "alive", false),
        age(object<struct embedded>::_, "age", 56u),
        ranking(object<struct embedded>::_, "ranking", 0.5f),
        name(object<struct embedded>::_, "name", "Me"),
        simple(object<struct embedded>::_, "simple") {}
      
      boolean alive;
      integer age;
      number  ranking;
      string  name;
      
      simple  simple;
    };

  public:
    void TestBody()
    {
      embedded v;
      
      {
        std::ostringstream ost;
        stream<T>(v).serialize(ost, false);
        CompareSerializations<T>(ost.str(), "{ \"simple\":{ \"name\":\"Hi\", \"alive\":true, \"age\":34, \"ranking\":0.125 },"
                                            "\"name\":\"Me\", \"alive\":false, \"age\":56, \"ranking\":0.5 }");
      }
      
      {
        std::ostringstream ost;
        
        v.alive.swap(v.simple.alive);
        swap(v.age, v.simple.age);
        v.ranking.swap(v.simple.ranking);
        swap(v.name, v.simple.name);

        stream<T>(v).serialize(ost, false);
        CompareSerializations<T>(ost.str(), "{ \"simple\":{ \"name\":\"Me\", \"alive\":false, \"age\":56, \"ranking\":0.5 },"
                                            "\"name\":\"Hi\", \"alive\":true, \"age\":34, \"ranking\":0.125 }");
      }
    }
  };

  template <tsjson::impl T>
  class DeserializationEmbeddedTest : public ::testing::Test
  {
    struct simple : object<struct simple>
    {
      simple(bindings &_bindings, const std::string& name)
      : object<struct simple>(_bindings, name),
        alive(object<struct simple>::_, "alive", false),
        age(object<struct simple>::_, "age", 56u),
        ranking(object<struct simple>::_, "ranking", 0.5f),
        name(object<struct simple>::_, "name", "Me") {}
      
      boolean alive;
      integer age;
      number  ranking;
      string  name;
    };
    
    struct embedded : object<struct embedded>
    {
      embedded()
      : object<struct embedded>(),
        alive(object<struct embedded>::_, "alive", true),
        age(object<struct embedded>::_, "age", 34u),
        ranking(object<struct embedded>::_, "ranking", 0.125f),
        name(object<struct embedded>::_, "name", "Hi"),
        simple(object<struct embedded>::_, "simple") {}
      
      boolean alive;
      integer age;
      number  ranking;
      string  name;
      
      simple  simple;
    };

  public:
    void TestBody()
    {
      embedded v;
      std::istringstream ist("{ \"simple\":{ \"name\":\"Hi\", \"alive\":true, \"age\":34, \"ranking\":0.125 },"
                             "\"name\":\"Me\", \"alive\":false, \"age\":56, \"ranking\":0.5 }");
      
      stream<T>(v).deserialize(ist);
      EXPECT_TRUE(v.alive == false) << enumToString(T);
      EXPECT_TRUE(v.age == 56u) << enumToString(T);
      EXPECT_DOUBLE_EQ(v.ranking, .5f) << enumToString(T);
      EXPECT_TRUE(v.name == "Me") << enumToString(T);

      EXPECT_TRUE(v.simple.alive == true) << enumToString(T);
      EXPECT_TRUE(v.simple.age == 34u) << enumToString(T);
      EXPECT_DOUBLE_EQ(v.simple.ranking, .125f) << enumToString(T);
      EXPECT_TRUE(v.simple.name == "Hi") << enumToString(T);
    }
  };

}


TEST(embedded_object, ctor)
{
  CtorObjectTest<impl::cjson>().TestBody();
  CtorObjectTest<impl::rapidjson>().TestBody();
  CtorObjectTest<impl::sajson>().TestBody();
  CtorObjectTest<impl::nlohmannj_son>().TestBody();
  CtorObjectTest<impl::minijson>().TestBody();
  CtorObjectTest<impl::taojson>().TestBody();
}

TEST(embedded_deserialization, ctor)
{
  DeserializationEmbeddedTest<impl::cjson>().TestBody();
  DeserializationEmbeddedTest<impl::rapidjson>().TestBody();
  DeserializationEmbeddedTest<impl::sajson>().TestBody();
  DeserializationEmbeddedTest<impl::nlohmannj_son>().TestBody();
  DeserializationEmbeddedTest<impl::minijson>().TestBody();
  DeserializationEmbeddedTest<impl::taojson>().TestBody();
}

TEST(embedded_serialization, ctor)
{
  SerializationEmbeddedTest<impl::cjson>().TestBody();
  SerializationEmbeddedTest<impl::rapidjson>().TestBody();
  SerializationEmbeddedTest<impl::nlohmannj_son>().TestBody();
  SerializationEmbeddedTest<impl::minijson>().TestBody();
  SerializationEmbeddedTest<impl::taojson>().TestBody();
}
