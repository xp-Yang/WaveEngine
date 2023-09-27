#ifndef FrameBuffer_hpp
#define FrameBuffer_hpp

#include <vector>

enum class AttachmentType {
	RGB,
	DEPTH24STENCIL8,
	Depth,
	Stencil,
};

class Attachment {
public:
	void create(AttachmentType type, int color_attachment_index, int samples, int width, int height);
	AttachmentType getType() const;
	unsigned int getMap() const;

private:
	unsigned int m_map = 0;
	AttachmentType m_type = AttachmentType::RGB;
};

// TODO multi-sample
class FrameBuffer {
public:
	FrameBuffer(int width, int height, int samples = 1);
	void create(const std::vector<AttachmentType>& attachments_type);
	unsigned int getFBO() const;
	int getWidth() const;
	int getHeight() const;
	const std::vector<Attachment>& getAttachments() const;
	const Attachment& getFirstAttachmentOf(AttachmentType type) const;
	void setSize(int width, int height);
	void setSamples(int samples);
	bool isMultiSampled() const;
	void blitColorMapTo(FrameBuffer* dest);
	void blitShadowMapTo(FrameBuffer* dest);
	void bind();
	void unBind();
	void clear();

private:
	unsigned int m_fbo = 0;
	std::vector<Attachment> m_attachments;
	int m_width = 0;
	int m_height = 0;
	int m_samples = 1;
	bool m_is_multi_sample = false;
};

#endif // !FrameBuffer_hpp
