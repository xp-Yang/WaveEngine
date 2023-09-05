#ifndef FrameBufferQuad_hpp
#define FrameBufferQuad_hpp

class FrameBufferQuad
{
public:
    FrameBufferQuad();
    unsigned int get_quad_VAO() { return m_VAO; }

private:
    void create();
    unsigned int m_VAO = -1;
};

#endif
