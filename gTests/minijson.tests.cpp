//
//  minijson.tests.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-12-02.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

// sample.json will exceed the default nested value (32)
#define MJR_NESTING_LIMIT 512
#include "minijson_reader.hpp"

#include <fstream>
#include <vector>

namespace
{
  void Parse(minijson::istream_context& ctx, const char* name, minijson::value& value)
  {
    switch (value.type())
    {
      case minijson::Array:
        // let ctx be a context
        minijson::parse_array(ctx, [&](minijson::value value)
                              {
                                Parse(ctx, name, value);
                              });
        break;
        
      case minijson::Object:
        // let ctx be a context
        minijson::parse_object(ctx, [&](const char* name, minijson::value value)
                               {
                                 Parse(ctx, name, value);
                               });
        break;
        
      case minijson::String:
      case minijson::Number:
      case minijson::Boolean:
      case minijson::Null:
        break;
    }
  }

  void Parse(const std::string& file)
  {
    std::filebuf fb;
    EXPECT_TRUE(fb.open (file, std::ios::in));
    std::istream ist(&fb);
    
    minijson::istream_context ctx(ist);
    
    // let ctx be a context
    minijson::parse_object(ctx, [&](const char* name, minijson::value value)
                           {
                             Parse(ctx, name, value);
                           });
  }
}


TEST(minijson, canada_json)
{
  EXPECT_NO_THROW(Parse("data/canada.json"));
}

TEST(minijson, citm_catalog_json)
{
  EXPECT_NO_THROW(Parse("data/citm_catalog.json"));
}

TEST(minijson, twitter_json)
{
  EXPECT_NO_THROW(Parse("data/twitter.json"));
}

TEST(minijson, DISABLED_jeopardy_json)
{
  // Fails, this is an array entity, which is valid
  EXPECT_NO_THROW(Parse("data/jeopardy.json"));
}

TEST(minijson, sample_json)
{
  EXPECT_NO_THROW(Parse("data/sample.json"));
}
