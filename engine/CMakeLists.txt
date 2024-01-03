set(WAVE_LIB_NAME WaveRuntime)

#include(${THIRD_PARTY_DIR}/CMakeLists.txt)

file(GLOB_RECURSE GLAD_HEAD_FILES "../thirdparty/glad/*.h")
file(GLOB_RECURSE GLAD_SRC_FILES "../thirdparty/glad/*.c")

file(GLOB_RECURSE IMGUI_HEAD_FILES "../thirdparty/imgui/*.h" "../thirdparty/imgui/*.hpp")
file(GLOB_RECURSE IMGUI_SRC_FILES "../thirdparty/imgui/*.c" "../thirdparty/imgui/*.cpp")

file(GLOB_RECURSE STB_HEAD_FILES "../thirdparty/stb/*.h" "../thirdparty/stb/*.hpp")
file(GLOB_RECURSE STB_SRC_FILES "../thirdparty/stb/*.c" "../thirdparty/stb/*.cpp")

file(GLOB_RECURSE GLFW_HEAD_FILES "../thirdparty/glfw/*.h" "../thirdparty/glfw/*.hpp")

file(GLOB_RECURSE ASSIMP_HEAD_FILES "../thirdparty/assimp/*.h" "../thirdparty/assimp/*.hpp")

file(GLOB_RECURSE GLM_HEAD_FILES "../thirdparty/glm/*.h" "../thirdparty/glm/*.hpp" "../thirdparty/glm/*.inl")

set(THIRD_PARTY_SRC_FILES
${GLAD_SRC_FILES}
${IMGUI_SRC_FILES}
${STB_SRC_FILES})

file(GLOB_RECURSE SRC_FILES "*.cpp")

add_library(${WAVE_LIB_NAME} ${SRC_FILES} ${THIRD_PARTY_SRC_FILES})

target_include_directories(${WAVE_LIB_NAME} PUBLIC ${THIRD_PARTY_DIR} ${ENGINE_DIR})

link_directories(${THIRD_LIB_DIR})
# TODO
target_link_libraries(${WAVE_LIB_NAME} ${THIRD_LIB_DIR}/glfw3.lib ${THIRD_LIB_DIR}/assimp-vc143-mt.lib)