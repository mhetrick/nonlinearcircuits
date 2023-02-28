cmake_minimum_required(VERSION 3.16)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
project(NonlinearCircuits VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PLUGIN_NAME ${PROJECT_NAME})
set(ADDITIONAL_PLUGIN_DISTRIBUTABLES res README.md CHANGELOG.md)
include(RackSDK.cmake)

message(STATUS "Nonlinear Circuits for Rack Build Process" )
message(STATUS "Special Thanks to baconpaul" )
message(STATUS "Target plugin install path: '${CMAKE_INSTALL_PREFIX}'")

file(GLOB SOURCES src/*.cpp)
add_compile_options(-fvisibility=hidden -fvisibility-inlines-hidden)

target_include_directories(${RACK_PLUGIN_LIB} PRIVATE src ${RACK_SDK_DIR}/include)
target_sources(${RACK_PLUGIN_LIB} PRIVATE
        ${SOURCES})
