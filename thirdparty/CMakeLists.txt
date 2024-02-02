set(third_party_folder "ThirdParty")

#glad
file(GLOB_RECURSE GLAD_HEAD_FILES "${THIRD_PARTY_DIR}/glad/*.h" "${THIRD_PARTY_DIR}/glad/*.hpp")
file(GLOB_RECURSE GLAD_SRC_FILES "${THIRD_PARTY_DIR}/glad/*.c" "${THIRD_PARTY_DIR}/glad/*.cpp")
add_library(glad ${GLAD_HEAD_FILES} ${GLAD_SRC_FILES})
set_target_properties(glad PROPERTIES FOLDER ${third_party_folder}/glad)
target_include_directories(glad PRIVATE ${GLAD_INCLUDE_DIR})
	
#imgui
file(GLOB_RECURSE IMGUI_HEAD_FILES "${THIRD_PARTY_DIR}/imgui/*.h" "${THIRD_PARTY_DIR}/imgui/*.hpp")
file(GLOB_RECURSE IMGUI_SRC_FILES "${THIRD_PARTY_DIR}/imgui/*.c" "${THIRD_PARTY_DIR}/imgui/*.cpp")
add_library(imgui ${IMGUI_HEAD_FILES} ${IMGUI_SRC_FILES})
set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui)
target_include_directories(imgui PRIVATE ${THIRD_PARTY_DIR}/imgui)
target_include_directories(imgui PRIVATE ${GLFW_INCLUDE_DIR})

#stb
file(GLOB_RECURSE STB_HEAD_FILES "${THIRD_PARTY_DIR}/stb/*.h" "${THIRD_PARTY_DIR}/stb/*.hpp")
file(GLOB_RECURSE STB_SRC_FILES "${THIRD_PARTY_DIR}/stb/*.c" "${THIRD_PARTY_DIR}/stb/*.cpp")
add_library(stb ${STB_HEAD_FILES} ${STB_SRC_FILES})
set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)
target_include_directories(stb PRIVATE ${THIRD_PARTY_DIR}/stb)

#glm
if(NOT TARGET glm)
	option(BUILD_STATIC_LIBS "" ON)
	add_subdirectory(glm)
	set_target_properties(glm_static PROPERTIES FOLDER ${third_party_folder}/glm)
endif()

#glfw
if(NOT TARGET glfw)
	option(GLFW_BUILD_EXAMPLES "" OFF)
	option(GLFW_BUILD_TESTS "" OFF)
	option(GLFW_BUILD_DOCS "" OFF)
	option(GLFW_INSTALL "" OFF)
	add_subdirectory(glfw)
	set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
	set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
endif()

#assimp
if(NOT TARGET assimp)
	option(BUILD_SHARED_LIBS "" OFF)
	option(ASSIMP_BUILD_SAMPLES "" OFF)
	option(ASSIMP_BUILD_TESTS "" OFF)
	option(ASSIMP_WARNINGS_AS_ERRORS "" OFF)
	option(ASSIMP_INSTALL "" OFF)
	option(ASSIMP_BUILD_ASSIMP_TOOLS "" OFF)
	option(ASSIMP_BUILD_ASSIMP_TOOLS "" OFF)
	option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT "" OFF)
	option(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT "" OFF)
	add_subdirectory(assimp)
	set_target_properties(assimp PROPERTIES FOLDER ${third_party_folder}/assimp)
	set_target_properties(uninstall PROPERTIES FOLDER ${third_party_folder}/assimp)
	set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER ${third_party_folder}/assimp)
	#set_target_properties(zlibstatic PROPERTIES FOLDER ${third_party_folder}/assimp)
endif()