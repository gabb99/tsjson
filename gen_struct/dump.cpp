//
//  main.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>

#include <boost/program_options.hpp>

// Support most formats
#include "tao/json.hpp"
#include "tsstream.hpp"

namespace
{
  std::istream& operator>>(std::istream& in, tsjson::impl &backend)
  {
    std::string token;
    in >> token;

    if (token == "cjson")
      backend = tsjson::impl::cjson;
    else if (token == "sajson")
      backend = tsjson::impl::sajson;
    else if (token == "nlohmann")
      backend = tsjson::impl::nlohmannj_son;
    else if (token == "minijson")
      backend = tsjson::impl::minijson;
    else if (token == "taojson")
      backend = tsjson::impl::taojson;
    else
      in.setstate(std::ios_base::failbit);

    return in;
  }

  void traverse(const tao::json::value& node)
  {
    switch (node.type())
    {
      case tao::json::type::UNINITIALIZED:
      case tao::json::type::DISCARDED:
      case tao::json::type::DESTROYED:
        break;

      case tao::json::type::VALUE_PTR:
      case tao::json::type::OPAQUE_PTR:
        break;
        
      case tao::json::type::NULL_:
        break;
        
      case tao::json::type::BOOLEAN:
        break;
        
      case tao::json::type::ARRAY:
      {
        auto length = node.get_length();
        for (size_t i = 0; i < length; ++i)
        {
          std::cout << i << " " << node.get_type() << std::endl;
          traverse(node.get_array_element(i));
        }
        break;
      }
        
      case tao::json::type::OBJECT:
      {
        auto length = node.get_length();
        for (auto i = 0u; i < length; ++i)
        {
          std::cout << node.get_object_key(i).as_string() << " " << node.get_type() << std::endl;
          traverse(node.get_object_value(i));
        }
        break;
      }
        
      case tao::json::type::STRING:
      case tao::json::type::STRING_VIEW:
        break;
        
      case tao::json::type::BINARY:
      case tao::json::type::BINARY_VIEW:
        break;
        
      case tao::json::type::SIGNED:
      case tao::json::type::UNSIGNED:
      case tao::json::type::DOUBLE:
        break;
    }
  }
}

int main(int argc, const char * argv[])
{
  tsjson::impl backend;

  // Declare the supported options.
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
  ("help,h", "produce help message")
  ("version,v", "Hardware revisions")
  ("scoped,s", "Generate scoped structures")
  ("using,u", boost::program_options::value<tsjson::impl>(&backend)->multitoken(), "The json back-end to target, else will generate templated accesors")
  ("input-file", boost::program_options::value<std::vector<std::string>>(), "input file(s)")
  ;
  
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);
  
  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    return 1;
  }
  
  try
  {
    for (auto&& file : vm["input-file"].as<std::vector<std::string>>())
    {
      // Iterate over document
      traverse(tao::json::parse_file(file));
    }
  }
  
  catch (const std::exception &ex)
  {
    std::cerr << "ERROR: Caught unhandled exception " << ex.what() << std::endl;
  }
  
  catch (...)
  {
    std::cerr << "ERROR: Caught unhandled unknown exception " << std::endl;
  }
  
  return -1;
}
