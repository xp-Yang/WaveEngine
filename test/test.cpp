//GLuint test_vao()
//{
    //   GLfloat triVertex[] =
    //   {
    //       -1.0f, -1.0f, 0.0f,
    //       -0.5f, -0.5f, 0.0f,
    //       -0.5f, -1.0f, 0.0f,
    //   };
    //   //�󶨵�һ�����㻺�����VBO1
    //   GLuint tri_vbo;//һ�����ƣ�vertex buffer object �����
    //   glGenBuffers(1, &tri_vbo);
    //   //��һ��������Ҫ���ɵĻ����������Ƶ��������ڶ����������洢����������Ƶ�����
    //   //�ú����᷵��n�������������Ƶ������(��ʵ���ϲ�û�д����κζ�������ֻ���ص�ǰδ�������������Ƶ������б�)
    //   //glGenBuffersʵ���ϸ�������Ҫ����ֻ����Ϊһ������ĺ���������һ��δʹ�õ�������
    //   //���ɵ������� GL ���Ϊ��ʹ�ã��������������ɡ������ַ�ʽ��ǵĶ������Ʋ��ᱻ�������÷�����������ͬ���͵����ƣ�ֱ��ͨ��ɾ����Щ�����ٴν�����Ϊδʹ��
    //   glBindBuffer(GL_ARRAY_BUFFER, tri_vbo);
    //   //��һ������ָ�������������ͣ��ڶ�������ָ�������������ƣ�Ҳ����������glGenBuffers()�����ɵ�����
    //   //�ú������� 0 size �Ļ�����������Ĭ��״̬ GL_READ_WRITE �� GL_STATIC_DRAW
    //   //�ú������������󶨵�OpenGL�����Ļ����С�
    //   glBufferData(GL_ARRAY_BUFFER, sizeof(triVertex), triVertex, GL_STATIC_DRAW);
    //   //�����Դ棬ʹ����������������ݣ��û�������������(glGenBuffers)������(glBindBuffer);
    //   //�����֮ǰ����Ķ������ݴ��䵽��ǰ�󶨵��Դ滺�����У����������ݴ���GPU��
    
    //   //����VAO
    //   //VAO��һ�����������ж����������Ե�״̬��ϣ����洢�˶������ݵĸ�ʽ�Լ��������������VBO��������á�
    //   //VAO����û�д洢���������������ݣ���Щ��Ϣ�Ǵ洢��VBO�еģ�VAO�൱���ǶԺܶ��VBO�����ã���һЩVBO�����һ����Ϊһ������ͳһ����
    //   glGenVertexArrays(1, &m_vao_id); //һ��id��vertext array object ���
    //   glBindVertexArray(m_vao_id);
    //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    //   //֪ͨOpenGL��ν�����Щ��������
    //   //��һ������ָ����������λ�ã��붥����ɫ����layout(location=0)��Ӧ��
    //   //�ڶ�������ָ���������Դ�С��
    //   //����������ָ���������͡�
    //   //���ĸ����������Ƿ�ϣ�����ݱ���׼����
    //   //����������ǲ�����Stride����ָ���������Ķ�������֮��ļ����
    //   //������������ʾ���ǵ�λ�������ڻ�������ʼλ�õ�ƫ������
    //   glEnableVertexAttribArray(0);
    //   // ����λ������
    //   glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    //   glEnableVertexAttribArray(0);
    //   //// ������ɫ����
    //   //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
    //   //glEnableVertexAttribArray(1);
    //   // ���㷨��������
    //   glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    //   glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
    //   glEnableVertexAttribArray(3);
    //
    //   // ��������
    //   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get_ibo_id());
    //
    //   // uv ��������
    //   glBindBuffer(GL_ARRAY_BUFFER, get_vbo_id());
    //   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
    //   glEnableVertexAttribArray(2);

//    return 0;
//}

//unsigned int generate_texture(int width, int height, unsigned char* data) {
//    unsigned int texture_id;
//    glGenTextures(1, &texture_id);
//    glBindTexture(GL_TEXTURE_2D, texture_id);
//    // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    //��һ������ָ��������Ŀ��(Target)������ΪGL_TEXTURE_2D��ζ�Ż������뵱ǰ�󶨵����������ͬһ��Ŀ���ϵ������κΰ󶨵�GL_TEXTURE_1D��GL_TEXTURE_3D���������ܵ�Ӱ�죩��
//    //�ڶ�������Ϊ����ָ���༶��Զ����ļ��������ϣ�������ֶ�����ÿ���༶��Զ����ļ���Ļ�������������0��Ҳ���ǻ�������
//    //��������������OpenGL����ϣ����������Ϊ���ָ�ʽ�����ǵ�ͼ��ֻ��RGBֵ���������Ҳ��������ΪRGBֵ��
//    //���ĸ��͵���������������յ�����Ŀ�Ⱥ͸߶ȡ�����֮ǰ����ͼ���ʱ�򴢴������ǣ���������ʹ�ö�Ӧ�ı�����
//    //�¸�����Ӧ�����Ǳ���Ϊ0����ʷ���������⣩��
//    //���ߵڰ˸�����������Դͼ�ĸ�ʽ���������͡�����ʹ��RGBֵ�������ͼ�񣬲������Ǵ���Ϊchar(byte)���飬���ǽ��ᴫ���Ӧֵ��
//    //���һ��������������ͼ�����ݡ�
//    glGenerateMipmap(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    return texture_id;
//}

//void set_view_port(int width, int height) {
    // ����Viewport
    // ���Ķ�������������塣
    // һ�������������������涨����Ӿ����еľ��ｫ����Ƶ�һ��ʲô�ߴ�Ļ���֮�ϣ�
    // ����һ�������ʾ������ƺõ�ͼ�񽫻ᱻ��ʾ����Ļ��ʲô����
    // �����ӿڱ任(Viewport Transform),��׼���豸�����任Ϊ��Ļ�ռ����ꡣ���õ���Ļ�ռ������ֻᱻ�任ΪƬ�����뵽Ƭ����ɫ���С�
    //glViewport(0, 0, width, height);
//}

//m_projection_matrix = glm::perspective(deg2rad(45.0f), /*1.0f*/WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
//���ĵ�һ������������fov��ֵ�������Ҫһ����ʵ�Ĺ۲�Ч��������ֵͨ������Ϊ45.0f������Ҫһ��ĩ�շ��Ľ������Խ�������һ�������ֵ��
//�ڶ������������˿�߱ȣ����ӿڵĿ���Ը����á���߱�Ϊ1�Ļ����ӿڵĿ�߱�Ӱ������Ⱦ������������Ŀ�߱ȡ�
//�����͵��ĸ�����������ƽ��ͷ��Ľ���Զƽ�档����ͨ�����ý�����Ϊ0.1f����Զ������Ϊ100.0f�������ڽ�ƽ���Զƽ�����Ҵ���ƽ��ͷ���ڵĶ��㶼�ᱻ��Ⱦ��