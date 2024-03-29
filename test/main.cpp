#include "WaveEngine/Application.hpp"

// TODO list:
// done: nanosuit 放在cube前加载就有问题: 答: body是nanosuit的最后一个材质，可能被cube覆盖了？已经无法复现。
// done: yoko 和 nanosuit 同时加载就有问题: 答: yoko模型没有镜面贴图，使用了nanosuit的镜面贴图导致的，加上default_map解决了
// done: main.cpp全局变量优化 答: 已将所有Object渲染对象交由Scene管理
// done: loop 中的逻辑分离 答: 分离了imgui的渲染
// done: 材质应该包含shader
// done: model.gs 的 explode 效果跟着fov缩放变 答：原因：learnOpengl的gs算法在裁剪空间计算法向量，而投影变换是非正交变换。
// done: 着色器的坐标空间理解
// done: 键盘和鼠标callback事件用imgui处理
// done: 几何着色器法向量显示
// done: 键盘事件摄像机移动方向应该是摄像机坐标系方向
// done：当前相机拖动视角为FPS style,会导致相机旋转和平移多次操作会出现问题，相机的拖动导致距离渲染对象变近。需要实现拖动视角令相机在一个圆轨道运行
// done: 定义View、Scene对象职责 答：参考filament
// done: Model类Mesh和Material对应关系处理
// done: picking
// done: 实现ECS架构，Component数据驱动开发，main.cpp所有全局变量封装，所有逻辑分离
// done: 阴影贴图(done)、帧缓冲(done)、法线贴图、tbn矩阵、天空盒(done)、反射(done)等知识学习
// done: 抗锯齿
// done: 帧缓冲的附件理解
// done: 窗口大小可缩放
// done: deferred rendering 性能分析 答：见LightingPass.cpp的相关注释
// done: 集成imgizmo
// done: 集成imgui窗口停靠，多viewport
// done: 解决相机运动至极点屏幕反转问题 答：原因是lookat()函数计算view矩阵时，用direction×up计算右矢，使得经过极点时右矢翻转。解决方法：维护相机自身的camera_up，并在计算view矩阵时，传给lookat()函数 camera_up参数而不是全局up参数即可。
// done: 天空盒的模型矩阵大小代表着什么 答：模型矩阵不需要缩放，因为shader中强行设置gl_Position的深度为1了
// done: 天空盒的采样和着色器采样坐标系理解
// done: 学会renderdoc使用 答：学会基本的使用了
// done: 手动实现glsl 预处理#include
// done: 引入pbr
// done: 初步实现sceneHierarchy
// done: 实现点光源和定向光源的物理模型
// done: 实时edit点光源数量
// done: 实现棋盘网格填充shader
// done: 优化pickingPass, readpixel用鼠标事件处理一次
// done: 实现scenegraph场景管理
// 35. gamma correction理解，HDR理解
// 36. 批渲染理解
// 37. 各种光源的物理模型
// 38. tbn矩阵计算
// 39. 理解peter panning 效应
// 40. 看一下模型加载那篇文章，贴图文件是相对路径保存的或绝对路径保存的，Assimp的简单原理。
// 41. 粒子系统
// 42. low-poly
// 43. 当前反射算法的弱点：距离不对
// 44. 动态环境贴图，球面时
// 45. 优化cpu和gpu的io调用
// 46. Editor 编辑材质后，保存文件
// 47. main的逻辑放入SandBox中
// 48. Material 频繁修改, 思考一下它的ECS架构
// 49. RenderSystem, 思考一下它的ECS架构
// 50. 线程分离
// 51. 实现事件机制
// 52. 加载、保存文件，序列化和反序列化
// 53. 调整fov缩放为什么比较合理
// 54. 去除所有gl裸调用
// 55. 软光栅+软光追，opengl光栅+opengl光追，opengl光栅+软光追，
// 56. 重心坐标为什么不是投影不变的
int main()
{
    auto& app = Application::GetApp();
    app.run();
    app.shutdown();
    return 0;
}