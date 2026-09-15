cmake_minimum_required(VERSION 3.30)

include(cmake/import-std/enable_import_std.cmake)
include(cmake/import-std.cmake)

enable_experimental_std()
project("os-tools"
		VERSION 0.0.3
		DESCRIPTION "Os tools library"
		LANGUAGES CXX
)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level_project)

option(OS_TOOLS_BUILD_TESTS "Build os-tools tests" OFF)
option(OS_TOOLS_BUILD_EXAMPLES "Build os-tools examples" OFF)
option(OS_TOOLS_INSTALL "Generate target for installing os-tools" is_top_level_project)

set(OS_TOOLS_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
set(OS_TOOLS_MINOR_VERSION ${PROJECT_VERSION_MINOR})
set(OS_TOOLS_PATCH_VERSION ${PROJECT_VERSION_PATCH})
set(OS_TOOLS_VERSION ${PROJECT_VERSION})

include(GNUInstallDirs)

set(INSTALL_OS_TOOLS_LIB_DIR "${CMAKE_INSTALL_LIBDIR}" CACHE PATH "Installation directory for libraries")
set(INSTALL_OS_TOOLS_BIN_DIR "${CMAKE_INSTALL_BINDIR}" CACHE PATH "Installation directory for executables")
set(INSTALL_OS_TOOLS_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE PATH "Installation directory for header files")
set(DEF_INSTALL_OS_TOOLS_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/os-tools")
set(INSTALL_OS_TOOLS_CMAKE_DIR "${DEF_INSTALL_OS_TOOLS_CMAKE_DIR}" CACHE PATH "Installation directory for cmake files")


add_library(${PROJECT_NAME})
add_library(os-tools::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set(cxx_sources
	"os-tools/winapi/aliases.cpp"
	"os-tools/winapi/handle.cpp"
	"os-tools/winapi/thread.cpp"
	"os-tools/winapi/process.cpp"
	
	"os-tools/linapi/aliases.cpp"
	"os-tools/linapi/handle.cpp"
	"os-tools/linapi/thread.cpp"
	"os-tools/linapi/process.cpp"
)
set(cxx_modules
	"os-tools/args_parser.ixx"
	"os-tools/os.ixx"

	"os-tools/winapi.ixx"
	"os-tools/winapi/aliases.ixx"
	"os-tools/winapi/handle.ixx"
	"os-tools/winapi/thread.ixx"
	"os-tools/winapi/process.ixx"

	"os-tools/linapi.ixx"
	"os-tools/linapi/aliases.ixx"
	"os-tools/linapi/handle.ixx"
	"os-tools/linapi/thread.ixx"
	"os-tools/linapi/process.ixx"
)

target_sources(${PROJECT_NAME} PRIVATE ${cxx_sources})
target_sources(${PROJECT_NAME} PUBLIC FILE_SET os_tools_modules TYPE CXX_MODULES FILES ${cxx_modules})

source_group("Sources" FILES ${cxx_sources})
source_group("Interfaces" FILES ${cxx_modules})

set_target_properties(${PROJECT_NAME} PROPERTIES
	SOVERSION ${PROJECT_VERSION_MAJOR}
	VERSION ${PROJECT_VERSION}
)
enable_cxx_modules(${PROJECT_NAME} STANDARD 23 STD_MODULE ON)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

if (OS_TOOLS_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
if (OS_TOOLS_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

include(CMakePackageConfigHelpers)

if (OS_TOOLS_INSTALL AND NOT CMAKE_SKIP_INSTALL_RULES)
	configure_package_config_file(cmake/os-tools-config.cmake.in
		"${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/os-tools-config.cmake"
		INSTALL_DESTINATION "${INSTALL_OS_TOOLS_CMAKE_DIR}")
	write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/os-tools-config-version.cmake"
		COMPATIBILITY SameMinorVersion)

	install(TARGETS ${PROJECT_NAME}
		EXPORT os_tools_targets
		RUNTIME DESTINATION "${INSTALL_OS_TOOLS_BIN_DIR}" COMPONENT bin
		ARCHIVE DESTINATION "${INSTALL_OS_TOOLS_LIB_DIR}" COMPONENT lib
		LIBRARY DESTINATION "${INSTALL_OS_TOOLS_LIB_DIR}" COMPONENT lib
		FILE_SET os_tools_modules DESTINATION "${INSTALL_OS_TOOLS_INCLUDE_DIR}" COMPONENT lib
	)
	set(targets_file "os-tools-targets.cmake")
	install(EXPORT os_tools_targets
		COMPONENT dev
		FILE "${targets_file}"
		DESTINATION "${INSTALL_OS_TOOLS_CMAKE_DIR}"
		NAMESPACE os-tools::
	)
	install(FILES
		"cmake/import-std/enable_import_std.cmake"
		"cmake/import-std.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/os-tools-config.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/os-tools-config-version.cmake"
		COMPONENT dev
		DESTINATION "${INSTALL_OS_TOOLS_CMAKE_DIR}"
	)
endif()
