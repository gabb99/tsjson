//
//  tsnumber.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsnumber_h
#define tsnumber_h

#include "tsbindings.h"

namespace tsjson
{
  // Note this behave as a simple wrapper of a float(double) value
  class number : public tsbase
  {
  public:
    // ctor used when declaring this in an array
    explicit number()
    : tsbase(typeid(*this)), m_value(.0) { undefined(true); }

    // ctors used when declaring this in an object
    explicit number(bindings& bindings, const std::string &name, double fValue = .0)
    : tsbase(typeid(*this)), m_value(fValue) { bindings.add(name, this); }

    void swap(number& other) { std::swap(m_value, other.m_value);  tsbase::swap(other); }

    number& operator=(double fValue) { null(false); undefined(false); m_value = fValue; return *this; }
    number& operator+=(double fValue) { null(false); undefined(false); m_value += fValue; return *this; }
    number& operator-=(double fValue) { null(false); undefined(false); m_value -= fValue; return *this; }

    operator double() const { return m_value; }
    
  private:
    double m_value;
  };
}

#endif /* tsnumber_h */
