#ifndef FrameBuffer_hpp
#define FrameBuffer_hpp
#include <vector>

struct Attachment {
	enum Type {
		Color_0,
		Depth,
		Stencil,
	};
	enum Format {
		RGB,
		RGBA,
	};

	unsigned int map;
	int width;
	int height;
	Format format;
	Type type;
};

class FrameBuffer {
private:
	unsigned int fbo;
	std::vector<unsigned int> attachments;
	int width;
	int height;
public:
	unsigned int getID() const;
	void create(); 
	std::vector<Attachment> getAttachments() const;
	void Bind();
	void UnBind();
	void setAttachments(const std::vector<Attachment>& attachments);
	void setSize();
	void clear();
};

#endif // !FrameBuffer_hpp
