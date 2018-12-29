//
//  tsjson_nlohmannjson.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-28.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "tsjson_nlohmannjson.hpp"
#include "tsobject.hpp"
#include "tssax.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::json;


template<typename BasicJsonType>
class SaxHandler : nlohmann::json_sax<BasicJsonType>
{
  tsjson::sax sax_helper;

public:
  SaxHandler(tsjson::tsbase& context)
  : sax_helper(context) {}

  // called when null is parsed
  bool null()
  {
    sax_helper.null();
    return true;
  }
  
  // called when a boolean is parsed; value is passed
  bool boolean(bool b)
  {
    sax_helper.boolean(b);
    return true;
  }

  // called when a signed or unsigned integer number is parsed; value is passed
  bool number_integer(typename BasicJsonType::number_integer_t i)
  {
    sax_helper.integer(i);
    return true;
  }

  bool number_unsigned(typename BasicJsonType::number_unsigned_t u)
  {
    sax_helper.integer(u);
    return true;
  }

  // called when a floating-point number is parsed; value and original string is passed
  bool number_float(typename BasicJsonType::number_float_t d, const typename BasicJsonType::string_t&)
  {
    sax_helper.number(d);
    return true;
  }

  // called when a string is parsed; value is passed and can be safely moved away
  bool string(typename BasicJsonType::string_t& s)
  {
    sax_helper.string(s);
    return true;
  }

  
  // called when an object or array begins or ends, resp. The number of elements is passed (or -1 if not known)
  bool start_object(size_t s)
  {
    sax_helper.begin_object(s);
    return true;
  }

  bool end_object()
  {
    sax_helper.end_object();
    return true;
  }


  bool start_array(size_t elements)
  {
    sax_helper.begin_array(elements);
    return true;
  }

  bool end_array()
  {
    sax_helper.end_array();
    return true;
  }

  // called when an object key is parsed; value is passed and can be safely moved away
  bool key(typename BasicJsonType::string_t& val)
  {
    sax_helper.key(val);
    return true;
  }
  
  // called when a parse error occurs; byte position, the last token, and an exception is passed
  bool parse_error(size_t position, const std::string& last_token, const nlohmann::detail::exception& ex)
  {
    return true;
  }
};


namespace
{
  void serialize_impl(json& j_object, const tsjson::bindings& context);

  void serialize_impl(json& j_object, const tsjson::arrayIterator& arrayIt)
  {
    unsigned indx = 0;
    // Force to empty array, instead of null when no members are added.
    j_object = json(nlohmann::detail::value_t::array);

    arrayIt.iterate([&](const tsjson::tsbase& element)
                    {
                      if (element.null())
                      {
                        j_object[indx++] = nullptr;
                      }
                      
                      else if (element.id() == typeid(tsjson::boolean))
                      {
                        j_object[indx++] = static_cast<const tsjson::boolean&>(element).operator bool();
                      }
                      
                      else if (element.id() == typeid(tsjson::string))
                      {
                        j_object[indx++] = static_cast<const tsjson::string&>(element).operator const std::string&();
                      }
                      
                      else if (element.id() == typeid(tsjson::integer))
                      {
                        j_object[indx++] = static_cast<const tsjson::integer&>(element).operator int();
                      }
                      
                      else if (element.id() == typeid(tsjson::number))
                      {
                        j_object[indx++] = static_cast<const tsjson::number&>(element).operator double();
                      }
                      
                      else if (element.id() == typeid(tsjson::arrayIterator))
                      {
                        serialize_impl(j_object[indx++],
                                       static_cast<const tsjson::arrayIterator&>(element));
                      }
                      
                      else if (element.id() == typeid(tsjson::object_))
                      {
                        serialize_impl(j_object[indx++],
                                       static_cast<const tsjson::objBindings&>(element)._);
                      }
                      
                      else
                        assert(false);
                    });
  }

  void serialize_impl(json& j_object, const tsjson::bindings& context)
  {
    // Force to empty object, instead of null when no members are added.
    j_object = json(nlohmann::detail::value_t::object);

    context.iterate([&](const tsjson::JsonBindings_t::TContainer::value_type& it)
                    {
                      if (it.second->null())
                      {
                        j_object[it.first] = nullptr;
                      }
                      
                      else if (it.second->id() == typeid(tsjson::boolean))
                      {
                        j_object[it.first] = static_cast<tsjson::boolean&>(*it.second).operator bool();
                      }
                      
                      else if (it.second->id() == typeid(tsjson::string))
                      {
                        j_object[it.first] = static_cast<tsjson::string&>(*it.second).operator const std::string&();
                      }
                      
                      else if (it.second->id() == typeid(tsjson::integer))
                      {
                        j_object[it.first] = static_cast<tsjson::integer&>(*it.second).operator int();
                      }
                      
                      else if (it.second->id() == typeid(tsjson::number))
                      {
                        j_object[it.first] = static_cast<tsjson::number&>(*it.second).operator double();
                      }

                      else if (it.second->id() == typeid(tsjson::arrayIterator))
                      {
                        serialize_impl(j_object[it.first],
                                       static_cast<tsjson::arrayIterator&>(*it.second));
                      }

                      else if (it.second->id() == typeid(tsjson::object_))
                      {
                        serialize_impl(j_object[it.first],
                                       static_cast<tsjson::objBindings&>(*it.second)._);
                      }
                      
                      else
                        assert(false);
                    });
  }
}

namespace nlohmann
{
  void serialize(std::ostream &ost, bool bPrettyPrint, const tsjson::tsbase& root)
  {
    // Not optimal, have to convert to nlohmann's json
    json j_object;

    if (root.id() == typeid(tsjson::object_))
      serialize_impl(j_object, static_cast<const tsjson::objBindings&>(root)._);
    else if (root.id() == typeid(tsjson::arrayIterator))
      serialize_impl(j_object, static_cast<const tsjson::arrayIterator&>(root));
    else
      assert(false);

    ost << j_object;
  }

  void serialize(std::ostream &ost, const tsjson::tsbase& root, enum format t)
  {
    // Not optimal, have to convert to nlohmann's json
    json j_object;

    if (root.id() == typeid(tsjson::object_))
      serialize_impl(j_object, static_cast<const tsjson::objBindings&>(root)._);
    else if (root.id() == typeid(tsjson::arrayIterator))
      serialize_impl(j_object, static_cast<const tsjson::arrayIterator&>(root));
    else
      assert(false);

    switch (t)
    {
      case tjson:
        serialize(ost, false, root);
        break;
  
      case bson:
        json::to_bson(j_object, ost);
        break;
        
      case cbor:
        json::to_cbor(j_object, ost);
        break;
        
      case messagepack:
        json::to_msgpack(j_object, ost);
        break;
        
      case ubjson:
        json::to_ubjson(j_object, ost);
        break;
    }
  }

  void deserialize(std::istream &ist, tsjson::tsbase& root, enum format t)
  {
    SaxHandler<json> handler(root);

    switch (t)
    {
      case tjson:
        json::sax_parse(ist, &handler);
        break;

      case bson:
        json::sax_parse(ist, &handler, detail::input_format_t::bson);
        break;
        
      case cbor:
        json::sax_parse(ist, &handler, detail::input_format_t::cbor);
        break;

      case messagepack:
        json::sax_parse(ist, &handler, detail::input_format_t::msgpack);
        break;

      case ubjson:
        json::sax_parse(ist, &handler, detail::input_format_t::ubjson);
        break;
    }
  }
}
