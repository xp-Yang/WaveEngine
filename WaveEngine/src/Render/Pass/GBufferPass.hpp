#ifndef GBufferPass_hpp
#define GBufferPass_hpp

#include "RenderPass.hpp"

class GBufferPass : public RenderPass {
public:
    GBufferPass();
    void init() override;
    void draw() override;
};

#endif // !GBufferPass_hpp