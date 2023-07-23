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
        //释放图像的内存
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
        -0.2f, -0.2f,  0.2f,       1.0f, 0.0f, 0.0f,                                 // 0 左下              0.0f, 0.0f,   
         0.2f, -0.2f,  0.2f,       0.0f, 1.0f, 0.0f,                                 // 1 右下              1.0f, 0.0f,   
         0.2f,  0.2f,  0.2f,       0.0f, 0.0f, 1.0f,                                 // 2 右上              1.0f, 1.0f,   
        -0.2f,  0.2f,  0.2f,       1.0f, 0.0f, 1.0f,                                 // 3 左上              0.0f, 1.0f,   
        -0.2f, -0.2f, -0.2f,       1.0f, 0.0f, 0.0f,                                 // 4 后：左下           0.0f, 0.0f,   
        -0.2f,  0.2f, -0.2f,       0.0f, 1.0f, 0.0f,                                 // 5 后：左上           0.0f, 1.0f,   
         0.2f,  0.2f, -0.2f,       0.0f, 0.0f, 1.0f,                                 // 6 后：右上           1.0f, 1.0f,   
         0.2f, -0.2f, -0.2f,       1.0f, 0.0f, 1.0f,                                 // 7 后：右下           1.0f, 0.0f,   
    };
    glGenBuffers(1, &m_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertex), cubeVertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLubyte cubeIndices[] =
    {
        0, 1, 2, 0, 2, 3, // Quad 0 前面
        4, 5, 6, 4, 6, 7, // Quad 1 后面
        5, 3, 2, 5, 2, 6, // Quad 2 上面
        4, 7, 1, 4, 1, 0, // Quad 3 下面
        7, 6, 2, 7, 2, 1, // Quad 4 右面
        4, 0, 3, 4, 3, 5, // Quad 5 左面
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
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
    //第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
    //第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
    //第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
    //下个参数应该总是被设为0（历史遗留的问题）。
    //第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。
    //最后一个参数是真正的图像数据。
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void MyCube::create_vao() {
    //创建VAO
    //VAO是一个保存了所有顶点数据属性的状态结合，它存储了顶点数据的格式以及顶点数据所需的VBO对象的引用。
    //VAO本身并没有存储顶点的相关属性数据，这些信息是存储在VBO中的，VAO相当于是对很多个VBO的引用，把一些VBO组合在一起作为一个对象统一管理。
    glGenVertexArrays(1, &m_vao_id);
    glBindVertexArray(m_vao_id);

    // 顶点数据和顶点颜色数据
    glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // 顶点颜色数据
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 顶点法向量数据
    glBindBuffer(GL_ARRAY_BUFFER, get_normal_id());
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(0));
    glEnableVertexAttribArray(3);

    // 索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_ibo_id());

    if (m_texture_type == TextureType::Normal) {
        // 纹理数据
        glBindTexture(GL_TEXTURE_2D, get_texture_id());

        // uv 坐标数据
        glBindBuffer(GL_ARRAY_BUFFER, get_uv_id());
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)(0 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
}