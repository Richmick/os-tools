module os.winapi;

#ifdef _WIN32
import std;

winapi::numa::info winapi::numa::get_info()
{
	unsigned long size;
	winapi::calls::GetSystemCpuSetInformation(nullptr, 0, &size, this_process::native_handle(), 0);
	if (size == 0)
	{
		std::println(std::cerr, "no cpusets");
	}
	info result;
	return result;
}
#endif
