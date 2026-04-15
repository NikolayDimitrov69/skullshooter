add_library(core
    GameState.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(core PUBLIC cxx_std_17)

target_precompile_headers(core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/pch.h
)

target_link_libraries(core PUBLIC
    SFML::System
)

target_compile_options(core PRIVATE  /FI${CMAKE_CURRENT_SOURCE_DIR}/pch.h)