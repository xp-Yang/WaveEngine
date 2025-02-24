cmake_minimum_required(VERSION 3.13)

project(WaveEngine VERSION 0.9.0)

#导致assimp 编译失败
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/engine)
set(ENGINE_INCLUDE_DIR ${ENGINE_DIR}/include)
set(THIRD_PARTY_DIR ${ENGINE_DIR}/thirdparty)
set(ENGINE_RESOURCE_DIR ${ENGINE_DIR}/resource)
set(ENGINE_ASSET_DIR ${ENGINE_DIR}/asset)
set(ENGINE_SRC_DIR ${ENGINE_DIR}/src)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

add_subdirectory(engine)
add_subdirectory(samples)

#set_target_properties(MyTarget PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)