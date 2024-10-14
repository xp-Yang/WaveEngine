#ifndef ZPrePass_hpp
#define ZPrePass_hpp

#include "RenderPass.hpp"

class ZPrePass : public RenderPass {
public:
    ZPrePass();
    void init() override;
    void draw() override;
};

#endif // !ZPrePass_hpp