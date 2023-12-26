#ifndef SPREAD_INTERFACE
#define SPREAD_INTERFACE
#include "ccglobal/export.h"

#if USE_SPREAD_DLL
	#define SPREAD_API CC_DECLARE_IMPORT
#elif USE_SPREAD_STATIC
	#define SPREAD_API CC_DECLARE_STATIC
#else
	#if SPREAD_DLL
		#define SPREAD_API CC_DECLARE_EXPORT
	#else
		#define SPREAD_API CC_DECLARE_STATIC
	#endif
#endif

#endif // SPREAD_INTERFACE