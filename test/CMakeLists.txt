set(TEST_EXEC_NAME Test)

file(GLOB_RECURSE HEAD_FILES "*.hpp")
file(GLOB_RECURSE SRC_FILES "*.cpp")

add_executable(${TEST_EXEC_NAME} ${HEAD_FILES} ${SRC_FILES})

# TODO 只应该include WaveRuntime库提供的头文件以便调用WaveRuntime库
target_include_directories(${TEST_EXEC_NAME} PUBLIC ${THIRD_PARTY_DIR} ${GLFW_INCLUDE_DIR} ${GLAD_INCLUDE_DIR} ${STB_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR} ${ENGINE_DIR})

target_link_libraries(${TEST_EXEC_NAME} WaveRuntime)

target_compile_features(${TEST_EXEC_NAME} PRIVATE cxx_std_17)