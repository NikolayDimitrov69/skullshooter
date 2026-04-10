cmake_minimum_required(VERSION 3.28)

project(skullshooter LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(skullshooter src/main.cpp)

target_include_directories(skullshooter PRIVATE include)

target_compile_features(skullshooter PRIVATE cxx_std_17)

target_precompile_headers(skullshooter PRIVATE include/pch.h)

target_link_libraries(skullshooter PRIVATE
    SFML::Graphics
    SFML::Audio
    SFML::Network
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT skullshooter)

add_custom_command(TARGET skullshooter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:skullshooter>/assets
)