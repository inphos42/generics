#ifndef GENERICS_MACROS_H
#define GENERICS_MACROS_H

#ifdef __GNUC__
	#define GENERICS_MARK_FUNC_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
	#define GENERICS_MARK_FUNC_DEPRECATED __declspec(deprecated)
#else
	#pragma message("WARNING: You need to implement GENERICS_MARK_FUNC_DEPRECATED for this compiler")
	#define GENERICS_MARK_FUNC_DEPRECATED
#endif

#endif