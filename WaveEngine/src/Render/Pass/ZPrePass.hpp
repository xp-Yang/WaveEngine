#ifndef ZPrePass_hpp
#define ZPrePass_hpp

#include "RenderPass.hpp"

class ZPrePass : public RenderPass {
public:
    ZPrePass();
    void draw() override;

protected:
    void init() override;
};

#endif // !ZPrePass_hpp