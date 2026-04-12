add_executable(embedded
    main.cpp
)

target_link_libraries(embedded PRIVATE
    client_lib
    server_lib
)

target_precompile_headers(embedded PRIVATE
    ${CMAKE_SOURCE_DIR}/src/client/pch.h
)