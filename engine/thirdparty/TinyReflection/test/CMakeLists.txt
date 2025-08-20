cmake_minimum_required(VERSION 3.13)
set(EXEC_NAME Test)
project(${EXEC_NAME} VERSION 0.9.0)

file(GLOB_RECURSE HEAD_FILES "*.hpp" "*.h")
file(GLOB_RECURSE SRC_FILES "*.cpp" "*.c")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SRC_FILES} ${HEAD_FILES})
#增加HEAD_FILES以便ide可以显示这些头文件
add_executable(${EXEC_NAME} ${HEAD_FILES} ${SRC_FILES})

#设为vs默认启动项目
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXEC_NAME})

target_include_directories(${EXEC_NAME}
	PUBLIC ${TINY_REFLECTION_INCLUDE_DIR})

target_link_libraries(${EXEC_NAME} 
	PRIVATE TinyReflection)

target_compile_features(${EXEC_NAME} PRIVATE cxx_std_17)