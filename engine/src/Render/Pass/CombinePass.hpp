#ifndef CombinePass_hpp
#define CombinePass_hpp

#include "RenderPass.hpp"

class CombinePass : public RenderPass
{
public:
    CombinePass();
    void draw() override;
    void enableFXAA(bool enable);

protected:
    void init() override;

private:
    bool m_fxaa = false;

    std::unique_ptr<RhiFrameBuffer> m_default_framebuffer;
};

#endif
