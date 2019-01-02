//
//  tsjson_cjson.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-28.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "tsjson_cjson.hpp"
#include "tsobject.hpp"

#include <iostream>
#include <vector>
#include <assert.h>

#include "cJSON.h"

namespace
{
  void serialize_impl(cJSON* object, const tsjson::bindings& context);

  void serialize_impl(cJSON* array, const tsjson::arrayIterator& arrayIt)
  {
    arrayIt.iterate([&](const tsjson::tsbase& element)
                    {
                      if (element.null())
                      {
                        cJSON_AddItemToArray(array, cJSON_CreateNull());
                      }

                      else if (element.id() == typeid(tsjson::boolean))
                      {
                        cJSON_bool bValue = static_cast<const tsjson::boolean&>(element).operator bool();
                        cJSON_AddItemToArray(array, cJSON_CreateBool(bValue));
                      }

                      else if (element.id() == typeid(tsjson::string))
                      {
                        auto&& sValue = static_cast<const tsjson::string&>(element).operator const std::string&();
                        cJSON_AddItemToArray(array, cJSON_CreateStringReference(sValue.c_str()));
                      }
                      
                      else if (element.id() == typeid(tsjson::integer))
                      {
                        auto&& iValue = static_cast<const tsjson::integer&>(element).operator int();
                        cJSON_AddItemToArray(array, cJSON_CreateNumber(iValue));
                      }
                      
                      else if (element.id() == typeid(tsjson::number))
                      {
                        auto&& dValue = static_cast<const tsjson::number&>(element).operator double();
                        cJSON_AddItemToArray(array, cJSON_CreateNumber(dValue));
                      }

                      else if (element.id() == typeid(tsjson::arrayIterator))
                      {
                        auto p = cJSON_CreateArray();
                        cJSON_AddItemToArray(array, p);

                        serialize_impl(p, static_cast<const tsjson::arrayIterator&>(element));
                      }

                      else
                      {
                        assert(element.id() == typeid(tsjson::object_));

                        auto p = cJSON_CreateObject();
                        cJSON_AddItemToArray(array, p);

                        serialize_impl(p, static_cast<const tsjson::objBindings&>(element)._);
                      }
                    });
  }

  void serialize_impl(cJSON* object, const tsjson::bindings& context)
  {
    context.iterate([&](const tsjson::JsonBindings_t::TContainer::value_type& it)
                    {
                      if (it.second->null())
                      {
                        cJSON_AddNullToObject(object, it.first.c_str());
                      }

                      else if (it.second->id() == typeid(tsjson::boolean))
                      {
                        cJSON_bool bValue = static_cast<const tsjson::boolean&>(*it.second).operator bool();
                        cJSON_AddBoolToObject(object, it.first.c_str(), bValue);
                      }

                      else if (it.second->id() == typeid(tsjson::string))
                      {
                        auto&& sValue = static_cast<const tsjson::string&>(*it.second).operator const std::string&();
                        cJSON_AddItemToObjectCS(object, it.first.c_str(), cJSON_CreateStringReference(sValue.c_str()));
                      }

                      else if (it.second->id() == typeid(tsjson::integer))
                      {
                        auto&& iValue = static_cast<const tsjson::integer&>(*it.second).operator int();
                        cJSON_AddNumberToObject(object, it.first.c_str(), iValue);
                      }
                      
                      else if (it.second->id() == typeid(tsjson::number))
                      {
                        auto&& dValue = static_cast<const tsjson::number&>(*it.second).operator double();
                        cJSON_AddNumberToObject(object, it.first.c_str(), dValue);
                      }

                      else if (it.second->id() == typeid(tsjson::arrayIterator))
                      {
                        serialize_impl(cJSON_AddArrayToObject(object, it.first.c_str()),
                                        static_cast<const tsjson::arrayIterator&>(*it.second));
                      }

                      else
                      {
                        assert(it.second->id() == typeid(tsjson::object_));

                        serialize_impl(cJSON_AddObjectToObject(object, it.first.c_str()),
                                       static_cast<const tsjson::objBindings&>(*it.second)._);
                      }
                    });
  }


  void deserialize_impl(cJSON* object, const tsjson::bindings& context);

  void deserialize_impl(cJSON* object, tsjson::arrayIterator& arrayIt)
  {
    // Traverse p
    cJSON *current_element = NULL;
    unsigned element_index = 0;
    cJSON_ArrayForEach(current_element, object)
    {
      assert(current_element->string == nullptr);
      
      /* do something with the key */
      switch (current_element->type)
      {
        case cJSON_Invalid:
          break;
          
        case cJSON_False:
        case cJSON_True:
          arrayIt.assign<bool, tsjson::boolean>(element_index, current_element->type == cJSON_True);
          break;
          
        case cJSON_NULL:
          arrayIt.null(element_index);
          break;
          
        case cJSON_Number:
          if (current_element->valuedouble != current_element->valueint)
            arrayIt.assign<double, tsjson::number>(element_index, current_element->valuedouble);
          else
            arrayIt.assign<int, tsjson::integer>(element_index, current_element->valueint);
          break;
          
        case cJSON_String:
          arrayIt.assign<const char*, tsjson::string>(element_index, current_element->valuestring);
          break;
          
        case cJSON_Array:
          {
            tsjson::arrayIterator* arr_context = arrayIt.getChildObject<tsjson::arrayIterator>(element_index);
            if (arr_context)
            {
              deserialize_impl(current_element, *arr_context);
            }
          }
          break;
          
        case cJSON_Object:
          {
            const tsjson::bindings* obj_context = arrayIt.getChildBindings<tsjson::objBindings>(element_index);
            if (obj_context)
            {
              deserialize_impl(current_element, *obj_context);
            }
          }
          break;
      }
      
      element_index++;
    }
  }

  void deserialize_impl(cJSON* object, const tsjson::bindings& context)
  {
    // Traverse p
    cJSON *current_element = NULL;
    cJSON_ArrayForEach(current_element, object)
    {
      assert(current_element->string);

      /* do something with the key */
      switch (current_element->type)
      {
        case cJSON_Invalid:
          break;
          
        case cJSON_False:
        case cJSON_True:
          context.assign<bool, tsjson::boolean>(current_element->string, current_element->type == cJSON_True);
          break;
          
        case cJSON_NULL:
          context.null(current_element->string);
          break;
          
        case cJSON_Number:
          if (current_element->valuedouble != current_element->valueint)
            context.assign<double, tsjson::number>(current_element->string, current_element->valuedouble);
          else
            context.assign<int, tsjson::integer>(current_element->string, current_element->valueint);
          break;
          
        case cJSON_String:
          context.assign<const char*, tsjson::string>(current_element->string, current_element->valuestring);
          break;
          
        case cJSON_Array:
          {
            tsjson::arrayIterator* arr_context = context.getChildObject<tsjson::arrayIterator>(current_element->string);
            if (arr_context)
            {
              deserialize_impl(current_element, *arr_context);
            }
          }
          break;
          
        case cJSON_Object:
          {
            const tsjson::bindings* obj_context = context.getChildBindings<tsjson::objBindings>(current_element->string);
            if (obj_context)
            {
              deserialize_impl(current_element, *obj_context);
            }
          }
          break;
      }
    }
  }
}

namespace cjson
{
  void serialize(std::ostream &ost, bool bPrettyPrint, const tsjson::tsbase& root)
  {
    cJSON* p = nullptr;

    if (root.id() == typeid(tsjson::object_))
    {
      serialize_impl(p = cJSON_CreateObject(), static_cast<const tsjson::objBindings&>(root)._);
    }
    else
    {
      assert(root.id() == typeid(tsjson::arrayIterator));
      serialize_impl(p = cJSON_CreateArray(), static_cast<const tsjson::arrayIterator&>(root));
    }

    ost << (bPrettyPrint ? cJSON_Print(p) : cJSON_PrintUnformatted(p));
    cJSON_Delete(p);
  }

  void deserialize(std::istream &ist, tsjson::tsbase& root)
  {
    std::vector<char> s;
    copy( std::istreambuf_iterator<char>(ist), std::istreambuf_iterator<char>(), back_inserter(s) );
    
    // No explicit support for SAX parsing, fallback to DOM
    auto p = cJSON_Parse(&s[0]);
    assert(p);

    if (root.id() == typeid(tsjson::object_) && p->type == cJSON_Object)
      deserialize_impl(p, static_cast<tsjson::objBindings&>(root)._);
    else if (root.id() == typeid(tsjson::arrayIterator) && p->type == cJSON_Array)
      deserialize_impl(p, static_cast<tsjson::arrayIterator&>(root));
    else
      // mis-match between top level object/array
      assert(false);

    cJSON_Delete(p);
  }
}
