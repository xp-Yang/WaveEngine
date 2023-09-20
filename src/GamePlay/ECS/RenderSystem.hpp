#ifndef RenderSystem_hpp
#define RenderSystem_hpp

namespace ecs {

struct RenderSystem {
    static void onUpdate(/*const View& view*/);
    static void render_picking_border();
    static void render_normal();
    static void render_shadow_map();
};

}

#endif // !RenderSystem_hpp
