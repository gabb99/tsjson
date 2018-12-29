//
//  tsobjBindings.h
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-29.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsobjBindings_h
#define tsobjBindings_h

#include "tsbase.h"

namespace tsjson
{
  struct objBindings : public tsbase
  {
    objBindings(const std::type_info& t) : tsbase(t) {}
    
    // Exposed a public member, don't know how to make opaque
    bindings _;
  };
}

#endif /* tsobjBindings_h */
