//
//  tsjson_taocpp.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-28.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "tsjson_taocpp.hpp"

#include "tao/json.hpp"
#include "tssax.hpp"

namespace
{
  struct reduced_consumer
  {
    tsjson::sax sax_helper;

    reduced_consumer(tsjson::tsbase& context)
    : sax_helper(context) {}

    void null()
    {
      sax_helper.null();
    }

    void boolean( const bool b )
    {
      sax_helper.boolean(b);
    }

    void number( const double d )
    {
      sax_helper.number(d);
    }

    void number( const int64_t i )
    {
      sax_helper.integer(i);
    }

    void number( const uint64_t u )
    {
      sax_helper.integer(u);
    }

    void string( const std::string_view& s )
    {
      sax_helper.string(static_cast<const std::string&>(s));
    }

    void begin_array( const std::size_t s = 0 )
    {
      sax_helper.begin_array(s);
    }

    void end_array( const std::size_t s = 0 )
    {
      sax_helper.end_array();
    }

    void begin_object( const std::size_t s = 0 )
    {
      sax_helper.begin_object();
    }

    void end_object( const std::size_t s = 0 )
    {
      sax_helper.end_object();
    }

    void key( const std::string_view& s )
    {
      sax_helper.key(static_cast<const std::string&>(s));
    }

    void binary( const tao::binary_view& b )
    {
      // todo Implement
      assert(false);
    }
  
    void member() {}
    void element() {}
  };

  template < class T>
  void serialize_impl(T &writer, const tsjson::bindings& context);

  template < class T>
  void serialize_impl(T &writer, const tsjson::arrayIterator& arrayIt)
  {
    arrayIt.iterate([&](const tsjson::tsbase& element)
                    {
                      if (element.null())
                      {
                        writer.null();
                      }
                      
                      else if (element.id() == typeid(tsjson::boolean))
                      {
                        writer.boolean(static_cast<const tsjson::boolean&>(element).operator bool());
                      }
                      
                      else if (element.id() == typeid(tsjson::string))
                      {
                        writer.string(static_cast<const tsjson::string&>(element).operator const std::string&());
                      }
                      
                      else if (element.id() == typeid(tsjson::integer))
                      {
                        writer.number(static_cast<int64_t>(static_cast<const tsjson::integer&>(element).operator int()));
                      }
                      
                      else if (element.id() == typeid(tsjson::number))
                      {
                        writer.number(static_cast<const tsjson::number&>(element).operator double());
                      }
                      
                      else if (element.id() == typeid(tsjson::arrayIterator))
                      {
                        writer.begin_array();
                        
                        serialize_impl(writer,
                                       static_cast<const tsjson::arrayIterator&>(element));
                        
                        writer.end_array();
                      }
                      
                      else
                      {
                        assert(element.id() == typeid(tsjson::object_));

                        writer.begin_object();
                        
                        serialize_impl(writer,
                                       static_cast<const tsjson::objBindings&>(element)._);
                        
                        writer.end_object();
                      }
                      
                      writer.element();
                    });
  }
  
  template < class T>
  void serialize_impl(T &writer, const tsjson::bindings& context)
  {
    context.iterate([&](const tsjson::JsonBindings_t::TContainer::value_type& it)
                    {
                      writer.key(it.first);
                      
                      if (it.second->null())
                      {
                        writer.null();
                      }
                      
                      else if (it.second->id() == typeid(tsjson::boolean))
                      {
                        writer.boolean(static_cast<tsjson::boolean&>(*it.second).operator bool());
                      }
                      
                      else if (it.second->id() == typeid(tsjson::string))
                      {
                        writer.string(static_cast<tsjson::string&>(*it.second).operator const std::string&());
                      }
                      
                      else if (it.second->id() == typeid(tsjson::integer))
                      {
                        writer.number(static_cast<int64_t>(static_cast<tsjson::integer&>(*it.second).operator int()));
                      }
                      
                      else if (it.second->id() == typeid(tsjson::number))
                      {
                        writer.number(static_cast<tsjson::number&>(*it.second).operator double());
                      }

                      else if (it.second->id() == typeid(tsjson::arrayIterator))
                      {
                        writer.begin_array();

                        serialize_impl(writer,
                                       static_cast<const tsjson::arrayIterator&>(*it.second));
                        
                        writer.end_array();
                      }

                      else
                      {
                        assert(it.second->id() == typeid(tsjson::object_));

                        writer.begin_object();
                        
                        serialize_impl(writer,
                                       static_cast<const tsjson::objBindings&>(*it.second)._);
                        
                        writer.end_object();
                      }

                      writer.element();
                    });
  }

  template <class T>
  void serialize_main(T &writer, const tsjson::tsbase& root)
  {
    if (root.id() == typeid(tsjson::object_))
    {
      writer.begin_object();
      serialize_impl(writer, static_cast<const tsjson::objBindings&>(root)._);
      writer.end_object();
    }
    else
    {
      assert(root.id() == typeid(tsjson::arrayIterator));

      writer.begin_array();
      serialize_impl(writer, static_cast<const tsjson::arrayIterator&>(root));
      writer.end_array();
    }
  }
}

namespace tao
{
  namespace json
  {
    void serialize(std::ostream &ost, bool bPrettyPrint, bool bJaxn, const tsjson::tsbase& root)
    {
      if (bJaxn)
      {
        jaxn::events::to_stream writer( ost );
        serialize_main(writer, root);
      }
      else if (bPrettyPrint)
      {
        events::to_pretty_stream writer( ost, 4 );
        serialize_main(writer, root);
      }
      else
      {
        events::to_stream writer( ost );
        serialize_main(writer, root);
      }
    }
                   
    void serialize(std::ostream &ost, const tsjson::tsbase& root, enum format t)
    {
      switch (t)
      {
        case tjson:
          serialize(ost, false, false, root);
          break;
          
        case jaxn_:
          serialize(ost, false, true, root);
          break;
          
        case cbor_:
          {
            cbor::events::to_stream writer( ost );
            serialize_main(writer, root);
          }
          break;
          
        case messagepack:
          {
            msgpack::events::to_stream writer( ost );
            serialize_main(writer, root);
          }
          break;
          
        case ubjson_:
          {
            msgpack::events::to_stream writer( ost );
            serialize_main(writer, root);
          }
          break;
      }
    }
    
    void deserialize(std::istream &ist, tsjson::tsbase& root, enum format t)
    {
      json::events::transformer<reduced_consumer> consumer(root);
      
      switch (t)
      {
        case tjson:
          events::from_stream( consumer, ist );
          break;

        case jaxn_:
          jaxn::events::from_stream( consumer, ist );
          break;
          
        case cbor_:
          {
            std::string s;
            copy( std::istreambuf_iterator<char>(ist), std::istreambuf_iterator<char>(), back_inserter(s) );
            cbor::events::from_string( consumer, s );
          }
          break;

        case messagepack:
          {
            std::string s;
            copy( std::istreambuf_iterator<char>(ist), std::istreambuf_iterator<char>(), back_inserter(s) );
            msgpack::events::from_string( consumer, s );
          }
          break;

        case ubjson_:
          {
            std::string s;
            copy( std::istreambuf_iterator<char>(ist), std::istreambuf_iterator<char>(), back_inserter(s) );
            ubjson::events::from_string( consumer, s );
          }
          break;
      }
    }
  }
}
