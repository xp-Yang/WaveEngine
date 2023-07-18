#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "myshader.hpp"
#include "stb_image.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    switch (key) {
    case GLFW_KEY_UP:

        break;
    case GLFW_KEY_LEFT:
        break;
    case GLFW_KEY_RIGHT:
        break;
    case GLFW_KEY_DOWN:
        break;
    case GLFW_KEY_1:
        break;
    case GLFW_KEY_2:
        break;
    case GLFW_KEY_3:
        break;
    case GLFW_KEY_4:
        break;
    case GLFW_KEY_SPACE:

        break;
    default:
        break;
    }
}

GLFWwindow* create_window(int size_x, int size_y) {
    //glfwInit函数来初始化GLFW，glfwWindowHint函数来配置GLFW
    glfwInit();
    //3.3版本对代码更严格了：比如：不能自己指定名称给glBindBuffer(),而是要使用glGenBuffers给我们返回的名称。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//告诉GLFW我们要使用的OpenGL版本是3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//告诉GLFW我们要使用的OpenGL版本是3.3,这样GLFW会在创建OpenGL上下文时做出适当的调整
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//明确告诉GLFW我们使用的是核心模式(Core-profile)
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//并且不允许用户调整窗口的大小

    //创建一个窗口对象
    GLFWwindow* window = glfwCreateWindow(size_x, size_y, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    //初始化GLAD，使其可以管理OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    return window;
}

void set_view_port(int width, int height) {
    // 设置Viewport
    // 它的定义具有两个意义。
    // 一个意义是它定义了上面定义的视景体中的景物将会绘制到一张什么尺寸的画布之上，
    // 另外一个意义表示这个绘制好的图像将会被显示在屏幕的什么区域。
    // 进行视口变换(Viewport Transform),标准化设备坐标会变换为屏幕空间坐标。所得的屏幕空间坐标又会被变换为片段输入到片段着色器中。
    glViewport(0, 0, width, height);
}

int main()
{
    GLFWwindow* window = create_window(1080, 720);

    //int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    set_view_port(720, 720);

    MyShader shader("../shader/vshader.vs", "../shader/fshader.fs");

    //创建VAO
    //VAO是一个保存了所有顶点数据属性的状态结合，它存储了顶点数据的格式以及顶点数据所需的VBO对象的引用。
    //VAO本身并没有存储顶点的相关属性数据，这些信息是存储在VBO中的，VAO相当于是对很多个VBO的引用，把一些VBO组合在一起作为一个对象统一管理。
    GLuint VAO1;//一个id，vertext array object 句柄
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);

    GLfloat cubeVertex[] =
    {
        -0.5f, -0.5f,  0.5f,     0.0f, 1.0f,     1.0f, 0.0f, 1.0f, 1.0f, // 0 左下
         0.5f, -0.5f,  0.5f,     0.0f, 1.0f,     0.0f, 1.0f, 1.0f, 1.0f, // 1 右下
         0.5f,  0.5f,  0.5f,     1.0f, 1.0f,     1.0f, 1.0f, 0.0f, 1.0f, // 2 右上
        -0.5f,  0.5f,  0.5f,     1.0f, 1.0f,     1.0f, 0.5f, 1.0f, 1.0f, // 3 左上
        -0.5f, -0.5f, -0.5f,     0.0f, 1.0f,     0.5f, 1.0f, 1.0f, 1.0f, // 4
        -0.5f,  0.5f, -0.5f,     1.0f, 1.0f,     1.0f, 1.0f, 0.5f, 1.0f, // 5
         0.5f,  0.5f, -0.5f,     1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f, // 6
         0.5f, -0.5f, -0.5f,     1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f, // 7
    };
    GLuint cube_vbo;
    glGenBuffers(1, &cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertex), cubeVertex, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLubyte cubeIndices[] =
    {
        0, 1, 2, 0, 2, 3, // Quad 0
        4, 5, 6, 4, 6, 7, // Quad 1
        5, 3, 2, 5, 2, 6, // Quad 2
        4, 7, 1, 4, 1, 0, // Quad 3
        7, 6, 2, 7, 2, 1, // Quad 4
        4, 0, 3, 4, 3, 5  // Quad 5
    };
    GLuint cube_ibo;
    glGenBuffers(1, &cube_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("../images/desert.jpg", &width, &height, &nrChannels, 0);
    GLuint cube_tex;
    glGenTextures(1, &cube_tex);
    glBindTexture(GL_TEXTURE_2D, cube_tex);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
    //第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
    //第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
    //第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
    //下个参数应该总是被设为0（历史遗留的问题）。
    //第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。
    //最后一个参数是真正的图像数据。
    glGenerateMipmap(GL_TEXTURE_2D);
    //释放图像的内存
    stbi_image_free(data);


    GLuint VAO2;//一个id，vertext array object 句柄
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    GLfloat triVertex[] =
    {
        -1.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, -1.0f, 0.0f,
    };
    //绑定第一个顶点缓冲对象VBO1
    GLuint tri_vbo;//一个名称（vertex buffer object 句柄）
	glGenBuffers(1, &tri_vbo);
    //第一个参数是要生成的缓冲对象的名称的数量，第二个是用来存储缓冲对象名称的数组
    //该函数会返回n个缓冲对象的名称到数组里。(它实际上并没有创建任何东西。它只返回当前未用作缓冲区名称的整数列表。)
    //glGenBuffers实际上根本不需要，它只是作为一个方便的函数来给你一个未使用的整数。
    //生成的名称由 GL 标记为已使用，仅用于名称生成。以这种方式标记的对象名称不会被其他调用返回以生成相同类型的名称，直到通过删除这些名称再次将其标记为未使用
	glBindBuffer(GL_ARRAY_BUFFER, tri_vbo);
    //第一个参数指定缓冲对象的类型，第二个参数指定缓冲对象的名称，也就是我们在glGenBuffers()里生成的名称
    //该函数创建 0 size 的缓冲对象，其具有默认状态 GL_READ_WRITE 和 GL_STATIC_DRAW
    //该函数将缓冲对象绑定到OpenGL上下文环境中。
	glBufferData(GL_ARRAY_BUFFER, sizeof(triVertex), triVertex, GL_STATIC_DRAW);
    //分配显存，真正生成缓冲对象，该缓冲对象具有名称(glGenBuffers)、类型(glBindBuffer);
    //它会把之前定义的顶点数据传输到当前绑定的显存缓冲区中，（顶点数据传入GPU）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    //通知OpenGL如何解释这些顶点数据
    //第一个参数指定顶点属性位置，与顶点着色器中layout(location=0)对应。
    //第二个参数指定顶点属性大小。
    //第三个参数指定数据类型。
    //第四个参数定义是否希望数据被标准化。
    //第五个参数是步长（Stride），指定在连续的顶点属性之间的间隔。
    //第六个参数表示我们的位置数据在缓冲区起始位置的偏移量。
	glEnableVertexAttribArray(0);



    glfwSetKeyCallback(window, key_callback);
    //Game Loop
    while (!glfwWindowShouldClose(window))
    {//渲染
        glfwPollEvents();//检查触发事件（比如键盘输入、鼠标移动等），然后调用对应的回调函数
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.start_using();
        GLfloat timeValue = glfwGetTime();
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, timeValue / 5.0f, glm::vec3(0.5f, 0.3f, 0.5f));
        shader.setMatrix("transform", 1, trans);

        GLfloat normalization_time = (sin(timeValue) / 3) + 0.6;
        shader.setFloat("time_var", normalization_time);

        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices), GL_UNSIGNED_BYTE, 0); // 使用_ibo指定的36个索引来绘制。 
        glBindVertexArray(0);

        //glBindVertexArray(VAO2);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glBindVertexArray(0);


        glfwSwapBuffers(window);//函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色的大缓冲），它在这一迭代中被用来绘制，并输出显示在屏幕上。
    }

    glDeleteVertexArrays(1, &VAO1);
    glDeleteVertexArrays(1, &VAO2);
    //glDeleteBuffers(1, &tri_vbo);
    //glDeleteBuffers(1, &VBO2);

    glfwTerminate();//调用glfwTerminate函数来释放GLFW分配的内存
    return 0;
}