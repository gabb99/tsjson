//
//  tsstream.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-12-13.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsstream_h
#define tsstream_h

namespace tsjson
{
  enum class impl { cjson, rapidjson, sajson, nlohmannj_son, minijson, taojson };

  template <impl I>
  struct stream
  {
    // Will always be partially specialized anyways, so no need for methods/members here
  };
}

#endif /* tsstream_h */
