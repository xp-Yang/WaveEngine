cmake_minimum_required(VERSION 3.13)

project(WaveEngine VERSION 0.9.0)

#导致assimp 编译失败
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/WaveEngine)
set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/thirdparty)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(ENGINE_SRC_DIR ${ENGINE_DIR}/src)
set(ENGINE_RESOURCE_DIR ${ENGINE_DIR}/resource)
set(ENGINE_ASSET_DIR ${ENGINE_DIR}/asset)

set(ENGINE_INCLUDE_DIR ${ENGINE_DIR}/include)
set(IMGUI_INCLUDE_DIR ${THIRD_PARTY_DIR})
set(GLFW_INCLUDE_DIR ${THIRD_PARTY_DIR}/glfw/include)
set(GLAD_INCLUDE_DIR ${THIRD_PARTY_DIR}/glad/include)
set(ASSIMP_INCLUDE_DIR ${THIRD_PARTY_DIR}/assimp/include)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

add_subdirectory(thirdparty)
add_subdirectory(WaveEngine)
add_subdirectory(test)

#通过宏定义告诉程序resource目录
target_compile_definitions(WaveRuntime PUBLIC RESOURCE_DIR="${ENGINE_RESOURCE_DIR}")
target_compile_definitions(WaveRuntime PUBLIC ASSET_DIR="${ENGINE_ASSET_DIR}")

file(COPY ${PROJECT_SOURCE_DIR}/imgui.ini DESTINATION ${CMAKE_BINARY_DIR}/test)
#TODO 自定义输出binary的路径
#set_target_properties(MyTarget PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)