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

//#define __trace_sax(s) std::cout << s << std::endl
#define __trace_sax(s) 

namespace tsjson
{
  struct sax
  {
    std::stack<std::pair<unsigned, tsbase*>> m_contextStack;
    tsbase* m_current_context;
    
    std::string m_current_key;
    unsigned m_current_indx = 0;
    bool m_anonymous_object = true;
    
    sax(tsbase& context)
    : m_current_context(&context) {}
    
    void null()
    {
      __trace_sax("null");
      if (m_current_context->id() == typeid(object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.null(m_current_key);
      }
      else
      {
        assert(m_current_context->id() == typeid(tsjson::arrayIterator));
        static_cast<tsjson::arrayIterator*>(m_current_context)->null(m_current_indx++);
      }
    }
    
    void boolean( const bool b )
    {
      __trace_sax("boolean " << b);
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.assign<bool, tsjson::boolean>(m_current_key, b);
      }
      else
      {
        assert(m_current_context->id() == typeid(tsjson::arrayIterator));
        static_cast<tsjson::arrayIterator*>(m_current_context)->assign<bool, tsjson::boolean>(m_current_indx++, b);
      }
    }
    
    void number( const double d )
    {
      __trace_sax("number " << d);
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.assign<double, tsjson::number>(m_current_key, d);
      }
      else
      {
        assert(m_current_context->id() == typeid(tsjson::arrayIterator));
        static_cast<tsjson::arrayIterator*>(m_current_context)->assign<double, tsjson::number>(m_current_indx++, d);
      }
    }
    
    void integer( const int64_t i )
    {
      __trace_sax("integer " << i);
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.assign<int64_t, tsjson::integer>(m_current_key, i);
      }
      else
      {
        assert(m_current_context->id() == typeid(tsjson::arrayIterator));
        static_cast<tsjson::arrayIterator*>(m_current_context)->assign<int64_t, tsjson::integer>(m_current_indx++, i);
      }
    }
    
    void integer( const uint64_t u )
    {
      __trace_sax("uinteger " << u);
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.assign<uint64_t, tsjson::integer>(m_current_key, u);
      }
      else
      {
        assert(m_current_context->id() == typeid(tsjson::arrayIterator));
        static_cast<tsjson::arrayIterator*>(m_current_context)->assign<uint64_t, tsjson::integer>(m_current_indx++, u);
      }
    }
    
    void string( const std::string& s )
    {
      __trace_sax("string " << s);
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        assert(m_current_key.empty() == false);
        static_cast<tsjson::objBindings*>(m_current_context)->_.assign<const std::string&, tsjson::string>(m_current_key, static_cast<const std::string&>(s));
      }
      else
      {
        assert(m_current_context->id() == typeid(tsjson::arrayIterator));
        static_cast<tsjson::arrayIterator*>(m_current_context)->assign<const std::string&, tsjson::string>(m_current_indx++, static_cast<const std::string&>(s));
      }
    }
    
    
    void begin_array( const std::size_t s = 0 )
    {
      __trace_sax("array[ " << m_current_indx);
      push_new_context();
      // resize using s
    }
    
    void end_array()
    {
      __trace_sax("array] " << m_current_indx);
      restore_context();
    }
    
    void begin_object()
    {
      __trace_sax("object{ " << m_current_indx);

      // Initially empty, and top level object is anonymous
      if (m_contextStack.empty())
      {
        assert(m_current_indx == 0);
        assert(m_current_context != nullptr);
        assert(m_current_context->id() == typeid(tsjson::object_));
        m_contextStack.push(std::make_pair(m_current_indx, m_current_context));
      }
      else
        push_new_context();
    }
    
    void end_object()
    {
      __trace_sax("object} " << m_current_indx);
      restore_context();
    }
    
    void key( const std::string& s )
    {
      __trace_sax("key:" << s);
      m_current_key = s;
    }
    
  private:
    void push_new_context()
    {
      if (m_current_context->id() == typeid(tsjson::object_))
      {
        __trace_sax("object{} " << (m_current_key.empty() ? "null" : m_current_key));
        tsjson::tsbase* obj_context = static_cast<tsjson::objBindings*>(m_current_context)->_.getChildObject<tsjson::tsbase>(m_current_key);
        if (obj_context)
        {
          // obj_context could be anything, objet/array or simple member (though no begin_object/array is expected)
          m_contextStack.push(std::make_pair(m_current_indx, m_current_context));
          m_current_context = obj_context;
          m_current_indx = 0;
          m_anonymous_object = obj_context->id() == typeid(tsjson::object_);
        }
        else
          // m_current_key was not declared in the bindings. Maybe a typo in the definitions ?
          assert(false);
      }
      else
      {
        __trace_sax("array[] " << m_current_indx);
        assert(m_current_context->id() == typeid(tsjson::arrayIterator));
        tsjson::tsbase* obj_context = static_cast<tsjson::arrayIterator*>(m_current_context)->getChildObject<tsjson::tsbase>(m_current_indx);
        if (obj_context)
        {
          // obj_context could be anything, objet/array or simple member (though no begin_object/array is expected)
          m_contextStack.push(std::make_pair(m_current_indx, m_current_context));
          m_current_context = obj_context;
          m_current_indx = 0;
          m_anonymous_object = obj_context->id() == typeid(tsjson::object_);
        }
        else
          // m_current_indx could not be retrieved, out of bounds for a fixed size array
          assert(false);
      }
    }
    
    void restore_context()
    {
      std::tie(m_current_indx, m_current_context) = m_contextStack.top();
      m_contextStack.pop();

      m_anonymous_object = m_current_context->id() == typeid(tsjson::arrayIterator);
      if (m_anonymous_object)
      {
        // obj_context could be anything, objet/array or simple member (though no begin_object/array is expected)
        m_contextStack.push(std::make_pair(m_current_indx++, m_current_context));
      }
    }
  };
}

#endif /* tssax_h */
