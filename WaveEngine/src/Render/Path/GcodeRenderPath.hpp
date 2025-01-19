#ifndef GcodeRenderPath_hpp
#define GcodeRenderPath_hpp

#include "RenderPath.hpp"

#include "Logical/Gcode/GcodeViewer.hpp"
#include "Logical/Gcode/GcodeViewerInstancing.hpp"

class GcodeRenderPath : public RenderPath {
public:
    GcodeRenderPath();
    void render() override;

protected:
    std::shared_ptr<GcodeViewer> m_gcode_viewer;
};

#endif // !GcodeRenderPath_hpp