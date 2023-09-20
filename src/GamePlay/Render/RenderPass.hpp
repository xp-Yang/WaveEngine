#ifndef RenderPass_hpp
#define RenderPass_hpp

#include <vector>

enum class PassType {
	SHADOW_PASS,
	MAIN_CAMERA_PASS, // do msaa
	PICKING_PASS,
	POST_PROCESSING_PASS,
};

struct FrameBuffer {
	std::vector<int> attachments;
};

class RenderPass {
public:
	void prepare_data();
	void draw();

private:
	FrameBuffer m_frame_buffer;
};

#endif // !RenderPass_hpp

