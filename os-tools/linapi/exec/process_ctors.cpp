module os.linapi;

#ifdef __linux__
import std;

linapi::process::process(native_handle_type h) noexcept:
	native_(h)
{}

linapi::process::process(process&& rhs) noexcept:
	native_(rhs.release_handle())
{}

linapi::process::process(const path& app):
	process(prepare_cmd_args(app, std::span< const string >{}))
{}
linapi::process::process(const path& app, std::span< const string > args):
	process(prepare_cmd_args(app, args))
{}
linapi::process::process(const path& app, std::span< const char_t* const > args):
	process(prepare_cmd_args(app, args))
{}
linapi::process::process(const char_t* app):
	process(prepare_cmd_args(app, std::span< const string >{}))
{}
linapi::process::process(const char_t* app, std::span< const string > args):
	process(prepare_cmd_args(app, args))
{}
linapi::process::process(const char_t* app, std::span< const char_t* const > args):
	process(prepare_cmd_args(app, args))
{}

linapi::process::process(const path& app, std::error_code& err):
	process(prepare_cmd_args(app, std::span< const string >{}), err)
{}
linapi::process::process(const path& app, std::span< const string > args, std::error_code& err):
	process(prepare_cmd_args(app, args), err)
{}
linapi::process::process(const path& app, std::span< const char_t* const > args, std::error_code& err):
	process(prepare_cmd_args(app, args), err)
{}
linapi::process::process(const char_t* app, std::error_code& err):
	process(prepare_cmd_args(app, std::span< const string >{}), err)
{}
linapi::process::process(const char_t* app, std::span< const string > args, std::error_code& err):
	process(prepare_cmd_args(app, args), err)
{}
linapi::process::process(const char_t* app, std::span< const char_t* const > args, std::error_code& err):
	process(prepare_cmd_args(app, args), err)
{}

linapi::process::process(initial_params& params, const path& app):
	process(params, prepare_cmd_args(app, std::span< const string >{}))
{}
linapi::process::process(initial_params& params, const path& app, std::span< const string > args):
	process(params, prepare_cmd_args(app, args))
{}
linapi::process::process(initial_params& params, const path& app, std::span< const char_t* const > args):
	process(params, prepare_cmd_args(app, args))
{}
linapi::process::process(initial_params& params, const char_t* app):
	process(params, prepare_cmd_args(app, std::span< const string >{}))
{}
linapi::process::process(initial_params& params, const char_t* app, std::span< const string > args):
	process(params, prepare_cmd_args(app, args))
{}
linapi::process::process(initial_params& params, const char_t* app, std::span< const char_t* const > args):
	process(params, prepare_cmd_args(app, args))
{}

linapi::process::process(initial_params& params, const path& app, std::error_code& err):
	process(params, prepare_cmd_args(app, std::span< const string >{}), err)
{}
linapi::process::process(initial_params& params,
			const path& app, std::span< const string > args, std::error_code& err):
	process(params, prepare_cmd_args(app, args), err)
{}
linapi::process::process(initial_params& params,
			const path& app, std::span< const char_t* const > args, std::error_code& err):
	process(params, prepare_cmd_args(app, args), err)
{}
linapi::process::process(initial_params& params,
			const char_t* app, std::error_code& err):
	process(params, prepare_cmd_args(app, std::span< const string >{}), err)
{}
linapi::process::process(initial_params& params,
			const char_t* app, std::span< const string > args, std::error_code& err):
	process(params, prepare_cmd_args(app, args), err)
{}
linapi::process::process(initial_params& params,
			const char_t* app, std::span< const char_t* const > args, std::error_code& err):
	process(params, prepare_cmd_args(app, args), err)
{}

linapi::process::process(const char_t* app, char_t* const* args)
{
	std::error_code err;
	native_ = create(app, args, err);
	throw_on_error(err, "failed to create process");
}
linapi::process::process(ready_args_t argv)
{
	string app = argv.first.front();
	std::error_code err;
	native_ = create(app.data(), argv.second.data(), err);
	throw_on_error(err, "failed to create process");
}
linapi::process::process(const char_t* app, char_t* const* args, std::error_code& err):
	native_(create(app, args, err))
{}
linapi::process::process(ready_args_t argv, std::error_code& err)
{
	string app = argv.first.front();
	native_ = create(app.data(), argv.second.data(), err);
}

linapi::process::process(initial_params& params, const char_t* app, char_t* const* args)
{
	std::error_code err;
	native_ = create(params, app, args, err);
	throw_on_error(err, "failed to create process");
}
linapi::process::process(initial_params& params, ready_args_t argv)
{
	string app = argv.first.front();
	std::error_code err;
	native_ = create(params, app.data(), argv.second.data(), err);
	throw_on_error(err, "failed to create process");
}
linapi::process::process(initial_params& params, const char_t* app, char_t* const* args, std::error_code& err):
	native_(create(params, app, args, err))
{}
linapi::process::process(initial_params& params, ready_args_t argv, std::error_code& err)
{
	string app = argv.first.front();
	native_ = create(params, app.data(), argv.second.data(), err);
}
#endif
