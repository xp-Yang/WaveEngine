#ifndef RenderSystem_hpp
#define RenderSystem_hpp

class RenderPipeline;

namespace ecs {

class RenderSystem {
public:
    void initPipeline();
    void onUpdate(/*const View& view*/);

private:
    RenderPipeline* m_render_pipeline{ nullptr };
};

}

#endif // !RenderSystem_hpp
