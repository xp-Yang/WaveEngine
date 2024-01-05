set(WAVE_LIB_NAME WaveRuntime)

#TODO 减少重复代码编写
#include(${THIRD_PARTY_DIR}/CMakeLists.txt)

#TODO 处理这些thirdparty文件，是否需要显示在ide src层级中
file(GLOB_RECURSE GLAD_SRC_FILES "${THIRD_PARTY_DIR}/glad/*.c")

file(GLOB_RECURSE IMGUI_SRC_FILES "${THIRD_PARTY_DIR}/imgui/*.c" "${THIRD_PARTY_DIR}/imgui/*.cpp")

file(GLOB_RECURSE STB_SRC_FILES "${THIRD_PARTY_DIR}/stb/*.c" "${THIRD_PARTY_DIR}/stb/*.cpp")

set(THIRD_PARTY_SRC_FILES
${GLAD_SRC_FILES}
${IMGUI_SRC_FILES}
${STB_SRC_FILES})

file(GLOB_RECURSE SRC_FILES "${ENGINE_SRC_DIR}/*.cpp" "${ENGINE_SRC_DIR}/*.c")
file(GLOB_RECURSE HEAD_FILES "${ENGINE_SRC_DIR}/*.hpp" "${ENGINE_SRC_DIR}/*.h")

#使ide呈现文件夹层级结构
#Note: 只有add_library/add_executable中的文件才会被source_group分组
source_group(TREE "${ENGINE_SRC_DIR}" FILES ${SRC_FILES} ${HEAD_FILES})
#下面这种方式不会形成树形结构
#source_group("" FILES ${SRC_FILES} ${HEAD_FILES})

#增加HEAD_FILES以便ide可以显示这些头文件
# TODO 将THIRD_PARTY_SRC_FILES自己制作成库
add_library(${WAVE_LIB_NAME} ${SRC_FILES} ${HEAD_FILES} ${THIRD_PARTY_SRC_FILES})

#使用PUBLIC，这样其他target target_link_libraries ${WAVE_LIB_NAME}这个target时，就会传递后面的include目录给其他target
target_include_directories(${WAVE_LIB_NAME} PUBLIC ${THIRD_PARTY_DIR} ${GLFW_INCLUDE_DIR} ${GLAD_INCLUDE_DIR} ${STB_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR} ${ENGINE_SRC_DIR})

#使用PUBLIC，这样其他target target_link_libraries ${WAVE_LIB_NAME}这个target时，也能访问glfw这个target
target_link_libraries(${WAVE_LIB_NAME} PUBLIC glfw)
target_link_libraries(${WAVE_LIB_NAME} PUBLIC assimp)

target_compile_features(${WAVE_LIB_NAME} PRIVATE cxx_std_17)