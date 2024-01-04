# thirdparty的依赖有各种形式：直接包含源代码使用；编译成静态库使用；编译成动态库使用

# glad imgui stb glm 包含源代码即用
#file(GLOB_RECURSE GLAD_HEAD_FILES "glad/*.h" "glad/*.hpp")
file(GLOB_RECURSE GLAD_SRC_FILES "glad/*.c" "glad/*.cpp")

#file(GLOB_RECURSE IMGUI_HEAD_FILES "imgui/*.h" "imgui/*.hpp")
file(GLOB_RECURSE IMGUI_SRC_FILES "imgui/*.c" "imgui/*.cpp")

#file(GLOB_RECURSE STB_HEAD_FILES "stb/*.h" "stb/*.hpp")
file(GLOB_RECURSE STB_SRC_FILES "stb/*.c" "stb/*.cpp")

# glm is header-only
#file(GLOB_RECURSE GLM_HEAD_FILES "glm/*.h" "glm/*.hpp" "glm/*.inl")
#file(GLOB_RECURSE GLM_SRC_FILES "glm/*.c" "glm/*.cpp")

#file(GLOB_RECURSE GLFW_HEAD_FILES "glfw/*.h" "glfw/*.hpp")

#file(GLOB_RECURSE ASSIMP_HEAD_FILES "assimp/*.h" "assimp/*.hpp")

set(THIRD_PARTY_SRC_FILES
${GLAD_SRC_FILES}
${IMGUI_SRC_FILES}
${STB_SRC_FILES}
CACHE INTERNAL ""
)

# glfw assimp 需要编译静态库使用
if(NOT TARGET glfw)
	option(GLFW_BUILD_EXAMPLES "" OFF)
	option(GLFW_BUILD_TESTS "" OFF)
	option(GLFW_BUILD_DOCS "" OFF)
	option(GLFW_INSTALL "" OFF)
	add_subdirectory(glfw)
endif()

if(NOT TARGET assimp)
	option(BUILD_SHARED_LIBS "" OFF)
	add_subdirectory(assimp)
endif()