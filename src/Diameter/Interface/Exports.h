#ifndef __DIAMCORE_EXPORTS_H__
#define __DIAMCORE_EXPORTS_H__

#ifdef WIN32
	#ifdef DIAMCORE_EXPORTS
		#define DIAMCORE_API __declspec(dllexport)
	#elif defined DIAMCORE_IMPORTS
		#define DIAMCORE_API __declspec(dllimport)
	#else
		#define DIAMCORE_API
	#endif
#else
	#define DIAMCORE_API
#endif

#endif //__DIAMCORE_EXPORTS_H__
