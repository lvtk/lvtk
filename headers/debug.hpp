#ifndef DEBUG_HPP
#define DEBUG_HPP

#ifndef LV2CPP_DEBUG
#ifdef NDEBUG
#define LV2CPP_DEBUG false
#else
#define LV2CPP_DEBUG true
#endif
#endif

#if LV2CPP_DEBUG
#include <iomanip>
#include <iostream>
#endif

#endif
