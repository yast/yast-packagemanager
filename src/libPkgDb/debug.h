#ifndef DEBUG_H
#define DEBUG_H

#ifdef NDEBUG
#  define debug(expr)
#else
#  define debug(expr)	do { std::cout << __PRETTY_FUNCTION__ << ": " << expr << std::endl; } while(0)
#endif

#endif
