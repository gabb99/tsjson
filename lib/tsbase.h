//
//  tsbase.h
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-29.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsbase_h
#define tsbase_h

#include <typeinfo>

namespace tsjson
{
  class tsbase
  {
    bool m_null = false;
    bool m_undefined = false;

    const std::type_info& m_type_info;

  public:
    tsbase(const std::type_info& t) : m_type_info(t) {}
    virtual ~tsbase() {}

    const std::type_info& id() const { return m_type_info; }

    bool null() const { return m_null; }
    bool undefined() const { return m_undefined; }

    void null(bool isNull) { m_null = isNull; }
    void undefined(bool isUndefined) { m_undefined = isUndefined; }

    void swap(tsbase& other) { std::swap(m_undefined, other.m_undefined);  std::swap(m_null, other.m_null); }
};

  template< class T>
  void swap(T& l, T& r)
  {
    l.swap(r);
  }
}

#endif /* tsbase_h */
