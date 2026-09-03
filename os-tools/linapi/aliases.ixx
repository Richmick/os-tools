module;
	#ifdef __linux__
		#include <unistd.h>
	#endif
export module os.linapi:aliases;

#ifdef __linux__

namespace linapi
{
	using handle_t = int;
}

#endif
