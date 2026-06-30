cmake_minimum_required(VERSION 3.20)
project(mkxrdplinks VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(mkxrdplinks src/mkxrdplinks.cpp)

include(FetchContent)
FetchContent_Declare(
	cli11
	GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
	GIT_TAG		v2.5.0
)
FetchContent_MakeAvailable(cli11)

target_link_libraries(mkxrdplinks PRIVATE CLI11::CLI11)
