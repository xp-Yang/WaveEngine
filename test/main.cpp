#include "WaveEngine/Application.hpp"

// TODO list:
// done: nanosuit ����cubeǰ���ؾ�������: ��: body��nanosuit�����һ�����ʣ����ܱ�cube�����ˣ��Ѿ��޷����֡�
// done: yoko �� nanosuit ͬʱ���ؾ�������: ��: yokoģ��û�о�����ͼ��ʹ����nanosuit�ľ�����ͼ���µģ�����default_map�����
// done: main.cppȫ�ֱ����Ż� ��: �ѽ�����Object��Ⱦ������Scene����
// done: loop �е��߼����� ��: ������imgui����Ⱦ
// done: ����Ӧ�ð���shader
// done: model.gs �� explode Ч������fov���ű� ��ԭ��learnOpengl��gs�㷨�ڲü��ռ���㷨��������ͶӰ�任�Ƿ������任��
// done: ��ɫ��������ռ�����ͳһ
// done: ���̺����callback�¼���imgui����
// done: ������ɫ����������ʾ
// done: �����¼�������ƶ�����Ӧ�������������ϵ����
// done����ǰ����϶��ӽ�ΪFPS style,�ᵼ�������ת��ƽ�ƶ�β�����������⣬������϶����¾�����Ⱦ����������Ҫʵ���϶��ӽ��������һ��Բ�������
// done: �ذ�����
// done: ����View��Scene����ְ�� �𣺲ο�filament
// done: Model��Mesh��Material��Ӧ��ϵ����
// done: picking
// done: ʵ��ECS�ܹ���Component��������������main.cpp����ȫ�ֱ�����װ�������߼�����
// done: ��Ӱ��ͼ(done)��֡����(done)��������ͼ��tbn������պ�(done)������(done)��֪ʶѧϰ
// done: �����
// done: ֡����ĸ������
// done: ���ڴ�С������
// done: deferred rendering ���ܷ�������LightingPass.cpp
// done: ����imgizmo
// done: ����imgui����ͣ������viewport
// done: �������˶���������Ļ��ת���� ��ԭ����lookat()��������view����ʱ����direction��up������ʸ��ʹ�þ�������ʱ��ʸ��ת�����������ά����������camera_up�����ڼ���view����ʱ������lookat()���� camera_up����������ȫ��up�������ɡ�
// 9. ����Ⱦ���
// 11. ��Դ����ģ��
// 12. tbn�������
// 13. ���peter panning ЧӦ
// 14. ��һ��ģ�ͼ�����ƪ���£���ͼ�ļ������·������Ļ����·������ģ�Assimp�ļ�ԭ��
// 16. ����ϵͳ
// 18. low-poly
// 19. ��պеĲ�������ɫ����������ϵ���
// 19. ��պе�ģ�;����С������ʲô
// 19. ��ǰ�����㷨�����㣺���벻��
// 20. ��̬������ͼ������ʱ
// 24. ѧ��renderdocʹ��
// 25. �Ż�cpu��gpu��io����
// 26. Editor �༭���ʺ󣬱����ļ�
// 27. main���߼�����SandBox��
// 27. Material Ƶ���޸�, ˼��һ������ECS�ܹ�
// 27. RenderSystem, ˼��һ������ECS�ܹ�
// 28. �̷߳���
// 28. ʵ���¼�����
// 28. ���ء������ļ������л��ͷ����л�
// 28. �Ż�pickingPass, readpixel������¼�����һ��
// 28. ʵ��scenegraph�������������bbs��objectlist
// 28. ����fov����Ϊʲô�ȽϺ���
// 28. ȥ������gl�����
// 28. ���դ+���׷��opengl��դ+opengl��׷��opengl��դ+���׷��
int main()
{
    auto& app = Application::GetApp();
    app.init();
    app.run();
    app.shutdown();
    return 0;
}