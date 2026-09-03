include_guard(GLOBAL)

macro (enable_cxx_modules TARGET)
	cmake_parse_arguments(_ECM "" "STANDARD;STD_MODULE" "" ${ARGN})

	if (NOT _ECM_STANDARD)
		set(_ECM_STANDARD 23)
	endif()
	if (NOT DEFINED _ECM_STD_MODULE)
		if (_ECM_STANDARD LESS 23)
			set(_ECM_STD_MODULE OFF)
		else()
			set(_ECM_STD_MODULE ON)
		endif()
	endif()

	if (${_ECM_STD_MODULE} AND (_ECM_STANDARD LESS 23))
		message(FATAL_ERROR "requested C++${__EN_CXX_MODS_STANDARD} and std module but"
			" 'import std;' requires C++23 or newer.")
	endif()

	if(NOT CMAKE_GENERATOR MATCHES "Ninja" AND NOT CMAKE_GENERATOR MATCHES "Visual Studio")
		message(WARNING
			"Generator '${CMAKE_GENERATOR}' does not support C++ modules. "
			"Reconfigure with -G Ninja (or a Visual Studio generator).")
	endif()

	set_target_properties(${TARGET} PROPERTIES
		CXX_STANDARD ${_ECM_STANDARD}
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS ON
		CXX_MODULE_STD ${_ECM_STD_MODULE}
		CXX_SCAN_FOR_MODULES ON
	)

	unset(_ECM_STANDARD)
	unset(_ECM_STD_MODULE)
	unset(_ECM_UNPARSED_ARGUMENTS)
	unset(_ECM_KEYWORDS_MISSING_VALUES)
endmacro()
