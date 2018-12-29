//
//  tsjson_sajson.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-28.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsjson_sajson_hpp
#define tsjson_sajson_hpp

#include <sstream>

#include "tsobject.hpp"
#include "tsstream.hpp"

namespace sajson
{
  void deserialize(std::istream &ist, tsjson::tsbase& root);
};

// Wrapper for templated streamer, used mainly by gTests
namespace tsjson
{
  template <>
  struct stream<impl::sajson>
  {
    tsjson::tsbase& m_root;
    
    stream(tsjson::tsbase& root) : m_root(root) {}
    
    // Default serialize/deserialize
    void deserialize(std::istream &ist)
    {
      sajson::deserialize(ist, m_root);
    }
  };
}

#endif /* tsjson_sajson_hpp */
