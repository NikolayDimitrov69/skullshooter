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

add_subdirectory(src/core)

add_library(common INTERFACE)
target_link_libraries(common INTERFACE
    core
    SFML::Network
)

add_library(client_common INTERFACE)
target_link_libraries(client_common INTERFACE
    common
    SFML::Graphics
    SFML::Audio
)

add_library(server_common INTERFACE)
target_link_libraries(server_common INTERFACE
    common
)

add_subdirectory(src/server)
add_subdirectory(src/client)
add_subdirectory(src/embedded)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT embedded)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

add_dependencies(client copy_assets)
add_dependencies(embedded copy_assets)