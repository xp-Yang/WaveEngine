#ifndef RenderPass_hpp
#define RenderPass_hpp

#include <vector>

enum class PassType {
	SHADOW_PASS,
	MAIN_CAMERA_PASS, // where do msaa
	PICKING_PASS,
	POST_PROCESSING_PASS,
	SCREEN_PASS,
};

struct FrameBuffer {
	std::vector<int> attachments;
};

class RenderPass {
public:
	virtual void init() = 0;
	// TODO
	// virtual void prepare_data(const FrameBuffer& fb);
	virtual void prepare_data(unsigned int fbo = -1, unsigned int map = -1) = 0;
	virtual void draw() = 0;
	// TODO
	// map 和 fbo 封装在FrameBuffer里
	virtual unsigned int get_fbo() { return m_fbo; }
	virtual unsigned int get_map() { return m_map; }

protected:
	unsigned int m_fbo = -1;
	unsigned int m_map = -1;
};

#endif // !RenderPass_hpp

