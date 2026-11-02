module os.linapi;

#ifdef __linux__
import std;

linapi::ready_args_t linapi::prepare_cmd_args(const char* app, std::span< const char*const > args)
{
	ready_args_t result;
	result.first.reserve(args.size() + 1);
	result.second.reserve(args.size() + 2);
	result.second.emplace_back(result.first.emplace_back(app).data());
	for (const char* arg: args)
	{
		result.second.emplace_back(result.first.emplace_back(arg).data());
	}
	result.second.emplace_back(nullptr);
	return result;
}
linapi::ready_args_t linapi::prepare_cmd_args(const path& app, std::span< const char*const > args)
{
	ready_args_t result;
	result.first.reserve(args.size() + 1);
	result.second.reserve(args.size() + 2);
	result.second.emplace_back(result.first.emplace_back(app.c_str()).data());
	for (const char* arg: args)
	{
		result.second.emplace_back(result.first.emplace_back(arg).data());
	}
	result.second.emplace_back(nullptr);
	return result;
}
linapi::ready_args_t linapi::prepare_cmd_args(const path& app, std::span< const std::string > args)
{
	ready_args_t result;
	result.first.reserve(args.size() + 1);
	result.second.reserve(args.size() + 2);
	result.second.emplace_back(result.first.emplace_back(app.c_str()).data());
	for (const std::string& arg: args)
	{
		result.second.emplace_back(result.first.emplace_back(arg.c_str()).data());
	}
	result.second.emplace_back(nullptr);
	return result;
}
linapi::process::native_handle_type linapi::process::create(const char* app, char*const* argv, std::error_code& err)
{
	process_handle_t h = linapi::calls::vfork();
	if (h == 0)
	{
		linapi::calls::execv(app, argv);
		linapi::calls::_exit(-1);
	}
	else if (h == -1)
		err = linapi::last_error_code();
	else
		err.clear();
	return h;
}
linapi::process::native_handle_type linapi::process::create(initial_params& params,
			const char* app, char*const* argv, std::error_code& err)
{
	return create(app, argv, err);
}

#endif
