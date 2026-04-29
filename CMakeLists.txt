cmake_minimum_required(VERSION 3.20)

if(MSVC)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(nozzle-TOP VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architectures" FORCE)
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")
endif()

# --- nozzle (static library via submodule) ---
set(NOZZLE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/nozzle)

# --- TD SDK headers ---
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/td_sdk)

# --- Output directory ---
set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/plugin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY})

# --- Plugin builder function ---
function(build_td_plugin OPNAME)
    add_library(${OPNAME} MODULE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${OPNAME}.cpp
    )

    set_target_properties(${OPNAME} PROPERTIES LINKER_LANGUAGE CXX)
    set_target_properties(${OPNAME} PROPERTIES PREFIX "")
    set_target_properties(${OPNAME} PROPERTIES BUNDLE TRUE)
    set_target_properties(${OPNAME} PROPERTIES BUNDLE_EXTENSION "plugin")

    target_include_directories(${OPNAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(${OPNAME} PRIVATE nozzle)

    # Disable exceptions/RTTI to match nozzle
    target_compile_options(${OPNAME} PRIVATE
        $<$<CXX_COMPILER_ID:Clang,AppleClang>:-fno-exceptions -fno-rtti>
        $<$<CXX_COMPILER_ID:GNU>:-fno-exceptions -fno-rtti>
        $<$<CXX_COMPILER_ID:MSVC>:/EHs-c- /GR->
    )
endfunction()

# --- Build both plugins ---
build_td_plugin(NozzleSend)
build_td_plugin(NozzleReceive)
