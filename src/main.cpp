#include "Application.hpp"

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
// 10. �������˶���������(direction = (0,-1,0)ʱ)
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
// 23. ����Ⱦ��
// 24. ѧ��renderdocʹ��
// 25. �Ż�cpu��gpu��io����
// 26. Editor �༭���ʺ󣬱����ļ�
// 27. main���߼�����SandBox��
// 27. Material Ƶ���޸�, ˼��һ������ECS�ܹ�
// 27. RenderSystem, ˼��һ������ECS�ܹ�
// 28. �̷߳���
// 28. ʵ��gizmo
// 28. ʵ���¼�����
// 28. ʵ�ִ���ͣ����ʵ�ֶ�viewport
// 28. ���ӿ�
int main()
{
    auto app = Application::GetApp();
    app.init();
    app.run();
    app.shutdown();
    return 0;
}