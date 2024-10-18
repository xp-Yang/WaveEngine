#ifndef GcodeViewer_hpp
#define GcodeViewer_hpp

#include "ResourceManager/Gcode/GcodeResultData.hpp"

struct Layer {
	float z;
	int begin_move_id;
	int end_move_id;
};

struct Path {};

struct RenderPath {};

enum LineType : unsigned int {
	// EXTRUDE TYPE
	NONE = 0,
	INNER_WALL					= 1 << 0,
	OUTER_WALL                  = 1 << 1,
	BRIDGE_WALL					= 1 << 2,
	SPARSE_INFILL				= 1 << 3,
	INTERNAL_SOLID_INFILL		= 1 << 4,
	TOP_SURFACE			        = 1 << 5,
	BOTTOM_SURFACE				= 1 << 6,
	IRONING						= 1 << 7,
	BRIDGE						= 1 << 8,
	SKIRT						= 1 << 9,
	BRIM						= 1 << 10,
	SUPPORT						= 1 << 11,
	SUPPORT_INTERFACE			= 1 << 12,
	PRIME_TOWER					= 1 << 13,
	CUSTOM						= 1 << 14,
	MIXED						= 1 << 15,

	// MOVE TYPE
	// EXTRUDE
	TRAVEL						= 1 << 16,
	RETRACT						= 1 << 17,
	UNRETRACT					= 1 << 18,
	WIPE						= 1 << 19,
	SEAM						= 1 << 20,
	COUNT						= 1 << 21,
};

enum class ViewType {
	LINE_TYPE,
	FILAMENT,
	SPEED,
	LAYER_HEIGHT,
	LAYER_WIDTH,
	TEMPERATURE,
	COUNT
};

class GcodeViewer {
public:
	void load(const GCodeProcessorResult& result);

protected:
	void generate_vertices_from_moves(std::vector<MoveVertex> moves);

private:
	std::vector<Layer> m_layers;

	std::array<int64_t, 2> m_move_ranges;
	std::array<int64_t, 2> m_layer_ranges;
	std::array<int64_t, 2> m_curr_layer_move_ranges;
	std::array<int64_t, 2> m_layer_scope;
	std::array<int64_t, 2> m_move_scope;

	bool m_move_type_visible[static_cast<size_t>(EMoveType::Count)] = { false };
	bool m_role_visible[static_cast<size_t>(ExtrusionRole::erCount)] = { true };

	unsigned int m_line_type;
	ViewType m_view_type;
};

#endif // !#define GcodeViewer_hpp
