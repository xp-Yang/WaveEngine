#include "WaveEngine/Application.hpp"

// TODO list:
// done: nanosuit ����cubeǰ���ؾ�������: ��: body��nanosuit�����һ�����ʣ����ܱ�cube�����ˣ��Ѿ��޷����֡�
// done: yoko �� nanosuit ͬʱ���ؾ�������: ��: yokoģ��û�о�����ͼ��ʹ����nanosuit�ľ�����ͼ���µģ�����default_map�����
// done: main.cppȫ�ֱ����Ż� ��: �ѽ�����Object��Ⱦ������Scene����
// done: loop �е��߼����� ��: ������imgui����Ⱦ
// done: ����Ӧ�ð���shader
// done: model.gs �� explode Ч������fov���ű� ��ԭ��learnOpengl��gs�㷨�ڲü��ռ���㷨��������ͶӰ�任�Ƿ������任��
// done: ��ɫ��������ռ����
// done: ���̺����callback�¼���imgui����
// done: ������ɫ����������ʾ
// done: �����¼�������ƶ�����Ӧ�������������ϵ����
// done����ǰ����϶��ӽ�ΪFPS style,�ᵼ�������ת��ƽ�ƶ�β�����������⣬������϶����¾�����Ⱦ����������Ҫʵ���϶��ӽ��������һ��Բ�������
// done: ����View��Scene����ְ�� �𣺲ο�filament
// done: Model��Mesh��Material��Ӧ��ϵ����
// done: picking
// done: ʵ��ECS�ܹ���Component��������������main.cpp����ȫ�ֱ�����װ�������߼�����
// done: ��Ӱ��ͼ(done)��֡����(done)��������ͼ��tbn������պ�(done)������(done)��֪ʶѧϰ
// done: �����
// done: ֡����ĸ������
// done: ���ڴ�С������
// done: deferred rendering ���ܷ��� �𣺼�LightingPass.cpp�����ע��
// done: ����imgizmo
// done: ����imgui����ͣ������viewport
// done: �������˶���������Ļ��ת���� ��ԭ����lookat()��������view����ʱ����direction��up������ʸ��ʹ�þ�������ʱ��ʸ��ת�����������ά����������camera_up�����ڼ���view����ʱ������lookat()���� camera_up����������ȫ��up�������ɡ�
// done: ��պе�ģ�;����С������ʲô ��ģ�;�����Ҫ���ţ���Ϊshader��ǿ������gl_Position�����Ϊ1��
// done: ��պеĲ�������ɫ����������ϵ���
// done: ѧ��renderdocʹ�� ��ѧ�������ʹ����
// done: �ֶ�ʵ��glsl Ԥ����#include
// done: ����pbr
// done: ����ʵ��sceneHierarchy
// done: ʵ�ֵ��Դ�Ͷ����Դ������ģ��
// done: ʵʱedit���Դ����
// done: ʵ�������������shader
// done: �Ż�pickingPass, readpixel������¼�����һ��
// done: ʵ��scenegraph��������
// 35. gamma correction��⣬HDR���
// 36. ����Ⱦ���
// 37. ���ֹ�Դ������ģ��
// 38. tbn�������
// 39. ���peter panning ЧӦ
// 40. ��һ��ģ�ͼ�����ƪ���£���ͼ�ļ������·������Ļ����·������ģ�Assimp�ļ�ԭ��
// 41. ����ϵͳ
// 42. low-poly
// 43. ��ǰ�����㷨�����㣺���벻��
// 44. ��̬������ͼ������ʱ
// 45. �Ż�cpu��gpu��io����
// 46. Editor �༭���ʺ󣬱����ļ�
// 47. main���߼�����SandBox��
// 48. Material Ƶ���޸�, ˼��һ������ECS�ܹ�
// 49. RenderSystem, ˼��һ������ECS�ܹ�
// 50. �̷߳���
// 51. ʵ���¼�����
// 52. ���ء������ļ������л��ͷ����л�
// 53. ����fov����Ϊʲô�ȽϺ���
// 54. ȥ������gl�����
// 55. ���դ+���׷��opengl��դ+opengl��׷��opengl��դ+���׷��
// 56. ��������Ϊʲô����ͶӰ�����
int main()
{
    auto& app = Application::GetApp();
    app.init();
    app.run();
    app.shutdown();
    return 0;
}