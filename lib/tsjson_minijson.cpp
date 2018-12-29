//
//  tsjson_minijson.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-12-02.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "tsjson_minijson.hpp"
#include "tsobject.hpp"

#include "minijson_reader.hpp"
#include "minijson_writer.hpp"

namespace
{
  void serialize_impl(minijson::object_writer &writer, const tsjson::bindings& context);
  
  void serialize_impl(minijson::array_writer &writer, const tsjson::arrayIterator& arrayIt)
  {
    arrayIt.iterate([&](const tsjson::tsbase& element)
                    {
                      if (element.null())
                      {
                        writer.write(minijson::null); // null type (on C++11 you can use nullptr)
                      }
                      
                      else if (element.id() == typeid(tsjson::boolean))
                      {
                        writer.write(static_cast<const tsjson::boolean&>(element).operator bool());
                      }
                      
                      else if (element.id() == typeid(tsjson::string))
                      {
                        writer.write(static_cast<const tsjson::string&>(element).operator const char*());
                      }
                      
                      else if (element.id() == typeid(tsjson::integer))
                      {
                        writer.write(static_cast<const tsjson::integer&>(element).operator int());
                      }
                      
                      else if (element.id() == typeid(tsjson::number))
                      {
                        writer.write(static_cast<const tsjson::number&>(element).operator double());
                      }
                      
                      else if (element.id() == typeid(tsjson::arrayIterator))
                      {
                        minijson::array_writer position_writer = writer.nested_array();
                        
                        serialize_impl(position_writer,
                                       static_cast<const tsjson::arrayIterator&>(element));

                        position_writer.close();
                      }
                      
                      else if (element.id() == typeid(tsjson::object_))
                      {
                        minijson::object_writer position_writer = writer.nested_object();
                        
                        serialize_impl(position_writer,
                                       static_cast<const tsjson::objBindings&>(element)._);
                        
                        position_writer.close();
                      }
                      
                      else
                        assert(false);
                    });
    
    writer.close();
  }

  void serialize_impl(minijson::object_writer &writer, const tsjson::bindings& context)
  {
    context.iterate([&](const tsjson::JsonBindings_t::TContainer::value_type& it)
                    {
                      if (it.second->null())
                      {
                        writer.write(it.first.c_str(), minijson::null); // null type (on C++11 you can use nullptr)
                      }
                      
                      else if (it.second->id() == typeid(tsjson::boolean))
                      {
                        writer.write(it.first.c_str(), static_cast<tsjson::boolean&>(*it.second).operator bool());
                      }
                      
                      else if (it.second->id() == typeid(tsjson::string))
                      {
                        writer.write(it.first.c_str(), static_cast<tsjson::string&>(*it.second).operator const char*());
                      }
                      
                      else if (it.second->id() == typeid(tsjson::integer))
                      {
                        writer.write(it.first.c_str(), static_cast<tsjson::integer&>(*it.second).operator int());
                      }
                      
                      else if (it.second->id() == typeid(tsjson::number))
                      {
                        writer.write(it.first.c_str(), static_cast<tsjson::number&>(*it.second).operator double());
                      }

                      else if (it.second->id() == typeid(tsjson::arrayIterator))
                      {
                        minijson::array_writer position_writer = writer.nested_array(it.first.c_str());

                        serialize_impl(position_writer,
                                       static_cast<tsjson::arrayIterator&>(*it.second));

                        position_writer.close();
                      }

                      else if (it.second->id() == typeid(tsjson::object_))
                      {
                        minijson::object_writer position_writer = writer.nested_object(it.first.c_str());

                        serialize_impl(position_writer,
                                       static_cast<tsjson::objBindings&>(*it.second)._);

                        position_writer.close();
                      }
                      
                      else
                        assert(false);
                    });

    writer.close();
  }

  void Parse(minijson::istream_context& ctx, const tsjson::bindings& context, const char* name, const minijson::value& value);

  void Parse(minijson::istream_context& ctx, tsjson::arrayIterator& arrayIt, unsigned indx, const minijson::value& value)
  {
    switch (value.type())
    {
      case minijson::Array:
      {
        tsjson::arrayIterator* arr_context = arrayIt.getChildObject<tsjson::arrayIterator>(indx);
        if (arr_context)
        {
          // let ctx be a context
          unsigned indx = 0;
          minijson::parse_array(ctx, [&](const minijson::value& value)
                                {
                                  Parse(ctx, *arr_context, indx++, value);
                                });
        }
      }
        break;
        
      case minijson::Object:
      {
        const tsjson::bindings* obj_context = arrayIt.getChildBindings<tsjson::objBindings>(indx);
        if (obj_context)
        {
          // let ctx be a context
          minijson::parse_object(ctx, [&](const char* name, const minijson::value& value)
                                 {
                                   Parse(ctx, *obj_context, name, value);
                                 });
        }
      }
        break;
        
      case minijson::String:
        arrayIt.assign<const char*, tsjson::string>(indx, value.as_string());
        break;
        
      case minijson::Number:
        if (value.as_double() != value.as_long())
          arrayIt.assign<double, tsjson::number>(indx, value.as_double());
        else
          arrayIt.assign<int, tsjson::integer>(indx, value.as_long());
        break;
        
      case minijson::Boolean:
        arrayIt.assign<bool, tsjson::boolean>(indx, value.as_bool());
        break;
        
      case minijson::Null:
        arrayIt.null(indx);
        break;
    }
  }

  void Parse(minijson::istream_context& ctx, const tsjson::bindings& context, const char* name, const minijson::value& value)
  {
    switch (value.type())
    {
      case minijson::Array:
        {
          tsjson::arrayIterator* arr_context = context.getChildObject<tsjson::arrayIterator>(name);
          if (arr_context)
          {
            // let ctx be a context
            unsigned indx = 0;
            minijson::parse_array(ctx, [&](const minijson::value& value)
                                  {
                                    Parse(ctx, *arr_context, indx++, value);
                                  });
          }
        }
        break;
        
      case minijson::Object:
        {
          const tsjson::bindings* obj_context = context.getChildBindings<tsjson::objBindings>(name);
          if (obj_context)
          {
            // let ctx be a context
            minijson::parse_object(ctx, [&](const char* name, const minijson::value& value)
                                   {
                                     Parse(ctx, *obj_context, name, value);
                                   });
          }
        }
        break;
        
      case minijson::String:
        context.assign<const char*, tsjson::string>(name, value.as_string());
        break;

      case minijson::Number:
        if (value.as_double() != value.as_long())
          context.assign<double, tsjson::number>(name, value.as_double());
        else
          context.assign<int, tsjson::integer>(name, value.as_long());
        break;

      case minijson::Boolean:
        context.assign<bool, tsjson::boolean>(name, value.as_bool());
        break;

      case minijson::Null:
        context.null(name);
        break;
    }
  }
}

namespace minijson
{
  void serialize(std::ostream &ost, bool bPrettyPrint, const tsjson::tsbase& root)
  {
    // enable pretty-printing with default settings (use 4 spaces)
    if (root.id() == typeid(tsjson::object_))
    {
      minijson::object_writer writer(ost,
                                     minijson::writer_configuration().pretty_printing(bPrettyPrint));
      
      serialize_impl(writer, static_cast<const tsjson::objBindings&>(root)._);
    }
    else if (root.id() == typeid(tsjson::arrayIterator))
    {
      minijson::array_writer writer(ost,
                                     minijson::writer_configuration().pretty_printing(bPrettyPrint));
      
      serialize_impl(writer, static_cast<const tsjson::arrayIterator&>(root));
    }
    else
      assert(false);
  }
  
  
  void deserialize(std::istream &ist, tsjson::tsbase& root)
  {
    minijson::istream_context ctx(ist);
  
    if (root.id() == typeid(tsjson::object_))
    {
      minijson::parse_object(ctx, [&](const char* name, const minijson::value& value)
                             {
                               Parse(ctx, static_cast<tsjson::objBindings&>(root)._, name, value);
                             });
    }
    else if (root.id() == typeid(tsjson::arrayIterator))
    {
      unsigned indx = 0;
      minijson::parse_array(ctx, [&](const minijson::value& value)
                            {
                              Parse(ctx, static_cast<tsjson::arrayIterator&>(root), indx++, value);
                            });
    }
    else
      assert(false);
  }
}
