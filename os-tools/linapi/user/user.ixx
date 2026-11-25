module;
	#ifdef __linux__
		#include <unistd.h>
	#endif
export module os.linapi:user;

#ifdef __linux__
namespace linapi
{
	class user
	{
	public:
		/// [Note] Winapi does not have integer representation
		using id = uid_t;
	};
}
#endif
