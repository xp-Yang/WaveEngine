//GLuint test_vao()
//{
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
    //   //分配显存，使缓冲对象真正有数据，该缓冲对象具有名称(glGenBuffers)、类型(glBindBuffer);
    //   //它会把之前定义的顶点数据传输到当前绑定的显存缓冲区中，（顶点数据传入GPU）
    
    //   //创建VAO
    //   //VAO是一个保存了所有顶点数据属性的状态结合，它存储了顶点数据的格式以及顶点数据所需的VBO对象的引用。
    //   //VAO本身并没有存储顶点的相关属性数据，这些信息是存储在VBO中的，VAO相当于是对很多个VBO的引用，把一些VBO组合在一起作为一个对象统一管理。
    //   glGenVertexArrays(1, &m_vao_id); //一个id，vertext array object 句柄
    //   glBindVertexArray(m_vao_id);
    //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    //   //通知OpenGL如何解释这些顶点数据
    //   //第一个参数指定顶点属性位置，与顶点着色器中layout(location=0)对应。
    //   //第二个参数指定顶点属性大小。
    //   //第三个参数指定数据类型。
    //   //第四个参数定义是否希望数据被标准化。
    //   //第五个参数是步长（Stride），指定在连续的顶点属性之间的间隔。
    //   //第六个参数表示我们的位置数据在缓冲区起始位置的偏移量。
    //   glEnableVertexAttribArray(0);
    //   // 顶点位置数据
    //   glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    //   glEnableVertexAttribArray(0);
    //   //// 顶点颜色数据
    //   //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
    //   //glEnableVertexAttribArray(1);
    //   // 顶点法向量数据
    //   glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    //   glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
    //   glEnableVertexAttribArray(3);
    //
    //   // 索引数据
    //   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_ibo_id());
    //
    //   // uv 坐标数据
    //   glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    //   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
    //   glEnableVertexAttribArray(2);

//    return 0;
//}

//unsigned int generate_texture(int width, int height, unsigned char* data) {
//    unsigned int texture_id;
//    glGenTextures(1, &texture_id);
//    glBindTexture(GL_TEXTURE_2D, texture_id);
//    // 为当前绑定的纹理对象设置环绕、过滤方式
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    //第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
//    //第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
//    //第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
//    //第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
//    //下个参数应该总是被设为0（历史遗留的问题）。
//    //第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。
//    //最后一个参数是真正的图像数据。
//    glGenerateMipmap(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    return texture_id;
//}

//void set_view_port(int width, int height) {
    // 设置Viewport
    // 它的定义具有两个意义。
    // 一个意义是它定义了上面定义的视景体中的景物将会绘制到一张什么尺寸的画布之上，
    // 另外一个意义表示这个绘制好的图像将会被显示在屏幕的什么区域。
    // 进行视口变换(Viewport Transform),标准化设备坐标会变换为屏幕空间坐标。所得的屏幕空间坐标又会被变换为片段输入到片段着色器中。
    //glViewport(0, 0, width, height);
//}

//m_projection_matrix = glm::perspective(deg2rad(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
//它的第一个参数定义了fov的值。如果想要一个真实的观察效果，它的值通常设置为45.0f，但想要一个末日风格的结果你可以将其设置一个更大的值。
//第二个参数设置了宽高比，由视口的宽除以高所得。宽高比为1的话，视口的宽高比影响了渲染出来的立方体的宽高比。
//第三和第四个参数设置了平截头体的近和远平面。我们通常设置近距离为0.1f，而远距离设为100.0f。所有在近平面和远平面内且处于平截头体内的顶点都会被渲染。