// a implementation of scoped handle which ensures the safe use of stardard handles

#ifndef BASE_MEMORY_SCOPED_STD_HANDLE_H_
#define BASE_MEMORY_SCOPED_STD_HANDLE_H_

#include <stdio.h>
#include "base/base_config.h"
#include "base/base_export.h"
#include "base/base_types.h"

namespace nbase
{

// This class wraps the c library function free() in a class that can be
// passed as a template argument to scoped_ptr_malloc below.
class BASE_EXPORT DeleterFree {
public:
	inline void operator()(void* x) const {
		free(x);
	}
};

class BASE_EXPORT DeleterFileClose {
public:
	inline void operator()(FILE * x) const {
		fclose(x);
	}
};

class BASE_EXPORT DeleterLibrary
{
public:
	inline void operator()(HMODULE * h) const {
		::FreeLibrary(*h);
		delete h;
	}
};

} // namespace

#endif // BASE_MEMORY_SCOPED_STD_HANDLE_H_
