//
//  sajson.tests.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "sajson.h"

#include <fstream>
#include <vector>

namespace
{
  std::vector<char> read(const std::string& fname)
  {
    std::ifstream file(fname);
    std::vector<char> vec;
    
    std::copy(std::istream_iterator<char>(file), std::istream_iterator<char>(), std::back_inserter(vec));
    return vec;
  }
}

TEST(sajson, canada_json)
{
  auto p = read("data/canada.json");
  auto document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(p.size(), &p[0]));
  EXPECT_TRUE(document.is_valid());
}

TEST(sajson, citm_catalog_json)
{
  auto p = read("data/citm_catalog.json");
  auto document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(p.size(), &p[0]));
  EXPECT_TRUE(document.is_valid());
}

TEST(sajson, twitter_json)
{
  auto p = read("data/twitter.json");
  auto document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(p.size(), &p[0]));
  EXPECT_TRUE(document.is_valid());
}

TEST(sajson, jeopardy_json)
{
  auto p = read("data/jeopardy.json");
  auto document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(p.size(), &p[0]));
  EXPECT_TRUE(document.is_valid());

}

TEST(sajson, sample_json)
{
  auto p = read("data/sample.json");
  auto document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(p.size(), &p[0]));
  EXPECT_TRUE(document.is_valid());
}
