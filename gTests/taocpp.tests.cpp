//
//  taocpp.tests.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"

#include "tao/json.hpp"

TEST(taocpp, canada_json)
{
  const tao::json::value& v = tao::json::parse_file( "data/canada.json" );
  EXPECT_TRUE(v.is_object());
}

TEST(taocpp, citm_catalog_json)
{
  const tao::json::value& v = tao::json::parse_file( "data/citm_catalog.json" );
  EXPECT_TRUE(v.is_object());
}

TEST(taocpp, twitter_json)
{
  const tao::json::value& v = tao::json::parse_file( "data/twitter.json" );
  EXPECT_TRUE(v.is_object());
}

TEST(taocpp, jeopardy_json)
{
  const tao::json::value& v = tao::json::parse_file( "data/jeopardy.json" );
  EXPECT_TRUE(v.is_array());
}

TEST(taocpp, sample_json)
{
  const tao::json::value& v = tao::json::parse_file( "data/sample.json" );
  EXPECT_TRUE(v.is_object());
}


TEST(taocpp, sample_msgpack)
{
  const tao::json::value& v = tao::json::msgpack::parse_file( "data/sample.json.msgpack" );
  EXPECT_TRUE(v.is_object());
}


TEST(taocpp, sample_ubjson)
{
  const tao::json::value& v = tao::json::ubjson::parse_file( "data/sample.json.ubjson" );
  EXPECT_TRUE(v.is_object());
}


TEST(taocpp, sample_cbor)
{
  const tao::json::value& v = tao::json::cbor::parse_file( "data/sample.json.cbor" );
  EXPECT_TRUE(v.is_object());
}

TEST(taocpp, unicode_cbor)
{
  const tao::json::value& v = tao::json::cbor::parse_file( "data/all_unicode.json.cbor" );
  EXPECT_TRUE(v.is_array());
}

TEST(taocpp, signed_ints_cbor)
{
  const tao::json::value& v = tao::json::cbor::parse_file( "data/signed_ints.json.cbor" );
  EXPECT_TRUE(v.is_array());
}

TEST(taocpp, floats_cbor)
{
  const tao::json::value& v = tao::json::cbor::parse_file( "data/floats.json.cbor" );
  EXPECT_TRUE(v.is_array());
}

TEST(taocpp, unsigned_ints_cbor)
{
  const tao::json::value& v = tao::json::cbor::parse_file( "data/unsigned_ints.json.cbor" );
  EXPECT_TRUE(v.is_array());
}
