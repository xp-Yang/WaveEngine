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

inline const std::vector<Color4> Extrusion_Role_Colors{
	{ 0.90f, 0.70f, 0.70f, 1.0f },   // erNone
	{ 1.00f, 0.90f, 0.30f, 1.0f },   // erPerimeter
	{ 1.00f, 0.49f, 0.22f, 1.0f },   // erExternalPerimeter
	{ 0.12f, 0.12f, 1.00f, 1.0f },   // erOverhangPerimeter
	{ 0.69f, 0.19f, 0.16f, 1.0f },   // erInternalInfill
	{ 0.59f, 0.33f, 0.80f, 1.0f },   // erSolidInfill
	{ 0.94f, 0.25f, 0.25f, 1.0f },   // erTopSolidInfill
	{ 0.40f, 0.36f, 0.78f, 1.0f },   // erBottomSurface
	{ 1.00f, 0.55f, 0.41f, 1.0f },   // erIroning
	{ 0.30f, 0.50f, 0.73f, 1.0f },   // erBridgeInfill
	{ 1.00f, 1.00f, 1.00f, 1.0f },   // erGapFill
	{ 0.00f, 0.53f, 0.43f, 1.0f },   // erSkirt
	{ 0.00f, 0.23f, 0.43f, 1.0f },   // erBrim
	{ 0.00f, 1.00f, 0.00f, 1.0f },   // erSupportMaterial
	{ 0.00f, 0.50f, 0.00f, 1.0f },   // erSupportMaterialInterface
	{ 0.00f, 0.25f, 0.00f, 1.0f },   // erSupportTransition
	{ 0.70f, 0.89f, 0.67f, 1.0f },   // erWipeTower
	{ 0.37f, 0.82f, 0.58f, 1.0f },   // erCustom
	{ 0.37f, 0.82f, 0.58f, 1.0f },   // erMixed
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

struct Segment {
	int begin_move_id;
	int end_move_id;
	std::shared_ptr<Mesh> mesh;
};

struct Polyline {
	int begin_move_id = INT_MAX;
	int end_move_id = -INT_MAX;
	std::vector<Segment> segments;

	void append_segment(const Segment& segment);
};

struct LineCollection {

	std::vector<Polyline> polylines;

	std::shared_ptr<Mesh> merged_mesh;

	bool empty() const { return polylines.empty() || (merged_mesh.get() == nullptr); }
	void append_polyline(const Polyline& polyline);
	int calculate_index_offset_of(int move_id) const;
	int calculate_reverse_index_offset_of(int move_id) const;
};

class GcodeViewer {
public:
	GcodeViewer();
	GcodeViewer(const GcodeViewer&) = delete;
	GcodeViewer(GcodeViewer&&) = delete;
	GcodeViewer& operator=(const GcodeViewer&) = delete;
	GcodeViewer& operator=(GcodeViewer&&) = delete;

	void load(const GCodeProcessorResult& result);
	const std::array<std::shared_ptr<Mesh>, ExtrusionRole::erCount>& meshes() const;

	void set_layer_scope(std::array<int, 2> layer_scope);
	void set_move_scope(std::array<int, 2> move_scope);

	const std::array<int, 2>& get_layer_range() const { return m_layer_range; }
	const std::array<int, 2>& get_move_range() const { return m_move_range; }

	void set_visible(ExtrusionRole role_type, bool visible);
	bool is_visible(ExtrusionRole role_type) const { return m_role_visible[role_type]; }

	bool dirty() const { return m_dirty; }
	void setDirty(bool dirty) { m_dirty = dirty; }

	bool valid() const { return m_valid; }

signals:
	Signal<std::array<std::shared_ptr<Mesh>, ExtrusionRole::erCount>> loaded;

protected:
	void reset();
	void parse_moves(std::vector<MoveVertex> moves);
	std::shared_ptr<Mesh> generate_cuboid_from_move(const MoveVertex& prev_2, const MoveVertex& prev, const MoveVertex& curr, const MoveVertex& next);
	std::shared_ptr<Mesh> generate_cuboid_from_move(const Vec3& to_prev_dir, const Vec3& to_curr_dir, const Vec3& to_next_dir, const Vec3& prev_pos, const Vec3& curr_pos, float move_width, float move_height);
	std::vector<std::shared_ptr<Mesh>> generate_arc_from_move(const MoveVertex& prev_2, const MoveVertex& prev, const MoveVertex& curr, const MoveVertex& next);
	void refresh();

private:
	std::vector<Layer> m_layers;

	std::array<int, 2> m_layer_range;
	std::array<int, 2> m_layer_scope;
	std::array<int, 2> m_move_range;
	std::array<int, 2> m_move_scope;

	std::array<bool, EMoveType::Count> m_move_type_visible = {};
	std::array<bool, ExtrusionRole::erCount> m_role_visible = {};
	ViewType m_view_type;

	std::array<LineCollection, ExtrusionRole::erCount> m_line_collections = {};
	std::array<std::shared_ptr<Mesh>, ExtrusionRole::erCount> m_clipped_mesh = {};

	bool m_dirty = false;

	bool m_valid = false;
};

#endif // !#define GcodeViewer_hpp
