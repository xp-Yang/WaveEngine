#include "rhi.hpp"
#include "rhi_impl.hpp"
#include "OpenGL/rhi_opengl.hpp"

RhiTexture::RhiTexture(Format format_, const Vec2& pixelSize_, int sampleCount_, Flag flags_, unsigned char* data_)
	: m_format(format_)
	, m_pixelSize(pixelSize_)
	, m_sampleCount(sampleCount_)
	, m_flags(flags_)
	, m_data(data_)
{
}

RhiFrameBuffer::RhiFrameBuffer(const RhiAttachment& colorAttachment, const Vec2& pixelSize_, int sampleCount_)
	: m_pixelSize(pixelSize_)
	, m_sampleCount(sampleCount_)
{
	m_colorAttachments[0] = colorAttachment;
}

RhiAttachment::RhiAttachment(RhiTexture* texture)
	: m_texture(texture)
{
}


Rhi* Rhi::create()
{
	Rhi* rhi = new Rhi();
	rhi->m_impl = new RhiOpenGL();
	return rhi;
}

void Rhi::drawIndexed(unsigned int vao_id, size_t indices_count, int inst_amount)
{
	m_impl->drawIndexed(vao_id, indices_count, inst_amount);
}

void Rhi::drawTriangles(unsigned int vao_id, size_t array_count)
{
	m_impl->drawTriangles(vao_id, array_count);
}

void Rhi::setViewport()
{
	m_impl->setViewport();
}

RhiBuffer* Rhi::newBuffer(RhiBuffer::Type type, RhiBuffer::UsageFlag usage, void* data, int size)
{
	return m_impl->newBuffer(type, usage, data, size);
}

RhiVertexLayout* Rhi::newVertexLayout(RhiBuffer* vbuffer, RhiBuffer* ibuffer)
{
	return m_impl->newVertexLayout(vbuffer, ibuffer);
}

RhiTexture* Rhi::newTexture(RhiTexture::Format format, const Vec2& pixelSize, int sampleCount, RhiTexture::Flag flags, unsigned char* data)
{
	return m_impl->newTexture(format, pixelSize, sampleCount, flags, data);
}

RhiFrameBuffer* Rhi::newFrameBuffer(const RhiAttachment& colorAttachment, const Vec2& pixelSize_, int sampleCount_)
{
	return m_impl->newFrameBuffer(colorAttachment, pixelSize_, sampleCount_);
}

RhiBuffer::RhiBuffer(Type type_, UsageFlag usage_, void* data, int size_)
	: m_type(type_)
	, m_usage(usage_)
	, m_data(data)
	, m_size(size_)
{
}

RhiVertexLayout::RhiVertexLayout(RhiBuffer* vbuffer, RhiBuffer* ibuffer)
	: m_vbuffer(vbuffer)
	, m_ibuffer(ibuffer)
{
}
