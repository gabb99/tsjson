//
//  main.cpp
//  tsjson
//
//  Created by Gabriel Beauchemin on 2018-11-27.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include <iostream>

#include "tsobject.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

int main(int argc, const char * argv[])
{
  testing::InitGoogleMock(&argc, const_cast<char**>(argv));
  return RUN_ALL_TESTS();
}
