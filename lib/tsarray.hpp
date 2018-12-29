//
//  tsarray.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsarray_h
#define tsarray_h

#include "tsbindings.h"

#include <array>
#include <vector>

namespace tsjson
{
  class bindings;

  class arrayIterator : public tsbase
  {
  public:
    // ctor used when declaring this in an array
    explicit arrayIterator() : tsbase(typeid(*this)) { undefined(true); }
    
    // ctors used when declaring this in an object
    explicit arrayIterator(bindings& bindings, const std::string &name)
    : tsbase(typeid(*this)) { bindings.add(name, this); }

    virtual void iterate(const std::function<void(const tsbase&)>& fct) const = 0;
    virtual tsbase* getChildObject(unsigned indx) = 0;

    template <typename T>
    T* getChildObject(unsigned indx)
    {
      return static_cast<T*>(getChildObject(indx));
    }

    template <typename T, typename O>
    void assign(unsigned indx, const T& value)
    {
      auto it = getChildObject<O>(indx);
      if (it)
      {
        // \todo validate O is valid for this
        O& obj(static_cast<O&>(*it));
        obj = value;
      }
    }

    template <typename T>
    const bindings* getChildBindings(unsigned indx)
    {
      auto it = getChildObject<T>(indx);
      if (it)
      {
        // \todo validate T is valid for this
        return &static_cast<T&>(*it)._;
      }

      return nullptr;
    }
  };

  template<class T, std::size_t N = 0>
  class array : public arrayIterator
  {
  public:
    typedef std::array<T, N> TContainer;
    
    // ctor used when declaring this in an array
    explicit array() : arrayIterator() {}
    
    // ctors used when declaring this in an object
    explicit array(bindings& bindings, const std::string &name)
    : arrayIterator(bindings, name) {}

    typename TContainer::iterator begin() { return m_members.begin(); }
    typename TContainer::const_iterator begin() const { return m_members.begin(); }
    typename TContainer::iterator end() { return m_members.end(); }
    typename TContainer::const_iterator end() const { return m_members.end(); }

    std::size_t size() const { return N; }

    typename TContainer::reference operator[] (typename TContainer::size_type n) { return m_members[n]; }
    typename TContainer::const_reference operator[] (typename TContainer::size_type n) const { return m_members[n]; }

    tsbase* getChildObject(unsigned indx) override
    {
      if (indx < m_members.size())
        return &m_members[indx];

      return nullptr;
    }

    void iterate(const std::function<void(const tsbase&)>& fct) const override
    {
      for (auto&& e : m_members)
        if (e.undefined() == false)
          fct(e);
    }

  private:
    TContainer m_members;
  };

  template<class T>
  class array<T, 0> : public arrayIterator
  {
  public:
    typedef std::vector<T> TContainer;
    
    // ctor used when declaring this in an array
    explicit array() : arrayIterator() {}

    // ctors used when declaring this in an object
    explicit array(bindings& bindings, const std::string &name)
    : arrayIterator(bindings, name) {}

    typename TContainer::iterator begin() { return m_members.begin(); }
    typename TContainer::const_iterator begin() const { return m_members.begin(); }
    typename TContainer::iterator end() { return m_members.end(); }
    typename TContainer::const_iterator end() const { return m_members.end(); }

    void reserve(typename TContainer::size_type n) { m_members.reserve(n); }
    std::size_t size() const { return m_members.size(); }

    typename TContainer::reference operator[] (typename TContainer::size_type n) { return m_members[n]; }
    typename TContainer::const_reference operator[] (typename TContainer::size_type n) const { return m_members[n]; }

    void resize(typename TContainer::size_type n)
    {
      m_members.resize(n);
    }

    tsbase* getChildObject(unsigned indx) override
    {
      if (indx < m_members.size())
        return &m_members[indx];

      // Create elements as needed (null)
      m_members.resize(indx + 1);
      return &m_members[indx];
    }

    void iterate(const std::function<void(const tsbase&)>& fct) const override
    {
      for (auto&& e : m_members)
        if (e.undefined() == false)
          fct(e);
    }

  private:
    TContainer m_members;
  };
}

#endif /* tsarray_h */
