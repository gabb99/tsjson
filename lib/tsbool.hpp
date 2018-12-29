//
//  tsbool.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsbool_h
#define tsbool_h

#include "tsbindings.h"

namespace tsjson
{
  // Note this behave as a simple wrapper of a bool value
  class boolean : public tsbase
  {
  public:
    // ctor used when declaring this in an array
    explicit boolean()
    : tsbase(typeid(*this)), m_value(false) { undefined(true); }

    // ctors used when declaring this in an object
    explicit boolean(bindings& bindings, const std::string &name, bool bValue = false)
    : tsbase(typeid(*this)), m_value(bValue) { bindings.add(name, this); }

    void swap(boolean& other) { std::swap(m_value, other.m_value); tsbase::swap(other); }

    boolean& operator=(bool bValue) { null(false); undefined(false); m_value = bValue; return *this; }
    operator bool() const { return m_value; }

  private:
    bool m_value;
  };
}

#endif /* tsbool_h */
