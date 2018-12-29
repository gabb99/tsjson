//
//  tsbindings.h
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-28.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tsbindings_h
#define tsbindings_h

#include <map>
#include <string>

#include "tsbase.h"

namespace tsjson
{
  class bindings;

  class JsonBindings_t
  {
  public:
    typedef std::map<std::string, tsbase*> TContainer;

    void add(const std::string& name, tsbase* object)
    {
      assert(m_bindings.find(name) == m_bindings.end());
      assert(object);

      m_bindings[name] = object;
    }

    void iterate(const std::function<void(const TContainer::value_type&)>& fct) const
    {
      for (auto&& it : m_bindings)
        if (it.second->undefined() == false)
          fct(it);
    }

  protected:
    friend class bindings;

    template <typename T, typename O>
    void assign(const std::string& name, const T& value) const
    {
      auto it = m_bindings.find(name);
      if (it != m_bindings.end())
      {
        // \todo validate O is valid for this
        O& obj(static_cast<O&>((*(it->second))));
        obj = value;
      }
    }

    void null(const std::string& name) const
    {
      auto it = m_bindings.find(name);
      if (it != m_bindings.end())
      {
        (*(it->second)).null(true);
      }
    }

    template <typename T>
    T* getChildObject(const std::string& name) const
    {
      auto it = m_bindings.find(name);
      if (it != m_bindings.end())
      {
        // \todo validate T is valid for this
        return static_cast<T*>(it->second);
      }

      return nullptr;
    }

    template <typename O>
    const bindings* getChildBindings(const std::string& name) const
    {
      auto it = m_bindings.find(name);
      if (it != m_bindings.end())
      {
        // \todo validate O is valid for this
        return &static_cast<O&>((*(it->second)))._;
      }
      
      return nullptr;
    }

  private:
    std::map<std::string, tsbase*> m_bindings;
  };

  class bindings
  {
    JsonBindings_t _;
    
  public:
    bindings() {}

    void add(const std::string& name, tsbase* object)
    {
      _.add(name, object);
    }

    void iterate(const std::function<void(const JsonBindings_t::TContainer::value_type&)>& fct) const
    {
      _.iterate(fct);
    }

    template <typename T, typename O>
    void assign(const std::string& name, const T& value) const
    {
      _.assign<T, O>(name, value);
    }

    template <typename T>
    T* getChildObject(const std::string& name) const
    {
      return _.getChildObject<T>(name);
    }
    
    void null(const std::string& name) const
    {
      _.null(name);
    }

    template <typename O>
    const bindings* getChildBindings(const std::string& name) const
    {
      return _.getChildBindings<O>(name);
    }
  };
}

#endif /* tsbindings_h */
