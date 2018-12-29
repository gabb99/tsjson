//
//  tsobject.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsobject_h
#define tsobject_h

#include "tsbool.hpp"
#include "tsstring.hpp"
#include "tsnumber.hpp"
#include "tsinteger.hpp"
#include "tsarray.hpp"

#include "tsbindings.h"
#include "tsobjBindings.h"

namespace tsjson
{
  // Work around difficulties with templated class and typeid()
  struct object_ {};
  
  template <class T>
  class object : public objBindings
  {
  public:
    // ctor used when declaring this in an array
    object() : objBindings(typeid(object_)) {}
    
    // ctors used when declaring this in an object
    object(bindings &_bindings, const std::string& name) : objBindings(typeid(object_)) { _bindings.add(name, this); }
  };
}

#endif /* tsobject_h */
