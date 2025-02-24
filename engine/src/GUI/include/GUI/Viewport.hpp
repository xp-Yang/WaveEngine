#ifndef Viewport_hpp
#define Viewport_hpp

#include "Base/Common.hpp"

struct Viewport {
    Viewport() = default;
    Viewport(int _x, int _y, int _width, int _height)
        : x(_x)
        , y(_y)
        , width(_width)
        , height(_height)
    {}
    Viewport(const Viewport& _rhs)
        : x(_rhs.x)
        , y(_rhs.y)
        , width(_rhs.width)
        , height(_rhs.height)
    {}
    int x;
    int y;
    int width;
    int height;

    float AspectRatio() {
        return (float)width / (float)height;
    }
};

#endif