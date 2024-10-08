#ifndef RenderPass_hpp
#define RenderPass_hpp

#include "Render/RenderSourceData.hpp"

static inline constexpr float DEFAULT_RENDER_RESOLUTION_X = 1920.0f;
static inline constexpr float DEFAULT_RENDER_RESOLUTION_Y = 1080.0f;

// Interface class
// each RenderPass corresponds to a framebuffer
// there could be different attachments(render targets, like texture/render buffer object) in one framebuffer
// perhaps we need subpass, so that can use input attachments to render
// a subpass (now as a RenderPass) may contain multiple graphics-pipeline, and execute graphics-pipeline multiple times 
// a graphics-pipeline need shader program and vertices to execute
class RenderPass {
public:
	enum class Type {
		Picking,
		Shadow,
		GBuffer,
		DeferredLighting,
		Transparent,

		// post process
		Bloom,
		Outline,
		Combined,

		WireFrame,
		CheckerBoard,
		Normal,
	};

	RenderPass() = default;
	RenderPass(const RenderPass&) = delete;
	RenderPass& operator=(const RenderPass&) = delete;
	virtual ~RenderPass() = default;
	virtual void init() = 0;
	virtual void draw() = 0;
	virtual void clear() {
		m_framebuffer->bind();
		m_framebuffer->clear();
	};
	void prepareRenderSourceData(const std::shared_ptr<RenderSourceData>& render_source_data) { 
		m_rhi = RenderSourceData::rhi;
		m_render_source_data = render_source_data;
	}
	void setInputPasses(const std::vector<RenderPass*>& input_passes) { m_input_passes = input_passes; }
	RhiFrameBuffer* getFrameBuffer() const { return m_framebuffer.get(); };

protected:
	std::shared_ptr<Rhi> m_rhi;
	std::shared_ptr<RenderSourceData> m_render_source_data;

	std::vector<RenderPass*> m_input_passes;
	std::unique_ptr<RhiFrameBuffer> m_framebuffer;
	Type m_type;
};

#endif // !RenderPass_hpp

