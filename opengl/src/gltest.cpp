#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "MyShader.hpp"
#include "MyCube.hpp"
#include "MyCamera.hpp"
#include "stb_image.h"

#define WINDOW_WIDTH 1080.0f
#define WINDOW_HEIGHT 720.0f

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 project = glm::perspective(glm::radians(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
//它的第一个参数定义了fov的值。如果想要一个真实的观察效果，它的值通常设置为45.0f，但想要一个末日风格的结果你可以将其设置一个更大的值。
//第二个参数设置了宽高比，由视口的宽除以高所得。宽高比为1的话，视口的宽高比影响了渲染出来的立方体的宽高比。
//第三和第四个参数设置了平截头体的近和远平面。我们通常设置近距离为0.1f，而远距离设为100.0f。所有在近平面和远平面内且处于平截头体内的顶点都会被渲染。

MyCamera camera({ 0.0f, 0.5f, 2.0f }, glm::vec3(0.0f));

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lastFrameX = WINDOW_WIDTH / 2;
float lastFrameY = WINDOW_HEIGHT / 2;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    camera.key_process(key, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{ 
    static bool firstMouse = true;
    if (firstMouse)
    {
        lastFrameX = xpos;
        lastFrameY = ypos;
        firstMouse = false;
        return;
    }

    float delta_x = xpos - lastFrameX;
    float delta_y = -(ypos - lastFrameY); // 注意这里是相反的，因为glfwSetCursorPosCallback返回给mouse_callback函数的 (x,y) 是鼠标相对于窗口左上角的位置，所以需要将 (ypos - lastY) 取反
    lastFrameX = xpos;
    lastFrameY = ypos;

    camera.mouse_process(delta_x, delta_y);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.mouse_scroll_process(yoffset);
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
    GLFWwindow* window = create_window(WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);

    //int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    set_view_port(WINDOW_WIDTH, WINDOW_HEIGHT);

    MyShader shader("../shader/vshader.vs", "../shader/fshader.fs");

    MyCube cube("../images/desert.jpg");

    shader.start_using();
    shader.setMatrix("view", 1, camera.get_view());
    shader.setMatrix("projection", 1, project);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(0.6f,  0.3f, -0.4f),
        glm::vec3(-0.3f, -0.2f, -0.3f),
    };

    //Game Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();//检查触发事件（比如键盘输入、鼠标移动等），然后调用对应的回调函数
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer

        shader.start_using();

        GLfloat timeValue = glfwGetTime();
        deltaTime = timeValue - lastFrame;
        lastFrame = timeValue;

        GLfloat normalization_time = (sin(timeValue) / 3) + 0.6;
        shader.setFloat("time_var", normalization_time);

        shader.setMatrix("view", 1, camera.get_view());
        shader.setMatrix("projection", 1, project);

        glBindVertexArray(cube.get_vao_id());
        for (int i = 0; i < 3; i++) {
            auto rotate = glm::rotate(glm::mat4(1.0f), normalization_time * 20.0f, glm::vec3(0.5f, 0.3f, 0.5f));
            auto translate = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            model = rotate * translate * glm::mat4(1.0f);
            shader.setMatrix("model", 1, model);
            glDrawElements(GL_TRIANGLES, cube.get_elements_count(), GL_UNSIGNED_BYTE, 0); // 使用cube.ibo指定的36个索引来绘制。 
        }
        glBindVertexArray(0);

        //glBindVertexArray(VAO2);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glBindVertexArray(0);

        glfwSwapBuffers(window);//函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色的大缓冲），它在这一迭代中被用来绘制，并输出显示在屏幕上。
    }

    glfwTerminate();//调用glfwTerminate函数来释放GLFW分配的内存
    return 0;
}

GLuint test_vao()
{
    GLuint test_vao;//一个id，vertext array object 句柄
    glGenVertexArrays(1, &test_vao);
    glBindVertexArray(test_vao);
    //   GLfloat triVertex[] =
    //   {
    //       -1.0f, -1.0f, 0.0f,
    //       -0.5f, -0.5f, 0.0f,
    //       -0.5f, -1.0f, 0.0f,
    //   };
    //   //绑定第一个顶点缓冲对象VBO1
    //   GLuint tri_vbo;//一个名称（vertex buffer object 句柄）
    //   glGenBuffers(1, &tri_vbo);
    //   //第一个参数是要生成的缓冲对象的名称的数量，第二个是用来存储缓冲对象名称的数组
    //   //该函数会返回n个缓冲对象的名称到数组里。(它实际上并没有创建任何东西。它只返回当前未用作缓冲区名称的整数列表。)
    //   //glGenBuffers实际上根本不需要，它只是作为一个方便的函数来给你一个未使用的整数。
    //   //生成的名称由 GL 标记为已使用，仅用于名称生成。以这种方式标记的对象名称不会被其他调用返回以生成相同类型的名称，直到通过删除这些名称再次将其标记为未使用
    //   glBindBuffer(GL_ARRAY_BUFFER, tri_vbo);
    //   //第一个参数指定缓冲对象的类型，第二个参数指定缓冲对象的名称，也就是我们在glGenBuffers()里生成的名称
    //   //该函数创建 0 size 的缓冲对象，其具有默认状态 GL_READ_WRITE 和 GL_STATIC_DRAW
    //   //该函数将缓冲对象绑定到OpenGL上下文环境中。
    //   glBufferData(GL_ARRAY_BUFFER, sizeof(triVertex), triVertex, GL_STATIC_DRAW);
    //   //分配显存，真正生成缓冲对象，该缓冲对象具有名称(glGenBuffers)、类型(glBindBuffer);
    //   //它会把之前定义的顶点数据传输到当前绑定的显存缓冲区中，（顶点数据传入GPU）
    //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    //   //通知OpenGL如何解释这些顶点数据
    //   //第一个参数指定顶点属性位置，与顶点着色器中layout(location=0)对应。
    //   //第二个参数指定顶点属性大小。
    //   //第三个参数指定数据类型。
    //   //第四个参数定义是否希望数据被标准化。
    //   //第五个参数是步长（Stride），指定在连续的顶点属性之间的间隔。
    //   //第六个参数表示我们的位置数据在缓冲区起始位置的偏移量。
    //   glEnableVertexAttribArray(0);
    return test_vao;
}