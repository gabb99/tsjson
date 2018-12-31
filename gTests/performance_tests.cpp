//
//  performance_tests.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-12-12.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include <fstream>      // std::ifstream

#include "tsobject.hpp"
#include "tsstream.hpp"
using namespace tsjson;

#include "tsjson_cjson.hpp"
#include "tsjson_sajson.hpp"
#include "tsjson_minijson.hpp"
#include "tsjson_taocpp.hpp"
#include "tsjson_nlohmannjson.hpp"
#include "tsjson_rapidjson.hpp"

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
  class CanadaTest : public ::testing::Test
  {
    struct geometry_t : object<struct geometry_t>
    {
      geometry_t(bindings &_bindings, const std::string& name)
      : object<struct geometry_t>(_bindings, name),
        type(object<struct geometry_t>::_, "type"),
        coordinates(object<struct geometry_t>::_, "coordinates") {}

      string  type;   // Polygon
      array<array<array<number, 2>>> coordinates;
    };

    struct properties_t : object<struct properties_t>
    {
      properties_t(bindings &_bindings, const std::string& name)
      : object<struct properties_t>(_bindings, name),
        name(object<struct properties_t>::_, "name") {}
      
      string  name;   // Canada
    };

    struct feature : object<struct feature>
    {
      feature()
      : object<struct feature>(),
        type(object<struct feature>::_, "type"),
        properties(object<struct feature>::_, "properties" ),
        geometry(object<struct feature>::_, "geometry") {}

      string  type;   // Feature
      properties_t properties;
      geometry_t geometry;
    };

    struct country : object<struct country>
    {
      country()
      : object<struct country>(),
        type(object<struct country>::_, "type"),
        features(object<struct country>::_, "features") {}
      
      string  type; // FeatureCollection
      array<feature> features;
    };

  public:
    void TestBody()
    {
      country v;
      std::ifstream ist("data/canada.json", std::ifstream::in);
      stream<T>(v).deserialize(ist);
      
      EXPECT_EQ(v.type, "FeatureCollection") << enumToString(T);
      EXPECT_TRUE(v.features.size() == 1) << enumToString(T);

      EXPECT_EQ(v.features[0].type, "Feature") << enumToString(T);
      EXPECT_EQ(v.features[0].properties.name, "Canada") << enumToString(T);

      EXPECT_EQ(v.features[0].geometry.type, "Polygon") << enumToString(T);
      EXPECT_EQ(v.features[0].geometry.coordinates.size(), 480) << enumToString(T);

      EXPECT_TRUE(v.features[0].geometry.coordinates[0].size() == 14) << enumToString(T);
      EXPECT_DOUBLE_EQ(v.features[0].geometry.coordinates[0][0][0], -65.613617) << enumToString(T);
      EXPECT_DOUBLE_EQ(v.features[0].geometry.coordinates[0][0][1],  43.420273) << enumToString(T);
    }
  };

  template <tsjson::impl T>
  class JeopardyTest : public ::testing::Test
  {
    struct question : object<struct question>
    {
      question()
      : object<struct question>(),
        category(object<struct question>::_, "category"),
        air_date(object<struct question>::_, "air_date"),
        fact(object<struct question>::_, "question"),
        value(object<struct question>::_, "value"),
        answer(object<struct question>::_, "answer"),
        round(object<struct question>::_, "round"),
        show_number(object<struct question>::_, "show_number") {}
      
      string category;
      string air_date;
      string fact;
      string value;
      string answer;
      string round;
      integer show_number;
    };
    
  public:
    void TestBody()
    {
      array<question> v;

/*
      v.reserve(216930);

       [----------] 4 tests from tsjson
       [ RUN      ] tsjson.canada_jcson
       [       OK ] tsjson.canada_jcson (266 ms)
       [ RUN      ] tsjson.canada_sajson
       [       OK ] tsjson.canada_sajson (192 ms)
       [ RUN      ] tsjson.jeopardy_jcson
       [       OK ] tsjson.jeopardy_jcson (6294 ms)
       [ RUN      ] tsjson.jeopardy_sajson
       [       OK ] tsjson.jeopardy_sajson (6240 ms)
       [----------] 4 tests from tsjson (12993 ms total)

vs no reserve()
       [----------] 4 tests from tsjson
       [ RUN      ] tsjson.canada_jcson
       [       OK ] tsjson.canada_jcson (259 ms)
       [ RUN      ] tsjson.canada_sajson
       [       OK ] tsjson.canada_sajson (181 ms)
       [ RUN      ] tsjson.jeopardy_jcson
       [       OK ] tsjson.jeopardy_jcson (6379 ms)
       [ RUN      ] tsjson.jeopardy_sajson
       [       OK ] tsjson.jeopardy_sajson (6342 ms)
       [----------] 4 tests from tsjson (13161 ms total)
*/
      std::ifstream ist("data/jeopardy.json", std::ifstream::in);
      stream<T>(v).deserialize(ist);

      EXPECT_TRUE(v.size() == 216930) << enumToString(T);
    }
  };
}


TEST(tsjson, canada_cjson)
{
  CanadaTest<impl::cjson>().TestBody();
}

TEST(tsjson, canada_sajson)
{
  CanadaTest<impl::sajson>().TestBody();
}

TEST(tsjson, canada_minijson)
{
  CanadaTest<impl::minijson>().TestBody();
}

TEST(tsjson, canada_taojson)
{
  CanadaTest<impl::taojson>().TestBody();
}

TEST(tsjson, canada_rapidjson)
{
  CanadaTest<impl::rapidjson>().TestBody();
}

TEST(tsjson, canada_nlohmannjson)
{
  CanadaTest<impl::nlohmannj_son>().TestBody();
}


TEST(tsjson, jeopardy_cjson)
{
  JeopardyTest<impl::cjson>().TestBody();
}

TEST(tsjson, jeopardy_sajson)
{
  JeopardyTest<impl::sajson>().TestBody();
}

TEST(tsjson, jeopardy_minijson)
{
  JeopardyTest<impl::minijson>().TestBody();
}

TEST(tsjson, DISABLED_jeopardy_taojson)
{
  JeopardyTest<impl::taojson>().TestBody();
}

TEST(tsjson, DISABLED_jeopardy_rapidjson)
{
  JeopardyTest<impl::rapidjson>().TestBody();
}

TEST(tsjson, DISABLED_jeopardy_nlohmannjson)
{
  JeopardyTest<impl::nlohmannj_son>().TestBody();
}
