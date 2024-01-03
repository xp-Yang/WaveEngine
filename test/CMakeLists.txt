set(TEST_EXEC_NAME Test)

file(GLOB_RECURSE HEAD_FILES "*.hpp")
file(GLOB_RECURSE SRC_FILES "*.cpp")

add_executable(${TEST_EXEC_NAME} ${HEAD_FILES} ${SRC_FILES})

target_include_directories(${TEST_EXEC_NAME} PUBLIC ${THIRD_PARTY_DIR} ${GLFW_INCLUDE_DIR} ${GLAD_INCLUDE_DIR} ${STB_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR} ${ENGINE_DIR})

# TODO target_link_directories
link_directories(${THIRD_LIB_DIR})
target_link_libraries(${TEST_EXEC_NAME} WaveRuntime)