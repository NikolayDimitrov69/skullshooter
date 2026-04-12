add_library(server_lib
    ServerApp.cpp
)

target_include_directories(server_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(server_lib PUBLIC server_common)

target_precompile_headers(server_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/pch.h
)

add_executable(server
    main.cpp
)

target_link_libraries(server PRIVATE server_lib)

target_compile_features(server PRIVATE cxx_std_17)