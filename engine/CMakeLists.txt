set(WAVE_LIB_NAME WaveRuntime)

#TODO 减少重复代码编写
#include(${THIRD_PARTY_DIR}/CMakeLists.txt)

file(GLOB_RECURSE GLAD_SRC_FILES "${THIRD_PARTY_DIR}/glad/*.c")

file(GLOB_RECURSE IMGUI_SRC_FILES "${THIRD_PARTY_DIR}/imgui/*.c" "${THIRD_PARTY_DIR}/imgui/*.cpp")

file(GLOB_RECURSE STB_SRC_FILES "${THIRD_PARTY_DIR}/stb/*.c" "${THIRD_PARTY_DIR}/stb/*.cpp")

set(THIRD_PARTY_SRC_FILES
${GLAD_SRC_FILES}
${IMGUI_SRC_FILES}
${STB_SRC_FILES})

file(GLOB_RECURSE SRC_FILES "*.cpp")

add_library(${WAVE_LIB_NAME} ${SRC_FILES} ${THIRD_PARTY_SRC_FILES})

target_include_directories(${WAVE_LIB_NAME} PUBLIC ${THIRD_PARTY_DIR} ${GLFW_INCLUDE_DIR} ${GLAD_INCLUDE_DIR} ${STB_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR} ${ENGINE_DIR})

target_link_libraries(${WAVE_LIB_NAME} PUBLIC glfw)
target_link_libraries(${WAVE_LIB_NAME} PUBLIC assimp)

target_compile_features(${WAVE_LIB_NAME} PRIVATE cxx_std_17)