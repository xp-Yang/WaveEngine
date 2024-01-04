cmake_minimum_required(VERSION 3.13)

project(WaveEngine VERSION 0.9.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/thirdparty)
set(GLFW_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/glfw/include)
set(GLAD_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/glad/include)
set(ASSIMP_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/assimp/include)
set(RESOURCE_DIR ${PROJECT_SOURCE_DIR}/resource)
set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/engine)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)

#TODO 自定义输出binary的路径
#set_target_properties(MyTarget PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

#TODO dll和resource需要手动拷贝怎么处理

add_subdirectory(thirdparty)
add_subdirectory(engine)
add_subdirectory(test)