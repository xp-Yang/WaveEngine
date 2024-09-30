#include "PickingSolver.hpp"
#include "EngineAPI.hpp"
#include "Logical/Framework/Scene.hpp"
#include "Render/RHI/rhi.hpp"
// TODO remove
#include <glad/glad.h>

void PickingSolver::onPicking(float mouse_x, float mouse_y, bool retain_old)
{
	int x = (int)mouse_x;
	int y = (int)mouse_y;
	// map to picking framebuffer size
	// picking framebuffer is {DEFAULT_RENDER_RESOLUTION_X, DEFAULT_RENDER_RESOLUTION_Y} size
	auto main_viewport = GetApp().editor()->getMainViewport();
	x *= DEFAULT_RENDER_RESOLUTION_X / (float)main_viewport.width;
	y *= DEFAULT_RENDER_RESOLUTION_Y / (float)main_viewport.height;
	// in gl coordinate system, left-bottom is as origin
	y = DEFAULT_RENDER_RESOLUTION_Y - y;

	unsigned char data[4] = { 0,0,0,0 };
	unsigned int frame_buffer_id = GetApp().renderSystem()->getPickingFBO();
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
	int picked_id = (int)data[0] + (((int)data[1]) << 8) + (((int)data[2]) << 16);
	emit pickedChanged({ picked_id }, retain_old ? std::vector<GObjectID>() : GetApp().scene()->getPickedObjectIDs());

	Logger::debug("PickingSolver::onPicking(), picking({}, {}), mouse({}, {}), picked_id:{}", x, y, mouse_x, mouse_y, picked_id);
}

PickingSolver::PickingSolver()
{
	connect(this, &pickedChanged, GetApp().scene(), &Scene::onPickedChanged);
}
