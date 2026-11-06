module;
	#ifdef __linux__
		#include <span>
		#include <system_error>
		#include <vector>
	#endif
module os.linapi;

#ifdef __linux__
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
	process_handle_t h = linapi::calls::fork();
	if (h == 0)
	{
		//linapi::calls::closefrom(this_process::stderr() + 1);
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
	bool manual_std_io = (params.stdin != empty_handle)
			|| (params.stdout != empty_handle) || (params.stderr != empty_handle);
	process_handle_t h = linapi::calls::fork();
	if (h == 0)
	{
		if (manual_std_io)
		{
			if (params.stdin != empty_handle) linapi::calls::dup2(params.stdin, this_process::stdin());
			else linapi::calls::close(this_process::stdin());
			if (params.stdout != empty_handle) linapi::calls::dup2(params.stdout, this_process::stdout());
			else linapi::calls::close(this_process::stdout());
			if (params.stderr != empty_handle) linapi::calls::dup2(params.stderr, this_process::stderr());
			else linapi::calls::close(this_process::stderr());
		}
		if (params.inherit_handles == inheritmode::none)
		{
			//linapi::calls::closefrom(this_process::stderr() + 1);
		}
		linapi::calls::execv(app, argv);
		linapi::calls::_exit(-1);
	}
	else if (h == -1)
		err = linapi::last_error_code();
	else
		err.clear();
	return h;
}

#endif
