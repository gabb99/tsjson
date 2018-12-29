//
//  tsstring.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsstring_h
#define tsstring_h

#include "tsbindings.h"

namespace tsjson
{
  // Note this behave as a simple wrapper of a char[]/std::string value
  class string : public tsbase
  {
  public:
    // ctor used when declaring this in an array
    explicit string() : tsbase(typeid(*this)) { undefined(true); }

    explicit string(bindings& bindings, const std::string &name) : tsbase(typeid(*this)) { bindings.add(name, this); }

    // ctors used when declaring this in an object
    explicit string(bindings& bindings, const std::string &name, const std::string& sValue)
    : tsbase(typeid(*this)), m_value(sValue) { bindings.add(name, this); }

    explicit string(bindings& bindings, const std::string &name, const char* sValue)
    : tsbase(typeid(*this)), m_value(sValue) { bindings.add(name, this); }

    void swap(string& other) { std::swap(m_value, other.m_value); tsbase::swap(other); }
    
    bool operator==(const char* rhs) const { return m_value == rhs; }

    string& operator=(const std::string& sValue) { null(false); undefined(false); m_value.assign(sValue); return *this; }
    string& operator=(const char* sValue) { null(false); undefined(false); m_value.assign(sValue); return *this; }
    string& operator+=(const std::string& sValue) { null(false); undefined(false); m_value.append(sValue); return *this; }
    string& operator+=(const char* sValue) { null(false); undefined(false); m_value.append(sValue); return *this; }

    operator const std::string&() const { return m_value; }
    operator const char*() const { return m_value.c_str(); }

  private:
    std::string m_value;
  };
}

#endif /* tsstring_h */
