cmake_minimum_required(VERSION 3.20)

project(edu_cpp_concurrency LANGUAGES CXX)

add_library(edu INTERFACE)
add_library(edu::edu ALIAS edu)

target_compile_features(edu INTERFACE cxx_std_23)
target_include_directories(edu INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

include(CTest)

if(BUILD_TESTING)
    add_library(doctest INTERFACE)
    add_library(doctest::doctest ALIAS doctest)

    target_include_directories(doctest SYSTEM INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty"
    )
endif()
