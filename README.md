# 依赖
## GLFW https://www.glfw.org/download.html 管理窗口
## GLAD https://glad.dav1d.de/ 管理Graphics API函数指针
## GLM https://glm.g-truc.net/0.9.8/index.html 数学库
## Assimp 模型加载库
## stb_image 图像加载
## ImGui
## ImGuizmo

# Build
## 仅限Windows，需要VS2019及以上版本。直接打开sln，生成项目，将 lib\lib-vc2019 或 lib\lib-vc2022 下的 assimp-vc143-mt.dll 拷贝至exe所在目录。