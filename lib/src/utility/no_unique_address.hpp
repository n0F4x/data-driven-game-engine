#pragma once

#ifndef __has_cpp_attribute
  #define engine_no_unique_address
#elif __has_cpp_attribute(no_unique_address)
  #define engine_no_unique_address no_unique_address
#elif __has_cpp_attribute(msvc::no_unique_address)
  #define engine_no_unique_address msvc::no_unique_address
#else
  #define engine_no_unique_address
#endif
