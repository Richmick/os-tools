module os.winapi;

#ifdef _WIN32
import std;

winapi::process::process(unique_handle&& h) noexcept:
	native_(std::move(h))
{}
winapi::process::process(native_handle_type h) noexcept:
	native_(h)
{}

winapi::process::process(const path& app)
{
	std::error_code err;
	*this = process{app.c_str(), string{}, err};
	throw_on_error(err, "failed to create process");
}
winapi::process::process(const path& app, std::span< const string > args)
{
	std::error_code err;
	*this = process{app.c_str(), prepare_cmd_args(app.c_str(), args), err};
	throw_on_error(err, "failed to create process");
}
winapi::process::process(const path& app, std::span< const char_t* const > args)
{
	std::error_code err;
	*this = process{app.c_str(), prepare_cmd_args(app.c_str(), args), err};
	throw_on_error(err, "failed to create process");
}
winapi::process::process(const char_t* app)
{
	std::error_code err;
	*this = process{app, string{}, err};
	throw_on_error(err, "failed to create process");
}
winapi::process::process(const char_t* app, std::span< const string > args)
{
	std::error_code err;
	*this = process{app, prepare_cmd_args(app, args), err};
	throw_on_error(err, "failed to create process");
}
winapi::process::process(const char_t* app, std::span< const char_t* const > args)
{
	std::error_code err;
	*this = process{app, prepare_cmd_args(app, args), err};
	throw_on_error(err, "failed to create process");
}

winapi::process::process(const path& app, std::error_code& err):
	process(app.c_str(), string{}, err)
{}
winapi::process::process(const path& app, std::span< const string > args, std::error_code& err) :
	process(app.c_str(), prepare_cmd_args(app.c_str(), args), err)
{}
winapi::process::process(const path& app, std::span< const char_t* const > args, std::error_code& err) :
	process(app.c_str(), prepare_cmd_args(app.c_str(), args), err)
{}
winapi::process::process(const char_t* app, std::error_code& err):
	process(app, string{}, err)
{}
winapi::process::process(const char_t* app, std::span< const string > args, std::error_code& err):
	process(app, prepare_cmd_args(app, args), err)
{}
winapi::process::process(const char_t* app, std::span< const char_t* const > args, std::error_code& err):
	process(app, prepare_cmd_args(app, args), err)
{}

winapi::process::process(initial_params& params, const path& app)
{
	std::error_code err;
	*this = process{params, app.c_str(), string{}, err};
	throw_on_error(err, "failed to create process");
}
winapi::process::process(initial_params& params, const path& app, std::span< const string > args)
{
	std::error_code err;
	*this = process{params, app.c_str(), prepare_cmd_args(app.c_str(), args), err};
	throw_on_error(err, "failed to create process");
}
winapi::process::process(initial_params& params, const path& app, std::span< const char_t* const > args)
{
	std::error_code err;
	*this = process{params, app.c_str(), prepare_cmd_args(app.c_str(), args), err};
	throw_on_error(err, "failed to create process");
}
winapi::process::process(initial_params& params, const char_t* app)
{
	std::error_code err;
	*this = process{params, app, string{}, err};
	throw_on_error(err, "failed to create process");
}
winapi::process::process(initial_params& params, const char_t* app, std::span< const string > args)
{
	std::error_code err;
	*this = process{params, app, prepare_cmd_args(app, args), err};
	throw_on_error(err, "failed to create process");
}
winapi::process::process(initial_params& params, const char_t* app, std::span< const char_t* const > args)
{
	std::error_code err;
	*this = process{params, app, prepare_cmd_args(app, args), err};
	throw_on_error(err, "failed to create process");
}

winapi::process::process(initial_params& params, const path& app, std::error_code& err):
	process(params, app.c_str(), string{}, err)
{}
winapi::process::process(initial_params& params, const path& app, std::span< const string > args, std::error_code& err) :
	process(params, app.c_str(), prepare_cmd_args(app.c_str(), args), err)
{}
winapi::process::process(initial_params& params, const path& app, std::span< const char_t* const > args, std::error_code& err) :
	process(params, app.c_str(), prepare_cmd_args(app.c_str(), args), err)
{}
winapi::process::process(initial_params& params, const char_t* app, std::error_code& err):
	process(params, app, string{}, err)
{}
winapi::process::process(initial_params& params, const char_t* app, std::span< const string > args, std::error_code& err):
	process(params, app, prepare_cmd_args(app, args), err)
{}
winapi::process::process(initial_params& params, const char_t* app, std::span< const char_t* const > args, std::error_code& err):
	process(params, app, prepare_cmd_args(app, args), err)
{}


// Winapi specific:

winapi::process::process(const char* app, std::span< const std::string > args, std::error_code& err):
	process(app, prepare_cmd_args(app, args), err)
{}
winapi::process::process(const char* app, std::span< const char* const > args, std::error_code& err):
	process(app, prepare_cmd_args(app, args), err)
{}
winapi::process::process(initial_params& params, const char* app, std::span< const std::string > args, std::error_code& err):
	process(params, app, prepare_cmd_args(app, args), err)
{}
winapi::process::process(initial_params& params, const char* app, std::span< const char* const > args, std::error_code& err):
	process(params, app, prepare_cmd_args(app, args), err)
{}

winapi::process::process(const std::string& app, std::string args, std::error_code& err):
	native_(create(app.data(), args.data(), err))
{}
winapi::process::process(std::string cl_cmd, std::error_code& err):
	native_(create(nullptr, cl_cmd.data(), err))
{}
winapi::process::process(const std::wstring& app, std::wstring args, std::error_code& err):
	native_(create(app.data(), args.data(), err))
{}
winapi::process::process(std::wstring cl_cmd, std::error_code& err):
	native_(create(nullptr, cl_cmd.data(), err))
{}

winapi::process::process(initial_params& params, const std::string& app, std::string args, std::error_code& err):
	native_(create(params, app.data(), args.data(), err))
{}
winapi::process::process(initial_params& params, std::string cl_cmd, std::error_code& err):
	native_(create(params, nullptr, cl_cmd.data(), err))
{}
winapi::process::process(initial_params& params, const std::wstring& app, std::wstring args, std::error_code& err):
	native_(create(params, app.data(), args.data(), err))
{}
winapi::process::process(initial_params& params, std::wstring cl_cmd, std::error_code& err):
	native_(create(params, nullptr, cl_cmd.data(), err))
{}
#endif
