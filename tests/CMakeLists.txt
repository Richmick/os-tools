cmake_minimum_required(VERSION 3.30)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level_project)
if (is_top_level_project)
	enable_testing()
	find_package(os-tools REQUIRED)
	enable_experimental_std()
endif()

project(os-tools-tests LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

set(cxx_sources
	"boost-unit-main.cpp"
	"thread-test.cpp"
)
set(cxx_modules)
add_executable(os-tools-test_child "child-app/main.cpp")
enable_cxx_modules(os-tools-test_child STANDARD 23 STD_MODULE ON)
target_link_libraries(os-tools-test_child PRIVATE os-tools::os-tools)

add_executable(os-tools-tests)
add_dependencies(os-tools-tests os-tools-test_child)
target_sources(os-tools-tests PRIVATE ${cxx_sources})
target_sources(${PROJECT_NAME} PRIVATE FILE_SET test_modules TYPE CXX_MODULES FILES ${cxx_modules})
target_link_libraries(os-tools-tests PRIVATE os-tools::os-tools Boost::boost)
target_include_directories(os-tools-tests PRIVATE ${Boost_INCLUDE_DIRS})
enable_cxx_modules(os-tools-tests STANDARD 23 STD_MODULE ON)
