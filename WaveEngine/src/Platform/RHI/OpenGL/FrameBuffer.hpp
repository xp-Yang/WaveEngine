#ifndef FrameBuffer_hpp
#define FrameBuffer_hpp

#include <vector>
#include "Core/Math.hpp"

enum class AttachmentType {// TODO 所有纹理格式
	RGB16F,
	RGBA,
	DEPTH24STENCIL8,
	DEPTH,
	STENCIL,
};

class Attachment {
public:
	void create(AttachmentType type, int color_attachment_index, int samples, int width, int height);
	AttachmentType getType() const;
	unsigned int getMap() const;

private:
	unsigned int m_map = 0;
	AttachmentType m_type = AttachmentType::RGBA;
};

// TODO multi-sample
class FrameBuffer {
public:
	FrameBuffer(int width, int height, int samples = 1);
	void create(const std::vector<AttachmentType>& attachments_type);
	void createDefault();
	unsigned int getFBO() const;
	int getWidth() const;
	int getHeight() const;
	const std::vector<Attachment>& getAttachments() const;
	const Attachment& getFirstAttachmentOf(AttachmentType type) const;
	void setSize(int width, int height);
	void setSamples(int samples);
	bool isMultiSampled() const;
	void blitColorMapTo(FrameBuffer* dest);
	void blitDepthMapTo(FrameBuffer* dest);
	void bind();
	void unBind();
	void clear(Color3 clear_color = Color3(1.0f));

private:
	unsigned int m_fbo = 0;
	std::vector<Attachment> m_attachments;
	int m_width = 0;
	int m_height = 0;
	int m_samples = 1;
	bool m_is_multi_sample = false;
};

#endif // !FrameBuffer_hpp
