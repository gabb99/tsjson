//
//  rapidjson.tests.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "document.h"     // rapidjson's DOM-style API
using namespace rapidjson;


#include <fstream>
#include <vector>

namespace
{
  std::vector<char> read(const std::string& fname)
  {
    std::ifstream file(fname);
    std::vector<char> vec;
    
    std::copy(std::istream_iterator<char>(file), std::istream_iterator<char>(), std::back_inserter(vec));
    vec.push_back('\0');
    
    return vec;
  }
}

TEST(rapidjson, canada_json)
{
  auto p = read("data/canada.json");
  
  Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
  EXPECT_FALSE(document.ParseInsitu(&p[0]).HasParseError());
}

TEST(rapidjson, citm_catalog_json)
{
  auto p = read("data/citm_catalog.json");

  Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
  EXPECT_FALSE(document.ParseInsitu(&p[0]).HasParseError());
}

TEST(rapidjson, twitter_json)
{
  auto p = read("data/twitter.json");
  
  Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
  EXPECT_FALSE(document.ParseInsitu(&p[0]).HasParseError());
}

TEST(rapidjson, jeopardy_json)
{
  auto p = read("data/jeopardy.json");
  
  Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
  EXPECT_FALSE(document.ParseInsitu(&p[0]).HasParseError());
}

TEST(rapidjson, sample_json)
{
  auto p = read("data/sample.json");
  
  Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
  EXPECT_FALSE(document.ParseInsitu(&p[0]).HasParseError());
}
