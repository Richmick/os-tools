cmake_minimum_required(VERSION 3.30)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level_project)

if (is_top_level_project)
	find_package(os-tools REQUIRED)
	enable_experimental_std()
endif()

project("os-tools-sync" LANGUAGES CXX)

set(cxx_sources "main.cpp")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${cxx_sources})
target_link_libraries(${PROJECT_NAME} PRIVATE os-tools::os-tools)
enable_cxx_modules(${PROJECT_NAME} STANDARD 23 STD_MODULE ON)
