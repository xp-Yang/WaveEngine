#TODO thirdparty如何处理
# thirdparty有各种形式：直接包含头文件和源文件；编译成静态库使用；编译成动态库使用

# glad imgui stb glm 包含源代码即用
file(GLOB_RECURSE GLAD_HEAD_FILES "glad/*.h")
file(GLOB_RECURSE GLAD_SRC_FILES "glad/*.c")

file(GLOB_RECURSE IMGUI_HEAD_FILES "imgui/*.h" "imgui/*.hpp")
file(GLOB_RECURSE IMGUI_SRC_FILES "imgui/*.c" "imgui/*.cpp")

file(GLOB_RECURSE STB_HEAD_FILES "stb/*.h" "stb/*.hpp")
file(GLOB_RECURSE STB_SRC_FILES "stb/*.c" "stb/*.cpp")

file(GLOB_RECURSE GLFW_HEAD_FILES "glfw/*.h" "glfw/*.hpp")

file(GLOB_RECURSE ASSIMP_HEAD_FILES "assimp/*.h" "assimp/*.hpp")

file(GLOB_RECURSE GLM_HEAD_FILES "glm/*.h" "glm/*.hpp" "glm/*.inl")

set(THIRD_PARTY_SRC_FILES
${GLAD_SRC_FILES}
${IMGUI_SRC_FILES}
${STB_SRC_FILES})

# glfw assimp 需要编译静态库使用
add_subdirectory(glfw)
add_subdirectory(assimp)