#include "MyCube.hpp"
#include "stb_image.h"

MyCube::MyCube()
    : m_model_matrix(glm::mat4(1.0f))
{
    create_vbo();
    create_vao();
}

MyCube::MyCube(const Material& material)
{
    MyCube();
    m_material = material;
}

MyCube::~MyCube(){
    //glDeleteTextures(1, &m_texture_id);
    glDeleteVertexArrays(1, &m_vao_id);
}

void MyCube::set_material_diffuse_map(std::string map_path)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(map_path.c_str(), &width, &height, &nrChannels, 4);
    m_material.diffuse_map = generate_texture(width, height, data);
    m_material.diffuse_map_path = map_path;
    //�ͷ�ͼ����ڴ�
    stbi_image_free(data);
}

void MyCube::set_material_specular_map(std::string map_path)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(map_path.c_str(), &width, &height, &nrChannels, 4);
    m_material.specular_map = generate_texture(width, height, data);
    m_material.specular_map_path = map_path;
    stbi_image_free(data);
}

void MyCube::create_vbo()
{
    GLfloat cube_vertices[] =
    { 
     // ���ֻ��λ�ã�ֻ��Ҫ8��������У�����ÿ����ķ�������ͬ��������ͬλ�õĶ�����������ͬ�ķ��������ԣ�һ����Ҫ24��������ͬ�Ķ���
     // pos                  // normal              // uv           
     -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,    // 0  ���� ���ϣ���ǰ�濴��
     -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,    // 1  ���� ���£���ǰ�濴��
      0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,    // 2  ���� ���£���ǰ�濴��
      0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,    // 3  ���� ���ϣ���ǰ�濴��

     -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f,    // 4  ǰ�� ����
     -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,    // 5  ǰ�� ����
      0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f,    // 6  ǰ�� ���� 
      0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,    // 7  ǰ�� ���� 

     -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,    // 8  ����
     -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 9  ����
     -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,    // 10 ����
     -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 11 ����

      0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,    // 12 ����
      0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 13 ����
      0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,    // 14 ����
      0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 15 ����

     -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,    // 16 ���� ����
     -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,    // 17 ���� ����
      0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,    // 18 ���� ����
      0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f,    // 19 ���� ����

     -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f,    // 20 ���� ����
     -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f,    // 21 ���� ����
      0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,    // 22 ���� ����
      0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f,    // 23 ���� ����
    };
    glGenBuffers(1, &m_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint cubeIndices[] =
    {
        1 , 2 , 3 , 1 , 3 , 0 , //����
        5 , 6 , 7 , 5 , 7 , 4 , //ǰ��
        9 , 10, 11, 9 , 11, 8 , //����
        13, 14, 15, 13, 15, 12, //����
        17, 18, 19, 17, 19, 16, //����
        21, 22, 23, 21, 23, 20, //����
    };
    glGenBuffers(1, &m_ibo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_indices_count = sizeof(cubeIndices) / sizeof(cubeIndices[0]);

    // ��ʹ��ibo�ķ�ʽ
    //float cube_vertices[] = {
    //// pos                  // normal              // uv           
    //-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,    // 0  ���� ����   
    // 0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,    // 1  ���� ����   
    // 0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,    // 2  ���� ����   
    // 0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,    // 3  ���� ����   
    //-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,    // 4  ���� ����
    //-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,    // 5  ���� ����
    //                                                                    
    //-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,    // 6  ǰ�� ���� 
    // 0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f,    // 7  ǰ�� ���� 
    // 0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,    // 8  ǰ�� ���� 
    // 0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,    // 9  ǰ�� ���� 
    //-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f,    // 10 ǰ�� ����
    //-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,    // 11 ǰ�� ����
    //                                                                    
    //-0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 12 ���� ����
    //-0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,    // 13 ���� ����
    //-0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 14 ���� ����
    //-0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 15 ���� ����
    //-0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,    // 16 ���� ����
    //-0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 17 ���� ����
    //                                                                    
    // 0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 18 ���� ����
    // 0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f,    // 19 ���� ����
    // 0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 20 ���� ����
    // 0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f,    // 21 ���� ����
    // 0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f,    // 22 ���� ����
    // 0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f,    // 23 ���� ����
    //                                                                    
    //-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,    // 24 ���� ����
    // 0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f,    // 25 ���� ����
    // 0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f,    // 26 ���� ����
    // 0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f,    // 27 ���� ����
    //-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f,    // 28 ���� ����
    //-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f,    // 29 ���� ����
    //                                                                    
    //-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f,    // 30 ���� ����
    // 0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f,    // 31 ���� ����
    // 0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f,    // 32 ���� ����
    // 0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f,    // 33 ���� ����
    //-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f,    // 34 ���� ����
    //-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f,    // 35 ���� ����
    //};
    //glGenBuffers(1, &m_vbo_id);
    //glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

unsigned int MyCube::generate_texture(int width, int height, unsigned char* data) {
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //��һ������ָ��������Ŀ��(Target)������ΪGL_TEXTURE_2D��ζ�Ż������뵱ǰ�󶨵����������ͬһ��Ŀ���ϵ������κΰ󶨵�GL_TEXTURE_1D��GL_TEXTURE_3D���������ܵ�Ӱ�죩��
    //�ڶ�������Ϊ����ָ���༶��Զ����ļ��������ϣ�������ֶ�����ÿ���༶��Զ����ļ���Ļ�������������0��Ҳ���ǻ�������
    //��������������OpenGL����ϣ����������Ϊ���ָ�ʽ�����ǵ�ͼ��ֻ��RGBֵ���������Ҳ��������ΪRGBֵ��
    //���ĸ��͵���������������յ�����Ŀ�Ⱥ͸߶ȡ�����֮ǰ����ͼ���ʱ�򴢴������ǣ���������ʹ�ö�Ӧ�ı�����
    //�¸�����Ӧ�����Ǳ���Ϊ0����ʷ���������⣩��
    //���ߵڰ˸�����������Դͼ�ĸ�ʽ���������͡�����ʹ��RGBֵ�������ͼ�񣬲������Ǵ���Ϊchar(byte)���飬���ǽ��ᴫ���Ӧֵ��
    //���һ��������������ͼ�����ݡ�
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture_id;
}

void MyCube::create_vao() {
    //����VAO
    //VAO��һ�����������ж����������Ե�״̬��ϣ����洢�˶������ݵĸ�ʽ�Լ��������������VBO��������á�
    //VAO����û�д洢���������������ݣ���Щ��Ϣ�Ǵ洢��VBO�еģ�VAO�൱���ǶԺܶ��VBO�����ã���һЩVBO�����һ����Ϊһ������ͳһ����
    glGenVertexArrays(1, &m_vao_id);
    glBindVertexArray(m_vao_id);

    // ����λ������
    glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    //// ������ɫ����
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    // ���㷨��������
    glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // ��������
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_ibo_id());

    // uv ��������
    glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}