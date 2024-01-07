set(TEST_EXEC_NAME Test)

file(GLOB_RECURSE HEAD_FILES "*.hpp" "*.h")
file(GLOB_RECURSE SRC_FILES "*.cpp" "*.c")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SRC_FILES} ${HEAD_FILES})
#增加HEAD_FILES以便ide可以显示这些头文件
add_executable(${TEST_EXEC_NAME} ${HEAD_FILES} ${SRC_FILES})

#设为vs默认启动项目
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TEST_EXEC_NAME})

target_link_libraries(${TEST_EXEC_NAME} WaveRuntime)

target_compile_features(${TEST_EXEC_NAME} PRIVATE cxx_std_17)