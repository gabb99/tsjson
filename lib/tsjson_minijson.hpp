//
//  tsjson_minijson.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-12-02.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsjson_minijson_hpp
#define tsjson_minijson_hpp

#include <sstream>

#include "tsobject.hpp"
#include "tsstream.hpp"

namespace minijson
{
  void serialize(std::ostream &ost, bool bPrettyPrint, const tsjson::tsbase& root);

  void deserialize(std::istream &ist, tsjson::tsbase& root);
}

// Wrapper for templated streamer, used mainly by gTests
namespace tsjson
{
  template <>
  struct stream<impl::minijson>
  {
    tsjson::tsbase& m_root;
    
    stream(tsjson::tsbase& root) : m_root(root) {}
    
    // Default serialize/deserialize
    void serialize(std::ostream &ost, bool bPrettyPrint = true)
    {
      minijson::serialize(ost, bPrettyPrint, m_root);
    }
    
    void deserialize(std::istream &ist)
    {
      minijson::deserialize(ist, m_root);
    }
  };
}

#endif /* tsjson_minijson_hpp */
