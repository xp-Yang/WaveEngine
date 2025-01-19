#ifndef SkyBoxPass_hpp
#define SkyBoxPass_hpp

#include "RenderPass.hpp"

class SkyBoxPass : public RenderPass {
public:
    SkyBoxPass();
    void draw() override;

protected:
    void init();
};

#endif // !SkyBoxPass_hpp