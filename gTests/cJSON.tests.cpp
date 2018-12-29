//
//  cJSON.tests.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "cJSON.h"

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

TEST(cJSON, canada_json)
{
  auto p = cJSON_Parse(&read("data/canada.json")[0]);
  EXPECT_NE(p, nullptr);
  
  cJSON_Delete(p);
}

TEST(cJSON, citm_catalog_json)
{
  auto p = cJSON_Parse(&read("data/citm_catalog.json")[0]);
  EXPECT_NE(p, nullptr);
  
  cJSON_Delete(p);
}

TEST(cJSON, twitter_json)
{
  auto p = cJSON_Parse(&read("data/twitter.json")[0]);
  EXPECT_NE(p, nullptr);
  
  cJSON_Delete(p);
}

TEST(cJSON, jeopardy_json)
{
  auto p = cJSON_Parse(&read("data/jeopardy.json")[0]);
  EXPECT_NE(p, nullptr);
  
  cJSON_Delete(p);
}

TEST(cJSON, sample_json)
{
  auto p = cJSON_Parse(&read("data/sample.json")[0]);
  EXPECT_NE(p, nullptr);
  
  cJSON_Delete(p);
}
