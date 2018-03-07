cmake_minimum_required(VERSION 3.9)

project(GetBilibili-C
        VERSION 0.1.0
        LANGUAGES C)

message(STATUS ${PROJECT_NAME})

set(CURL_DIR curl)
set(OpenSSL_DIR openssl)

set(CMAKE_C_FLAGS "-s -static -Os -Wall")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
set(CMAKE_PREFIX_PATH ${CURL_DIR} ${OpenSSL_DIR})
set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR} CACHE PATH "" FORCE)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

#find_library(LIBRARY_VAR1 wldap32)
#find_library(LIBRARY_VAR2 ws2_32)

add_subdirectory(src)
