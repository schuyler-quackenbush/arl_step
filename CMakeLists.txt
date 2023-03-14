#See ReadMe.md, line 9 for how to execute this and dependent cmake files

cmake_minimum_required(VERSION 3.18)
project(step)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if (APPLE)
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14")
endif()

set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_ROOT_DIR $ENV{HOME}/local/openssl CACHE PATH "OpenSSL path")
set(OPENSSL_INCLUDE_DIR $ENV{HOME}/local/openssl/include CACHE PATH "OpenSSL include path")
set(portaudio_DIR $ENV{HOME}/local/portaudio/lib/cmake/portaudio CACHE PATH "PortAudio path")

find_package(OpenSSL REQUIRED)
find_package(portaudio REQUIRED)
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(SHORT_VERSION "3.01")
set(VERSION "1")
message(STATUS "Version: ${SHORT_VERSION} ${VERSION}")

add_subdirectory(port_src)
add_subdirectory(Step)
