add_library(client_lib
    ClientApp.cpp
)

target_include_directories(client_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(client_lib PUBLIC client_common)

add_executable(client
    main.cpp
)

target_precompile_headers(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/pch.h
)

target_link_libraries(client PRIVATE client_lib)

target_compile_features(client PRIVATE cxx_std_17)

target_compile_options(client PRIVATE
    /FI${CMAKE_CURRENT_SOURCE_DIR}/pch.h
)