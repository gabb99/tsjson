//
//  nlohmann.tests.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include <fstream>
#include <vector>

namespace
{
  std::vector<char> read_text(const std::string& fname)
  {
    std::ifstream file(fname);
    std::vector<char> vec;
    
    std::copy(std::istream_iterator<char>(file), std::istream_iterator<char>(), std::back_inserter(vec));
    vec.push_back('\0');
    
    return vec;
  }

  std::vector<uint8_t> read_binary(const std::string& fname)
  {
    std::ifstream file(fname, std::ios::binary);
    std::vector<uint8_t> vec;
    
    std::copy(std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>(), std::back_inserter(vec));
    return vec;
  }
}

TEST(nlohmann, canada_json)
{
  auto p = read_text("data/canada.json");

  json j = json::parse(p);
  EXPECT_FALSE(j.empty());
}

TEST(nlohmann, citm_catalog_json)
{
  auto p = read_text("data/citm_catalog.json");

  json j = json::parse(p);
  EXPECT_FALSE(j.empty());
}

TEST(nlohmann, twitter_json)
{
  auto p = read_text("data/twitter.json");

  json j = json::parse(p);
  EXPECT_FALSE(j.empty());
}

TEST(nlohmann, jeopardy_json)
{
  auto p = read_text("data/jeopardy.json");

  json j = json::parse(p);
  EXPECT_FALSE(j.empty());
}

TEST(nlohmann, sample_json)
{
  auto p = read_text("data/sample.json");
  
  auto j = json::parse(p);
  EXPECT_FALSE(j.empty());
}


TEST(nlohmann, sample_msgpack)
{
  auto p = read_binary("data/sample.json.msgpack");
  
  // deserialize it with msgPack
  auto j = json::from_msgpack(p);
  EXPECT_FALSE(j.empty());
}


TEST(nlohmann, sample_ubjson)
{
  auto p = read_binary("data/sample.json.ubjson");
  
  // deserialize it with ubjson
  auto j = json::from_ubjson(p);
  EXPECT_FALSE(j.empty());
}


TEST(nlohmann, sample_cbor)
{
  auto p = read_binary("data/sample.json.cbor");
  
  // deserialize it with CBOR
  auto j = json::from_cbor(p);
  EXPECT_FALSE(j.empty());
}

TEST(nlohmann, unicode_cbor)
{
  auto p = read_binary("data/all_unicode.json.cbor");
  
  // deserialize it with CBOR
  auto j = json::from_cbor(p);
  EXPECT_FALSE(j.empty());
}

TEST(nlohmann, signed_ints_cbor)
{
  auto p = read_binary("data/signed_ints.json.cbor");
  
  // deserialize it with CBOR
  auto j = json::from_cbor(p);
  EXPECT_FALSE(j.empty());
}

TEST(nlohmann, floats_cbor)
{
  auto p = read_binary("data/floats.json.cbor");
  
  // deserialize it with CBOR
  auto j = json::from_cbor(p);
  EXPECT_FALSE(j.empty());
}

TEST(nlohmann, unsigned_ints_cbor)
{
  auto p = read_binary("data/unsigned_ints.json.cbor");
  
  // deserialize it with CBOR
  auto j = json::from_cbor(p);
  EXPECT_FALSE(j.empty());
}
