#ifndef DEBUG_HPP
#define DEBUG_HPP

#ifndef LV2CXX_DEBUG
#  ifdef NDEBUG
#    define LV2CXX_DEBUG false
#  else
#    define LV2CXX_DEBUG true
#  endif
#endif

#if LV2CXX_DEBUG
#include <iomanip>
#include <iostream>
#endif

#endif
