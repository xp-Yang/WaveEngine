#ifndef Engine_hpp
#define Engine_hpp

#include "Core/Common.hpp"
#include "GUI/Application.hpp"
//#include "Render/RHI/rhi.hpp"

static inline constexpr float DEFAULT_RENDER_RESOLUTION_X = 1920.0f;
static inline constexpr float DEFAULT_RENDER_RESOLUTION_Y = 1080.0f;
static inline constexpr float DEFAULT_WINDOW_WIDTH = 1920.0f;
static inline constexpr float DEFAULT_WINDOW_HEIGHT = 1080.0f;

static inline const std::string RESOURCE_DIRECTORY = RESOURCE_DIR;
static inline const std::string ASSET_DIRECTORY = ASSET_DIR;

inline Application& GetApp() {
	return Application::get();
}

#endif