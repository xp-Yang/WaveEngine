#ifndef RayTracingPass_hpp
#define RayTracingPass_hpp

#include "RenderPass.hpp"

class RayTracingPass : public RenderPass {
public:
    void init() override;
    void draw() override;
};

#endif
