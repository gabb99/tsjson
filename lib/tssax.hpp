//
//  tssax.hpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-12-16.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#ifndef tssax_h
#define tssax_h

#include "tsobject.hpp"

#include <iostream>
#include <stack>
#include <string>

namespace tsjson
{
  struct sax
  {
    std::stack<std::pair<unsigned, tsbase*>> m_contextStack;
    tsbase* m_current_context;
    
    std::string m_current_key;
    unsigned m_current_indx = 0;
    
    sax(tsbase& context)
    : m_current_context(&context) {}
    
    void null()
    {
      if (m_current_context->id() == typeid(object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.null(m_current_key);
      }
      else if (m_current_context->id() == typeid(tsjson::arrayIterator))
      {
        static_cast<tsjson::arrayIterator*>(m_current_context)->null(m_current_indx++);
      }
      else
        assert(false);
    }
    
    void boolean( const bool b )
    {
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.assign<bool, tsjson::boolean>(m_current_key, b);
      }
      else if (m_current_context->id() == typeid(tsjson::arrayIterator))
      {
        static_cast<tsjson::arrayIterator*>(m_current_context)->assign<bool, tsjson::boolean>(m_current_indx++, b);
      }
      else
        assert(false);
    }
    
    void number( const double d )
    {
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.assign<double, tsjson::number>(m_current_key, d);
      }
      else if (m_current_context->id() == typeid(tsjson::arrayIterator))
      {
        static_cast<tsjson::arrayIterator*>(m_current_context)->assign<double, tsjson::number>(m_current_indx++, d);
      }
      else
        assert(false);
    }
    
    void integer( const int64_t i )
    {
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.assign<int64_t, tsjson::integer>(m_current_key, i);
      }
      else if (m_current_context->id() == typeid(tsjson::arrayIterator))
      {
        static_cast<tsjson::arrayIterator*>(m_current_context)->assign<int64_t, tsjson::integer>(m_current_indx++, i);
      }
      else
        assert(false);
    }
    
    void integer( const uint64_t u )
    {
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.assign<uint64_t, tsjson::integer>(m_current_key, u);
      }
      else if (m_current_context->id() == typeid(tsjson::arrayIterator))
      {
        static_cast<tsjson::arrayIterator*>(m_current_context)->assign<uint64_t, tsjson::integer>(m_current_indx++, u);
      }
      else
        assert(false);
    }
    
    void string( const std::string& s )
    {
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.assign<const std::string&, tsjson::string>(m_current_key, static_cast<const std::string&>(s));
      }
      else if (m_current_context->id() == typeid(tsjson::arrayIterator))
      {
        static_cast<tsjson::arrayIterator*>(m_current_context)->assign<const std::string&, tsjson::string>(m_current_indx++, static_cast<const std::string&>(s));
      }
      else
        assert(false);
    }
    
    
    void begin_array( const std::size_t s = 0 )
    {
      push_new_context();
      // resize using s
    }
    
    void end_array( const std::size_t s = 0 )
    {
      restore_context();
   }
    
    void begin_object( const std::size_t s = 0 )
    {
      // Initially empty, and top level object is anonymous
      if (m_contextStack.empty())
      {
        assert(m_current_indx == 0);
        assert(m_current_context != nullptr);
        assert(m_current_context->id() == typeid(tsjson::object_));

        // Not usefull but end_object/end_array can assert properly
        m_contextStack.push(std::make_pair(m_current_indx, m_current_context));
      }
      else
        push_new_context();
    }
    
    void end_object( const std::size_t s = 0 )
    {
      restore_context();
    }
    
    void key( const std::string& s )
    {
      m_current_key = s;
    }
    
  private:
    void push_new_context()
    {
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        assert(m_current_key.empty() == false);
        tsjson::arrayIterator* obj_context = static_cast<tsjson::objBindings*>(m_current_context)->_.getChildObject<tsjson::arrayIterator>(m_current_key);
        if (obj_context)
        {
          m_contextStack.push(std::make_pair(m_current_indx, m_current_context));
          m_current_context = obj_context;
          m_current_indx = 0;
        }
        else
          assert(false);
      }
      else if (m_current_context->id() == typeid(tsjson::arrayIterator))
      {
        tsjson::arrayIterator* obj_context = static_cast<tsjson::arrayIterator*>(m_current_context)->getChildObject<tsjson::arrayIterator>(m_current_indx++);
        if (obj_context)
        {
          m_contextStack.push(std::make_pair(m_current_indx, m_current_context));
          m_current_context = obj_context;
          m_current_indx = 0;
        }
        else
          assert(false);
      }
      else
        assert(false);
    }
    
    void restore_context()
    {
      if (m_contextStack.empty() == false)
      {
        std::tie(m_current_indx, m_current_context) = m_contextStack.top();
        m_contextStack.pop();
      }
      else
        assert(false);
    }
  };
}

#endif /* tssax_h */
