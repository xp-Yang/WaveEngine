#include "MyCube.hpp"
#include "stb_image.h"

MyCube::MyCube(const char* texture_path, const glm::vec3& color)
    : m_model_matrix(glm::mat4(1.0f))
    , m_color(color)
{
    create_vbo();

    if (texture_path) {
        m_texture_type = TextureType::Normal;
        int width, height, nrChannels;
        unsigned char* data = stbi_load(texture_path, &width, &height, &nrChannels, 4);
        generate_texture(width, height, data);
        //�ͷ�ͼ����ڴ�
        stbi_image_free(data);
    }
    else {
        m_texture_type = TextureType::None;
    }

    create_vao();
}

MyCube::~MyCube(){
    glDeleteTextures(1, &m_texuture_id);
    glDeleteVertexArrays(1, &m_vao_id);
}

void MyCube::create_vbo()
{
    GLfloat cubeVertex[] =
    {
        // vertex                  // color                   // normal                                    // uv         
        -0.2f, -0.2f,  0.2f,       1.0f, 0.0f, 0.0f,                                 // 0 ����              0.0f, 0.0f,   
         0.2f, -0.2f,  0.2f,       0.0f, 1.0f, 0.0f,                                 // 1 ����              1.0f, 0.0f,   
         0.2f,  0.2f,  0.2f,       0.0f, 0.0f, 1.0f,                                 // 2 ����              1.0f, 1.0f,   
        -0.2f,  0.2f,  0.2f,       1.0f, 0.0f, 1.0f,                                 // 3 ����              0.0f, 1.0f,   
        -0.2f, -0.2f, -0.2f,       1.0f, 0.0f, 0.0f,                                 // 4 ������           0.0f, 0.0f,   
        -0.2f,  0.2f, -0.2f,       0.0f, 1.0f, 0.0f,                                 // 5 ������           0.0f, 1.0f,   
         0.2f,  0.2f, -0.2f,       0.0f, 0.0f, 1.0f,                                 // 6 ������           1.0f, 1.0f,   
         0.2f, -0.2f, -0.2f,       1.0f, 0.0f, 1.0f,                                 // 7 ������           1.0f, 0.0f,   
    };
    glGenBuffers(1, &m_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertex), cubeVertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLubyte cubeIndices[] =
    {
        0, 1, 2, 0, 2, 3, // Quad 0 ǰ��
        4, 5, 6, 4, 6, 7, // Quad 1 ����
        5, 3, 2, 5, 2, 6, // Quad 2 ����
        4, 7, 1, 4, 1, 0, // Quad 3 ����
        7, 6, 2, 7, 2, 1, // Quad 4 ����
        4, 0, 3, 4, 3, 5, // Quad 5 ����
    };
    glGenBuffers(1, &m_ibo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLfloat texCoord[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
    };
    glGenBuffers(1, &m_uv_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_uv_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLfloat cubeNormal[] = {
       0.0f,  0.0f, 1.0f,
       0.0f,  0.0f, 1.0f,
       0.0f,  0.0f, 1.0f,
       0.0f,  0.0f, 1.0f,
       0.0f,  0.0f, 1.0f,
       0.0f,  0.0f, 1.0f,

       0.0f,  0.0f, -1.0f,
       0.0f,  0.0f, -1.0f,
       0.0f,  0.0f, -1.0f,
       0.0f,  0.0f, -1.0f,
       0.0f,  0.0f, -1.0f,
       0.0f,  0.0f, -1.0f,

       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,

       0.0f,  -1.0f,  0.0f,
       0.0f,  -1.0f,  0.0f,
       0.0f,  -1.0f,  0.0f,
       0.0f,  -1.0f,  0.0f,
       0.0f,  -1.0f,  0.0f,
       0.0f,  -1.0f,  0.0f,

       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,
       1.0f,  0.0f,  0.0f,

       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
    };
    glGenBuffers(1, &m_normal_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_normal_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormal), cubeNormal, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyCube::generate_texture(int width, int height, unsigned char* data) {
    glGenTextures(1, &m_texuture_id);
    glBindTexture(GL_TEXTURE_2D, m_texuture_id);
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
}

void MyCube::create_vao() {
    //����VAO
    //VAO��һ�����������ж����������Ե�״̬��ϣ����洢�˶������ݵĸ�ʽ�Լ��������������VBO��������á�
    //VAO����û�д洢���������������ݣ���Щ��Ϣ�Ǵ洢��VBO�еģ�VAO�൱���ǶԺܶ��VBO�����ã���һЩVBO�����һ����Ϊһ������ͳһ����
    glGenVertexArrays(1, &m_vao_id);
    glBindVertexArray(m_vao_id);

    // �������ݺͶ�����ɫ����
    glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // ������ɫ����
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ���㷨��������
    glBindBuffer(GL_ARRAY_BUFFER, get_normal_id());
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(0));
    glEnableVertexAttribArray(3);

    // ��������
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_ibo_id());

    if (m_texture_type == TextureType::Normal) {
        // ��������
        glBindTexture(GL_TEXTURE_2D, get_texture_id());

        // uv ��������
        glBindBuffer(GL_ARRAY_BUFFER, get_uv_id());
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)(0 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
}