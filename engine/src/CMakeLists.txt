set(WAVE_LIB_NAME WaveRuntime)

file(GLOB_RECURSE SRC_FILES "${ENGINE_SRC_DIR}/*.cpp" "${ENGINE_SRC_DIR}/*.c")
file(GLOB_RECURSE HEAD_FILES "${ENGINE_SRC_DIR}/*.hpp" "${ENGINE_SRC_DIR}/*.h" "${ENGINE_INCLUDE_DIR}/*.hpp" "${ENGINE_INCLUDE_DIR}/*.h")
file(GLOB_RECURSE RC_FILES "${ENGINE_RESOURCE_DIR}/*.*")

#使ide呈现文件夹层级结构
#Note: 只有add_library/add_executable中的文件才会被source_group分组
source_group(TREE "${ENGINE_DIR}" FILES ${SRC_FILES} ${HEAD_FILES} ${RC_FILES})
#下面这种方式不会形成树形结构
#source_group("" FILES ${SRC_FILES} ${HEAD_FILES})

#增加所有文件以便ide可以显示这些文件
add_library(${WAVE_LIB_NAME} ${SRC_FILES} ${HEAD_FILES})
#hack 通过这种方式使ide显示resource文件，否则resource中的.obj文件被错误地当成目标文件链接
set_source_files_properties(${RC_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
target_sources(${WAVE_LIB_NAME} PRIVATE ${RC_FILES})

target_include_directories(${WAVE_LIB_NAME}
	PUBLIC ${ENGINE_INCLUDE_DIR}
	PUBLIC ${ENGINE_SRC_DIR})

target_link_libraries(${WAVE_LIB_NAME} 
	PUBLIC glad
	PUBLIC imgui
	PUBLIC stb
	PUBLIC spdlog
	PUBLIC json11
	PUBLIC glm
	PUBLIC glfw
	PUBLIC assimp)

target_compile_features(${WAVE_LIB_NAME} PRIVATE cxx_std_17)