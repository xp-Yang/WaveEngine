#pragma once

// ½Ó¿ÚÀà
class RHI {
public:
	virtual ~RHI() = 0;
	virtual void drawIndex() = 0;
	virtual void drawTriangles() = 0;
	virtual void createFrameBuffer() = 0;
	virtual void setViewport() = 0;

};