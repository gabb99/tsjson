//
//  tsjson_rapidjson.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-28.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "tsjson_rapidjson.hpp"
#include "tsobject.hpp"
#include "tssax.hpp"

#define RAPIDJSON_HAS_STDSTRING 1
#include "document.h"     // rapidjson's SAX-style API
#include "istreamwrapper.h"
#include "ostreamwrapper.h"
#include "prettywriter.h"
using namespace rapidjson;

namespace
{
  struct MyHandler
  {
    tsjson::sax sax_helper;

    MyHandler(tsjson::tsbase& context)
    : sax_helper(context) {}

    bool Null()
    {
      sax_helper.null();
      return true;
    }
    
    bool Bool(bool b)
    {
      sax_helper.boolean(b);
      return true;
    }
    
    bool Int(int i)
    {
      sax_helper.integer(static_cast<int64_t>(i));
      return true;
    }
    
    bool Uint(unsigned u)
    {
      sax_helper.integer(static_cast<uint64_t>(u));
      return true;
    }
    
    bool Int64(int64_t i)
    {
      sax_helper.integer(i);
      return true;
    }
    
    bool Uint64(uint64_t u)
    {
      sax_helper.integer(u);
      return true;
    }
    
    bool Double(double d)
    {
      sax_helper.number(d);
      return true;
    }
    
    bool RawNumber(const char*, SizeType, bool)
    {
      // Only if kParseNumbersAsStrings
      assert(false);
      return true;
    }

    bool String(const char* str, SizeType length, bool)
    {
      sax_helper.string(std::string(str, length));
      return true;
    }

    bool Key(const char* str, SizeType length, bool)
    {
      sax_helper.key(std::string(str, length));
      return true;
    }

    bool StartObject()
    {
      sax_helper.begin_object();
      return true;
    }
    
    bool EndObject(SizeType)
    {
      sax_helper.end_object();
      return true;
    }

    bool StartArray()
    {
      sax_helper.begin_array();
      return true;
    }

    bool EndArray(SizeType)
    {
      sax_helper.end_array();
      return true;
    }
  };

  template <class T>
  void serialize_impl(T &writer, const tsjson::bindings& context);

  template <class T>
  void serialize_impl(T &writer, const tsjson::arrayIterator& arrayIt)
  {
    arrayIt.iterate([&](const tsjson::tsbase& element)
                    {
                      if (element.null())
                      {
                        writer.Null();
                      }
                      
                      else if (element.id() == typeid(tsjson::boolean))
                      {
                        writer.Bool(static_cast<const tsjson::boolean&>(element).operator bool());
                      }
                      
                      else if (element.id() == typeid(tsjson::string))
                      {
                        writer.String(static_cast<const tsjson::string&>(element).operator const std::string&());
                      }
                      
                      else if (element.id() == typeid(tsjson::integer))
                      {
                        writer.Int(static_cast<const tsjson::integer&>(element).operator int());
                      }
                      
                      else if (element.id() == typeid(tsjson::number))
                      {
                        writer.Double(static_cast<const tsjson::number&>(element).operator double());
                      }
                      
                      else if (element.id() == typeid(tsjson::arrayIterator))
                      {
                        writer.StartArray();

                        serialize_impl(writer,
                                       static_cast<const tsjson::arrayIterator&>(element));

                        writer.EndArray();
                      }
                      
                      else if (element.id() == typeid(tsjson::object_))
                      {
                        writer.StartObject();
                        
                        serialize_impl(writer,
                                       static_cast<const tsjson::objBindings&>(element)._);
                        
                        writer.EndObject();
                      }
                      
                      else
                        assert(false);
                    });
  }

  template <class T>
  void serialize_impl(T &writer, const tsjson::bindings& context)
  {
    context.iterate([&](const tsjson::JsonBindings_t::TContainer::value_type& it)
                    {
                      writer.Key(it.first);

                      if (it.second->null())
                      {
                        writer.Null();
                      }
                      
                      else if (it.second->id() == typeid(tsjson::boolean))
                      {
                        writer.Bool(static_cast<tsjson::boolean&>(*it.second).operator bool());
                      }
                      
                      else if (it.second->id() == typeid(tsjson::string))
                      {
                        writer.String(static_cast<tsjson::string&>(*it.second).operator const std::string&());
                      }
                      
                      else if (it.second->id() == typeid(tsjson::integer))
                      {
                        writer.Int(static_cast<tsjson::integer&>(*it.second).operator int());
                      }
                      
                      else if (it.second->id() == typeid(tsjson::number))
                      {
                        writer.Double(static_cast<tsjson::number&>(*it.second).operator double());
                      }

                      else if (it.second->id() == typeid(tsjson::arrayIterator))
                      {
                        writer.StartArray();

                        serialize_impl(writer,
                                       static_cast<const tsjson::arrayIterator&>(*it.second));
                        
                        writer.EndArray();
                      }

                      else if (it.second->id() == typeid(tsjson::object_))
                      {
                        writer.StartObject();
                        
                        serialize_impl(writer,
                                       static_cast<const tsjson::objBindings&>(*it.second)._);
                        
                        writer.EndObject();
                      }
                      
                      else
                        assert(false);
                    });
  }
 
  template <class T>
  void serialize_main(T &writer, const tsjson::tsbase& root)
  {
    writer.StartObject();
 
    if (root.id() == typeid(tsjson::object_))
      serialize_impl(writer, static_cast<const tsjson::objBindings&>(root)._);
    else if (root.id() == typeid(tsjson::arrayIterator))
      serialize_impl(writer, static_cast<const tsjson::arrayIterator&>(root));
    else
      assert(false);

    writer.EndObject();
  }
}


namespace rapidjson
{
  void serialize(std::ostream &ost, bool bPrettyPrint, const tsjson::tsbase& root)
  {
    OStreamWrapper osw(ost);
    
    if (bPrettyPrint)
    {
      PrettyWriter<OStreamWrapper> writer(osw);
      serialize_main(writer, root);
    }
    else
    {
      Writer<OStreamWrapper> writer(osw);
      serialize_main(writer, root);
    }
  }

  void deserialize(std::istream &ist, tsjson::tsbase& root)
  {
    IStreamWrapper isw(ist);
    Reader reader;
    MyHandler handler(root);

    reader.Parse(isw, handler);
  }
}
