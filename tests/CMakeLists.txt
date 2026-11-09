add_library(doctest INTERFACE)
add_library(doctest::doctest ALIAS doctest)

target_include_directories(doctest SYSTEM INTERFACE
    "${PROJECT_SOURCE_DIR}/thirdparty"
)

find_package(Threads REQUIRED)

function(add_edu_test name)
    set(target edu_${name}_tests)
    add_executable(${target} main.cpp ${name}.cpp)
    target_link_libraries(${target} PRIVATE edu::edu doctest::doctest Threads::Threads)
    add_test(NAME ${name} COMMAND ${target})
endfunction()

add_edu_test(lock)
add_edu_test(lock_guard)
add_edu_test(scoped_lock)
add_edu_test(unique_lock)
