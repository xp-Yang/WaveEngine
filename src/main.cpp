#include "Application.hpp"

// TODO list:
// done: nanosuit 放在cube前加载就有问题: 答: body是nanosuit的最后一个材质，可能被cube覆盖了？已经无法复现。
// done: yoko 和 nanosuit 同时加载就有问题: 答: yoko模型没有镜面贴图，使用了nanosuit的镜面贴图导致的，加上default_map解决了
// done: main.cpp全局变量优化 答: 已将所有Object渲染对象交由Scene管理
// done: loop 中的逻辑分离 答: 分离了imgui的渲染
// done: 材质应该包含shader
// done: model.gs 的 explode 效果跟着fov缩放变 答：原因：learnOpengl的gs算法在裁剪空间计算法向量，而投影变换是非正交变换。
// done: 着色器的坐标空间理解和统一
// done: 键盘和鼠标callback事件用imgui处理
// done: 几何着色器法向量显示
// done: 键盘事件摄像机移动方向应该是摄像机坐标系方向
// done：当前相机拖动视角为FPS style,会导致相机旋转和平移多次操作会出现问题，相机的拖动导致距离渲染对象变近。需要实现拖动视角令相机在一个圆轨道运行
// done: 地板铺满
// done: 定义View、Scene对象职责 答：参考filament
// done: Model类Mesh和Material对应关系处理
// done: picking
// done: 实现ECS架构，Component数据驱动开发，main.cpp所有全局变量封装，所有逻辑分离
// done: 阴影贴图(done)、帧缓冲(done)、法线贴图、tbn矩阵、天空盒(done)、反射(done)等知识学习
// done: 抗锯齿
// done: 帧缓冲的附件理解
// done: 窗口大小可缩放
// done: deferred rendering 性能分析，见LightingPass.cpp
// 10. 解决相机运动死锁问题(direction = (0,-1,0)时)
// 11. 光源物理模型
// 12. tbn矩阵计算
// 13. 理解peter panning 效应
// 14. 看一下模型加载那篇文章，贴图文件是相对路径保存的或绝对路径保存的，Assimp的简单原理。
// 16. 粒子系统
// 18. low-poly
// 19. 天空盒的采样和着色器采样坐标系理解
// 19. 天空盒的模型矩阵大小代表着什么
// 19. 当前反射算法的弱点：距离不对
// 20. 动态环境贴图，球面时
// 23. 批渲染？
// 24. 学会renderdoc使用
// 25. 优化cpu和gpu的io调用
// 26. Editor 编辑材质后，保存文件
// 27. main的逻辑放入SandBox中
// 27. Material 频繁修改, 思考一下它的ECS架构
// 27. RenderSystem, 思考一下它的ECS架构
// 28. 线程分离
// 28. 实现gizmo
// 28. 实现事件机制
// 28. 实现窗口停靠，实现多viewport
// 28. 多视口
int main()
{
    auto app = Application::GetApp();
    app.init();
    app.run();
    app.shutdown();
    return 0;
}