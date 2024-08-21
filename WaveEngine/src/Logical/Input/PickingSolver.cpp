#include "PickingSolver.hpp"
#include "Engine.hpp"
#include "Logical/Framework/Scene.hpp"
#include "Render/RHI/rhi.hpp"
// TODO remove
#include <glad/glad.h>
void PickingSolver::onPicking(float mouse_x, float mouse_y)
{
	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.

	//glFlush();
	//glFinish();
	//// TODO: 含义？
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// TODO 尝试 glGetTextureImage 
	unsigned int frame_buffer_id = GetApp().renderSystem()->getPickingFBO();
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
	//glReadBuffer(GL_COLOR_ATTACHMENT0);
	// glReadPixels()的坐标是相对于屏幕左下角的
	int x = (int)mouse_x;
	int y = (int)mouse_y;

	unsigned char data[4] = { 0,0,0,0 };
	glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
	int picked_id = (int)data[0] + (((int)data[1]) << 8) + (((int)data[2]) << 16);
	// TODO 多picked的管理
	emit pickedChanged({ picked_id }, GetApp().scene()->getPickedObjectIDs());
	//glReadBuffer(GL_NONE);
}

PickingSolver::PickingSolver()
{
	connect(this, &pickedChanged, GetApp().scene(), &Scene::onPickedChanged);
}
