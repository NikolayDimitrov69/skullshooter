add_library(client_lib
    ClientApp.cpp
)

target_include_directories(client_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(client_lib PUBLIC client_common)

target_precompile_headers(client_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/pch.h
)

add_executable(client
    main.cpp
)

target_link_libraries(client PRIVATE client_lib)

target_compile_features(client PRIVATE cxx_std_17)