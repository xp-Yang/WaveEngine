#include "GcodeViewer.hpp"

const std::vector<Color4> Extrusion_Role_Colors{
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

static int ColorToInt(Color4 color) {
	int r = ((int)(color.x * 255)) << 24;
	int g = ((int)(color.y * 255)) << 16;
	int b = ((int)(color.z * 255)) << 8;
	int a = ((int)(color.w * 255)) << 0;
	return r + g + b + a;
}

static Color4 IntToColor(int color) {
	float r = ((color >> 24) & 0x000000FF) / 255.0f;
	float g = ((color >> 16) & 0x000000FF) / 255.0f;
	float b = ((color >> 8) & 0x000000FF) / 255.0f;
	float a = ((color >> 0) & 0x000000FF) / 255.0f;
	return Color4(r, g, b, a);
}

void GcodeViewer::load(const GCodeProcessorResult& result)
{
	parse_moves(result.moves);
}

void GcodeViewer::parse_moves(std::vector<MoveVertex> moves)
{
	size_t move_id = 1;
	int point_count = 0;
	for (size_t i = 1; i < moves.size() - 1; i++) {
		const MoveVertex& prev = moves[i - 1];
		const MoveVertex& curr = moves[i];
		const MoveVertex& next = moves[i + 1];
		if (prev.position == curr.position) {
			point_count++;
			continue;
		}

		EMoveType move_type = curr.type;
		// parse layers
		if (move_type == EMoveType::Extrude) {
			float last_height = m_layers.empty() ? -FLT_MAX : m_layers.back().height;
			float curr_height = curr.position.z;
			if (last_height < curr_height)
				m_layers.emplace_back(curr.position.z, move_id, move_id);
			else
				m_layers.back().end_move_id = move_id;

			ExtrusionRole last_role = m_segments.empty() ? ExtrusionRole::erNone : m_segments.back().role_type;
			ExtrusionRole curr_role = curr.extrusion_role;
			if (last_role != curr_role)
			{
				auto mesh = generate_cuboid_from_move(prev, curr);
				Segment seg;
				seg.sub_meshes.push_back(mesh);
				seg.begin_move_id = move_id;
				seg.end_move_id = move_id;
				seg.role_type = curr.extrusion_role;
				m_segments.push_back(seg);
			}
			else {
				m_segments.back().end_move_id = move_id;
				auto mesh = generate_cuboid_from_move(prev, curr);
				m_segments.back().sub_meshes.push_back(mesh);
			}
		}

		move_id++;
	}

	for (auto& seg : m_segments) {
		seg.merged_mesh = Mesh::merge(seg.sub_meshes);
		seg.merged_mesh->material = Material::create_default_material();
		seg.merged_mesh->material->albedo = Vec3(Extrusion_Role_Colors[seg.role_type]);
		m_meshes.push_back(seg.merged_mesh);
	}

	m_layer_ranges = { 0, static_cast<unsigned int>(m_layers.size() - 1) };
	m_move_ranges[0] = 0;
	m_move_ranges[1] = move_id;
}

std::shared_ptr<Mesh> GcodeViewer::generate_cuboid_from_move(const MoveVertex& prev, const MoveVertex& curr)
{
	Vec3 dir = Math::Normalize(curr.position - prev.position);
	Vec3 left_dir = Math::Cross(Vec3(0, 1, 0), dir);
	Vec3 right_dir = -left_dir;
	Vec3 up_dir = Math::Cross(dir, left_dir);
	Vec3 down_dir = -up_dir;

	std::array<Vec3, 8> vertices_positions;
	vertices_positions[0] = prev.position + (prev.width / 2.0f) * left_dir;
	vertices_positions[1] = prev.position + (prev.height / 2.0f) * down_dir;
	vertices_positions[2] = prev.position + (prev.width / 2.0f) * right_dir;
	vertices_positions[3] = prev.position + (prev.height / 2.0f) * up_dir;
	vertices_positions[4] = curr.position + (prev.height / 2.0f) * up_dir;
	vertices_positions[5] = curr.position + (prev.width / 2.0f) * right_dir;
	vertices_positions[6] = curr.position + (prev.height / 2.0f) * down_dir;
	vertices_positions[7] = curr.position + (prev.width / 2.0f) * left_dir;

	return Mesh::create_cuboid_mesh(vertices_positions);
}
