//
//  tsjson_cjson.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-28.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsjson_cjson_hpp
#define tsjson_cjson_hpp

#include <sstream>

#include "tsobject.hpp"
#include "tsstream.hpp"

namespace cjson
{
  void serialize(std::ostream &ost, bool bPrettyPrint, const tsjson::tsbase& root);

  void deserialize(std::istream &ist, tsjson::tsbase& root);
}

// Wrapper for templated streamer, used mainly by gTests
namespace tsjson
{
  template <>
  struct stream<impl::cjson>
  {
    tsjson::tsbase& m_root;

    stream(tsjson::tsbase& root) : m_root(root) {}

    // Default serialize/deserialize
    void serialize(std::ostream &ost, bool bPrettyPrint = true)
    {
      cjson::serialize(ost, bPrettyPrint, m_root);
    }
    
    void deserialize(std::istream &ist)
    {
      cjson::deserialize(ist, m_root);
    }
  };
}

#endif /* tsjson_cjson_hpp */
