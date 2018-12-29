//
//  tsjson_taocpp.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-28.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsjson_taocpp_hpp
#define tsjson_taocpp_hpp

#include <sstream>

#include "tsobject.hpp"
#include "tsarray.hpp"
#include "tsstream.hpp"

namespace tao
{
  namespace json
  {
    enum format { tjson, cbor_, messagepack, ubjson_, jaxn_ };
    
    void serialize(std::ostream &ost, bool bPrettyPrint, bool bJaxn, const tsjson::tsbase& root);
    void serialize(std::ostream &ost, const tsjson::tsbase& root, enum format = cbor_);
    
    void deserialize(std::istream &ist, tsjson::tsbase& root, enum format = tjson);
  }
}

// Wrapper for templated streamer, used mainly by gTests
namespace tsjson
{
  template <>
  struct stream<impl::taojson>
  {
    tsjson::tsbase& m_root;
    
    stream(tsjson::tsbase& root) : m_root(root) {}
    
    // Default serialize/deserialize
    void serialize(std::ostream &ost, bool bPrettyPrint = true, bool bJaxn = false)
    {
      tao::json::serialize(ost, bPrettyPrint, bJaxn, m_root);
    }
    
    void deserialize(std::istream &ist)
    {
      tao::json::deserialize(ist, m_root);
    }
  };
}

#endif /* tsjson_taocpp_hpp */
