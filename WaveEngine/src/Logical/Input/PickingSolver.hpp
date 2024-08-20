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
	const std::vector<GObjectID> getPickedIds() const { return picked_ids; }

signals:
	Signal<std::vector<GObjectID>, std::vector<GObjectID>> pickedChanged;

public slots:
	void onPicking(float mouse_x, float mouse_y);

private:
	PickingSolver();
	std::vector<GObjectID> picked_ids;
};

#endif