#ifndef PickingSolver_hpp
#define PickingSolver_hpp

#include "Core/Common.hpp"
#include <Logical/Framework/Object/GObject.hpp>

class PickingSolver {
public:
	static PickingSolver& get() {
		static PickingSolver instance;
		return instance;
	}

signals:
	Signal<std::vector<GObjectID>, std::vector<GObjectID>> pickedChanged;

public slots:
	void onPicking(float mouse_x, float mouse_y, bool retain_old = false);

private:
	PickingSolver();
};

#endif