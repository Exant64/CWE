cmake_minimum_required(VERSION 3.20)
project(CWE LANGUAGES CXX)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "This project requires a 32-bit build.")
endif()

add_subdirectory(CWE)

set_target_properties(CWE PROPERTIES
    OUTPUT_NAME "CWE"
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build"
)

target_compile_options(CWE PRIVATE -masm=intel)
target_compile_options(CWE PRIVATE -Wno-error=narrowing)

target_compile_definitions(CWE PRIVATE CWEV9 _UNICODE)


target_link_options(CWE PRIVATE
    -static-libgcc -static-libstdc++ -static
)

if (STRIP)
    target_link_options(CWE PRIVATE
        -s
    )
endif()

target_include_directories(CWE PRIVATE
    CWE
    CWE/navigation/external/Detour/Include
    CWE/navigation/external/Recast/Include
    api
    rapidjson/include/rapidjson
    external/xxhash
    external/sa2b-render-fix/api
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_include_directories(CWE PRIVATE external/minidx9/Include)
    # todo: lib here aswell
endif()

if(IMGUIDEBUG)
    target_compile_options(CWE PRIVATE -g)
    target_compile_definitions(CWE PRIVATE IMGUIDEBUG)
    target_link_libraries(CWE PRIVATE dwmapi gdi32)
endif()

target_link_libraries(CWE PRIVATE d3dx9)

target_compile_features(CWE PRIVATE cxx_std_20)