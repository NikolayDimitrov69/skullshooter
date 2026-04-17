add_library(server_lib
    ClientRegistry.cpp
    ServerApp.cpp
)

target_include_directories(server_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(server_lib PUBLIC server_common)
add_executable(server
    main.cpp
)

target_precompile_headers(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/pch.h
)

target_link_libraries(server PRIVATE server_lib)
target_compile_features(server PRIVATE cxx_std_17)

target_compile_options(server PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/pch.h
)
