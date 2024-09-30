#ifndef EngineAPI_hpp
#define EngineAPI_hpp

#include "Core/Common.hpp"
#include "GUI/Application.hpp"

inline Application& GetApp() {
	return Application::get();
}

#endif