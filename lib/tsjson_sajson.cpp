//
//  tsjson_sajson.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-28.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "tsjson_sajson.hpp"
#include "tsobject.hpp"

#include "sajson.h"
using namespace sajson;

namespace
{
  void traverse(const std::string& name, const sajson::value& node, const tsjson::bindings& context);

  void traverse(unsigned indx, const sajson::value& node, tsjson::arrayIterator& arrayIt)
  {
    switch (node.get_type())
    {
      case TYPE_NULL:
        arrayIt.null(indx);
        break;
        
      case TYPE_FALSE:
      case TYPE_TRUE:
        arrayIt.assign<bool, tsjson::boolean>(indx, node.get_type() == TYPE_TRUE);
        break;
        
      case TYPE_ARRAY:
      {
        tsjson::arrayIterator* arr_context = arrayIt.getChildObject<tsjson::arrayIterator>(indx);
        if (arr_context)
        {
          auto length = node.get_length();
          for (size_t i = 0; i < length; ++i)
          {
            traverse(i, node.get_array_element(i), *arr_context);
          }
        }
      }
        break;
        
      case TYPE_OBJECT:
      {
        const tsjson::bindings* obj_context = arrayIt.getChildBindings<tsjson::objBindings>(indx);
        if (obj_context)
        {
          auto length = node.get_length();
          for (auto i = 0u; i < length; ++i)
          {
            traverse(node.get_object_key(i).as_string(), node.get_object_value(i), *obj_context);
          }
        }
      }
        break;
        
      case TYPE_STRING:
        arrayIt.assign<const std::string&, tsjson::string>(indx, node.as_string());
        break;
        
      case TYPE_DOUBLE:
        arrayIt.assign<double, tsjson::number>(indx, node.get_double_value());
        break;
        
      case TYPE_INTEGER:
        arrayIt.assign<double, tsjson::integer>(indx, node.get_integer_value());
        break;
        
      default:
        assert(false && "unknown node type");
    }
  }

  void traverse(const std::string& name, const sajson::value& node, const tsjson::bindings& context)
  {
    switch (node.get_type())
    {
      case TYPE_NULL:
        context.null(name);
        break;
        
      case TYPE_FALSE:
      case TYPE_TRUE:
        context.assign<bool, tsjson::boolean>(name, node.get_type() == TYPE_TRUE);
        break;
        
      case TYPE_ARRAY:
        {
          tsjson::arrayIterator* arr_context = context.getChildObject<tsjson::arrayIterator>(name);
          if (arr_context)
          {
            auto length = node.get_length();
            for (size_t i = 0; i < length; ++i)
            {
              traverse(i, node.get_array_element(i), *arr_context);
            }
          }
        }
        break;
        
      case TYPE_OBJECT:
        {
          const tsjson::bindings* obj_context = context.getChildBindings<tsjson::objBindings>(name);
          if (obj_context)
          {
            auto length = node.get_length();
            for (auto i = 0u; i < length; ++i)
            {
              traverse(node.get_object_key(i).as_string(), node.get_object_value(i), *obj_context);
            }
          }
        }
        break;
        
      case TYPE_STRING:
        context.assign<const std::string&, tsjson::string>(name, node.as_string());
        break;
        
      case TYPE_DOUBLE:
        context.assign<double, tsjson::number>(name, node.get_double_value());
        break;
  
      case TYPE_INTEGER:
        context.assign<double, tsjson::integer>(name, node.get_integer_value());
        break;
        
      default:
        assert(false && "unknown node type");
    }
  }
}

namespace sajson
{
  void deserialize(std::istream &ist, tsjson::tsbase& root)
  {
    std::vector<char> s;
    copy( std::istreambuf_iterator<char>(ist), std::istreambuf_iterator<char>(), back_inserter(s) );

    // No explicit support for SAX parsing, fallback to DOM
    auto document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(s.size(), &s[0]));
    if (document.is_valid())
    {
      auto p = document.get_root();
      if (p.get_type() == TYPE_OBJECT && root.id() == typeid(tsjson::object_))
      {
        auto length = p.get_length();
        for (auto i = 0u; i < length; ++i)
        {
          traverse(p.get_object_key(i).as_string(), p.get_object_value(i), static_cast<tsjson::objBindings&>(root)._);
        }
      }
      else if (p.get_type() == TYPE_ARRAY && root.id() == typeid(tsjson::arrayIterator))
      {
        auto length = p.get_length();
        for (auto i = 0u; i < length; ++i)
        {
          traverse(i, p.get_array_element(i), static_cast<tsjson::arrayIterator&>(root));
        }
      }
      else
        assert(false);
    }
  }
}
