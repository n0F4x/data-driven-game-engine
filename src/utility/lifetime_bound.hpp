#pragma once


#ifdef CHECK_LIFETIME_BOUND
  #ifndef __has_cpp_attribute
    #define lifetime_bound
  #elif __has_cpp_attribute(msvc::lifetimebound)
    #define lifetime_bound msvc::lifetimebound
  #elif __has_cpp_attribute(clang::lifetimebound)
    #define lifetime_bound clang::lifetimebound
  #elif __has_cpp_attribute(lifetimebound)
    #define lifetime_bound lifetimebound
  #else
    #define lifetime_bound
  #endif
#else
  #define lifetime_bound
#endif
