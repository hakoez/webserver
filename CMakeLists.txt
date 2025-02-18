cmake_minimum_required(VERSION 3.22)

project(WebX VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# VCPKG toolchain static path
set (VCPKG_DIR "/home/vcpkg")
include(${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake)


find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(httplib REQUIRED)
find_package(pugixml REQUIRED)
find_package(asio REQUIRED)


add_subdirectory(src)
add_subdirectory(include)
