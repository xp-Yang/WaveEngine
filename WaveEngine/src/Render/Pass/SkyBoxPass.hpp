#ifndef SkyBoxPass_hpp
#define SkyBoxPass_hpp

#include "RenderPass.hpp"

class SkyBoxPass : public RenderPass {
public:
    SkyBoxPass();
    void init() override;
    void draw() override;
};

#endif // !SkyBoxPass_hpp