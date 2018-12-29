//
//  tsinteger.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsinteger_h
#define tsinteger_h

#include "tsbindings.h"

namespace tsjson
{
  // Note this behave as a simple wrapper of a integral value
  // Has also support for unsigned int
  class integer : public tsbase
  {
  public:
    // ctor used when declaring this in an array
    explicit integer()
    : tsbase(typeid(*this)), m_value(0) { undefined(true); }

    // ctors used when declaring this in an object
    explicit integer(bindings& bindings, const std::string &name)
    : tsbase(typeid(*this)), m_value(0) { bindings.add(name, this); }
    
    explicit integer(bindings& bindings, const std::string &name, int iValue)
    : tsbase(typeid(*this)), m_value(iValue) { bindings.add(name, this); }

    explicit integer(bindings& bindings, const std::string &name, unsigned int uValue)
    : tsbase(typeid(*this)), m_value(uValue) { bindings.add(name, this); }

    void swap(integer& other) { std::swap(m_value, other.m_value); tsbase::swap(other); }

    template <typename T>
    integer& operator=(T value) { null(false); undefined(false); m_value = static_cast<int>(value); return *this; }

    template <typename T>
    integer& operator+=(T value) { null(false); undefined(false); m_value += static_cast<int>(value); return *this; }

    template <typename T>
    integer& operator-=(T value) { null(false); undefined(false); m_value -= static_cast<int>(value); return *this; }

    // Since only the return type changes, will generate ambiguous warnings
    operator int() const { return m_value; }

  private:
    int m_value;
  };
}

#endif /* tsinteger_h */
