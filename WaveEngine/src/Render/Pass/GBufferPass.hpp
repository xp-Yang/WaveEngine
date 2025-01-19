#ifndef GBufferPass_hpp
#define GBufferPass_hpp

#include "RenderPass.hpp"

class GBufferPass : public RenderPass {
public:
    GBufferPass();
    void draw() override;
    void enablePBR(bool enable);

protected:
    void init() override;

private:
    bool m_pbr = false;
};

#endif // !GBufferPass_hpp