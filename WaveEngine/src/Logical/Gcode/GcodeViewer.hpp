#ifndef GcodeViewer_hpp
#define GcodeViewer_hpp

#include "ResourceManager/Gcode/GcodeResultData.hpp"
#include "Logical/Mesh.hpp"

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

struct Layer {
	Layer(float height_, int begin_move_id_, int end_move_id_)
		: height(height_)
		, begin_move_id(begin_move_id_)
		, end_move_id(end_move_id_)
	{}
	float height;
	int begin_move_id;
	int end_move_id;
};

struct Path {
	struct Segment {
		int begin_move_id;
		int end_move_id;
		std::shared_ptr<Mesh> mesh;
	};

	struct SubPath {
		int begin_move_id = INT_MAX;
		int end_move_id = -INT_MAX;
		std::vector<Segment> segments;

		void append_segment(const Segment& segment);
		Segment segment_of(int move_id) const;
	};

	std::vector<SubPath> sub_paths;

	std::shared_ptr<Mesh> merged_mesh;

	bool empty() const { return sub_paths.empty(); }
	void append_sub_path(const SubPath& sub_path);
	SubPath sub_path_of(int move_id) const;
	int calculate_index_offset_of(int move_id) const;
	int calculate_reverse_index_offset_of(int move_id) const;
};

struct VisualPath {
	int path_id;
	int sub_path_id;
};

class GcodeViewer {
public:
	void load(const GCodeProcessorResult& result);
	const std::vector<std::shared_ptr<Mesh>>& meshes() const;

	void set_layer_scope(std::array<int, 2> layer_scope);
	void set_move_scope(std::array<int, 2> move_scope);
	void set_visible(LineType type, bool visible);

	const std::array<int, 2>& get_layer_range() const { return m_layer_range; }
	const std::array<int, 2>& get_curr_layer_move_range() const { return m_curr_layer_move_range; }

	bool dirty() const { return m_dirty; }
	void setDirty(bool dirty) { m_dirty = dirty; }
	bool valid() const { return m_valid; }

protected:
	void reset();
	void parse_moves(std::vector<MoveVertex> moves);
	std::shared_ptr<Mesh> generate_cuboid_from_move(const MoveVertex& prev, const MoveVertex& curr);
	void refresh();

private:
	std::vector<Layer> m_layers;

	std::array<int, 2> m_move_range;
	std::array<int, 2> m_layer_range;
	std::array<int, 2> m_curr_layer_move_range;
	std::array<int, 2> m_layer_scope;
	std::array<int, 2> m_move_scope;

	bool m_move_type_visible[static_cast<size_t>(EMoveType::Count)] = { false };
	bool m_role_visible[static_cast<size_t>(ExtrusionRole::erCount)] = { true };

	unsigned int m_line_type;
	ViewType m_view_type;

	std::array<Path, ExtrusionRole::erCount> m_paths;
	std::array<std::shared_ptr<Mesh>, ExtrusionRole::erCount> m_visual_mesh;
	std::vector<std::shared_ptr<Mesh>> m_meshes;

	bool m_dirty = false;

	bool m_valid = false;
};

#endif // !#define GcodeViewer_hpp
