//
//  tsjson_nlohmannjson.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-28.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsjson_nlohmannjson_hpp
#define tsjson_nlohmannjson_hpp

#include <sstream>

#include "tsobject.hpp"
#include "tsarray.hpp"
#include "tsstream.hpp"

namespace nlohmann
{
  enum format { tjson, bson, cbor, messagepack, ubjson };

  void serialize(std::ostream &ost, bool bPrettyPrint, const tsjson::tsbase& root);
  void serialize(std::ostream &ost, const tsjson::tsbase& root, enum format = bson);

  void deserialize(std::istream &ist, tsjson::tsbase& root, enum format = tjson);
}

// Wrapper for templated streamer, used mainly by gTests
namespace tsjson
{
  template <>
  struct stream<impl::nlohmannj_son>
  {
    tsjson::tsbase& m_root;
    
    stream(tsjson::tsbase& root) : m_root(root) {}
    
    // Default serialize/deserialize
    void serialize(std::ostream &ost, bool bPrettyPrint = true)
    {
     nlohmann::serialize(ost, bPrettyPrint, m_root);
    }
    
    void deserialize(std::istream &ist)
    {
      nlohmann::deserialize(ist, m_root);
    }
  };
}

#endif /* tsjson_nlohmannjson_hpp */
