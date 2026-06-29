#pragma once


#ifdef CHECK_LIFETIME_BOUND
  #ifndef __has_cpp_attribute
    #define engine_lifetimebound
  #elif __has_cpp_attribute(msvc::lifetimebound)
    #define engine_lifetimebound msvc::lifetimebound
  #elif __has_cpp_attribute(clang::lifetimebound)
    #define engine_lifetimebound clang::lifetimebound
  #elif __has_cpp_attribute(lifetimebound)
    #define engine_lifetimebound lifetimebound
  #else
    #define engine_lifetimebound
  #endif
#else
  #define engine_lifetimebound
#endif
