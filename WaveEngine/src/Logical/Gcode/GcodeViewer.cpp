#include "GcodeViewer.hpp"

void Polyline::append_segment(const Segment& segment)
{
	if (segment.begin_move_id < begin_move_id)
		begin_move_id = segment.begin_move_id;

	if (segment.end_move_id > end_move_id)
		end_move_id = segment.end_move_id;

	if (!segments.empty()) {
		Segment corner_segment;
		const auto& last_segment = segments.back();
		corner_segment.begin_move_id = last_segment.end_move_id;
		corner_segment.end_move_id = segment.begin_move_id;
		assert(corner_segment.begin_move_id == corner_segment.end_move_id);

		//std::array<Vec3, 8> vertices_positions;
		//vertices_positions[0] = last_segment.mesh->vertices[7].position;
		//vertices_positions[1] = last_segment.mesh->vertices[6].position;
		//vertices_positions[2] = last_segment.mesh->vertices[5].position;
		//vertices_positions[3] = last_segment.mesh->vertices[4].position;
		//vertices_positions[4] = segment.mesh->vertices[3].position;
		//vertices_positions[5] = segment.mesh->vertices[2].position;
		//vertices_positions[6] = segment.mesh->vertices[1].position;
		//vertices_positions[7] = segment.mesh->vertices[0].position;
		//corner_segment.mesh = Mesh::create_vertex_normal_cuboid_mesh(vertices_positions);

		std::vector<int> indices = {
			-1, -2, -3, -1, -3, -4,//前
			3, 2, 1, 3, 1, 0,//后
			0, 1, -2, 0, -2, -1,//左
			-4, -3, 2, -4, 2, 3,//右
			-2, 1, 2, -2, 2, -3,//下
			0, -1, -4, 0, -4, 3,//上
		};
		corner_segment.mesh = std::make_shared<SimpleMesh>(std::vector<Vertex>(), indices);
		segments.push_back(corner_segment);
	}

	segments.push_back(segment);
}

void LinesBatch::append_polyline(const Polyline& polyline)
{
	polylines.push_back(polyline);
}

int LinesBatch::calculate_index_offset_of(int move_id) const
{
	// debug
	//int single_segment_indices_count = 36;
	//int total_indices_count = 0;
	//for (int i = 0; i < polylines.size(); i++) {
	//	const auto& polyline = polylines[i];
	//	for (int j = 0; j < polyline.segments.size(); j++) {
	//		total_indices_count += single_segment_indices_count;
	//	}
	//}

	int single_segment_indices_count = 36;

	int offset = 0;
	for (int i = 0; i < polylines.size(); i++) {
		const auto& polyline = polylines[i];
		if (polyline.end_move_id <= move_id) {
			offset += polyline.segments.size();
			continue;
		}
		if (polyline.begin_move_id < move_id && move_id <= polyline.end_move_id) {
			for (int j = 0; j < polyline.segments.size(); j++) {
				const auto& segment = polyline.segments[j];
				if (move_id == segment.end_move_id) {
					offset += (j + 1);
					return offset * single_segment_indices_count;
				}
			}
		}
	}
	return offset * single_segment_indices_count;
}

GcodeViewer::GcodeViewer()
{
	reset();
}

void GcodeViewer::reset()
{
	m_move_type_visible.fill(false);
	m_role_visible.fill(false);
	m_view_type = ViewType::LINE_TYPE;

	m_layers.clear();
	m_layer_range = {};
	m_layer_scope = {};
	m_move_range = {};
	m_move_scope = {};

	m_lines_batches = {};
	m_clipped_indices = {};

	m_dirty = false;
	m_valid = false;
}

void GcodeViewer::load(const GCodeProcessorResult& result)
{
	reset();
	parse_moves(result.moves);
	m_valid = true;
	emit loaded(m_lines_batches);
}

void GcodeViewer::set_layer_scope(std::array<int, 2> layer_scope)
{
	assert(layer_scope[1] >= layer_scope[0]);
	m_layer_scope = layer_scope;

	m_move_range[0] = m_layers[m_layer_scope[1]].begin_move_id;
	m_move_range[1] = m_layers[m_layer_scope[1]].end_move_id;
	m_move_scope = m_move_range;

	refresh();
}

void GcodeViewer::set_move_scope(std::array<int, 2> move_scope)
{
	assert(move_scope[1] >= move_scope[0]);
	m_move_scope[0] = std::max(move_scope[0], m_move_range[0]);
	m_move_scope[1] = std::min(move_scope[1], m_move_range[1]);

	refresh();
}

void GcodeViewer::set_visible(ExtrusionRole role_type, bool visible)
{
	m_role_visible[role_type] = visible;
	clipping_indices();
	m_dirty = true;
	//update the horizontal slider
}

std::shared_ptr<SimpleMesh> GcodeViewer::generate_cuboid_from_move(const MoveVertex& prev, const MoveVertex& curr)
{
	Vec3 to_curr_dir = Math::Normalize(curr.position - prev.position);
	return generate_cuboid_from_move(to_curr_dir, prev.position, curr.position, curr.width, curr.height);
}

std::shared_ptr<SimpleMesh> GcodeViewer::generate_cuboid_from_move(const Vec3& to_curr_dir, const Vec3& prev_pos, const Vec3& curr_pos, float move_width, float move_height)
{
	Vec3 up_dir = Vec3(0, 0, 1);
	Vec3 down_dir = -up_dir;
	Vec3 first_left_dir = Math::Normalize(Math::Cross(Vec3(0, 0, 1), to_curr_dir));
	Vec3 second_left_dir = Math::Normalize(Math::Cross(Vec3(0, 0, 1), to_curr_dir));
	Vec3 first_right_dir = -first_left_dir;
	Vec3 second_right_dir = -second_left_dir;

	Vec3 orthogonal_left_dir = Math::Normalize(Math::Cross(up_dir, to_curr_dir));

	float half_width = move_width / 2.0f;
	float half_height = move_height / 2.0f;
	float first_width = half_width / (Math::Dot(first_left_dir, orthogonal_left_dir) / (Math::Length(first_left_dir) * Math::Length(orthogonal_left_dir)));
	float second_width = half_width / (Math::Dot(second_left_dir, orthogonal_left_dir) / (Math::Length(second_left_dir) * Math::Length(orthogonal_left_dir)));

	std::array<Vec3, 8> vertices_positions;
	vertices_positions[0] = prev_pos + first_width * first_left_dir;
	vertices_positions[1] = prev_pos + half_height * down_dir;
	vertices_positions[2] = prev_pos + first_width * first_right_dir;
	vertices_positions[3] = prev_pos + half_height * up_dir;
	vertices_positions[4] = curr_pos + half_height * up_dir;
	vertices_positions[5] = curr_pos + second_width * second_right_dir;
	vertices_positions[6] = curr_pos + half_height * down_dir;
	vertices_positions[7] = curr_pos + second_width * second_left_dir;

	return SimpleMesh::create_vertex_normal_cuboid_mesh(vertices_positions);
}

std::vector<std::shared_ptr<SimpleMesh>> GcodeViewer::generate_arc_from_move(const MoveVertex& prev, const MoveVertex& curr)
{
	std::vector<std::shared_ptr<SimpleMesh>> res;
	size_t loop_num = curr.is_arc_move_with_interpolation_points() ? curr.interpolation_points.size() : 0;
	for (size_t i = 0; i < loop_num + 1; i++) {
		const Vec3f& prev_pos = (i == 0 ? prev.position : curr.interpolation_points[i - 1]);
		const Vec3f& curr_pos = (i == loop_num ? curr.position : curr.interpolation_points[i]);
		Vec3 to_curr_dir = Math::Normalize(curr_pos - prev_pos);
		res.push_back(generate_cuboid_from_move(to_curr_dir, prev_pos, curr_pos, curr.width, curr.height));
	}
	return res;
}

void GcodeViewer::parse_moves(std::vector<MoveVertex> moves)
{
	int point_count = 0;
	ExtrusionRole prev_role = ExtrusionRole::erNone;
	for (size_t move_id = 1; move_id < moves.size(); move_id++) {
		const MoveVertex& prev = moves[move_id - 1];
		const MoveVertex& curr = moves[move_id];

		if (curr.type != EMoveType::Extrude) {
			prev_role = ExtrusionRole::erNone;
		}

		if (prev.position == curr.position) {
			// seam or spit or ...
			// skip segment generation for these moves
			point_count++;
			continue;
		}

		if (curr.type == EMoveType::Extrude) {
			// parse layers
			float last_height = m_layers.empty() ? -FLT_MAX : m_layers.back().height;
			float curr_height = curr.position.z;
			if (last_height < curr_height)
				m_layers.emplace_back(curr.position.z, move_id - 1, move_id);
			else
				m_layers.back().end_move_id = move_id;

			// parse segment
			if (curr.is_arc_move_with_interpolation_points()) {
				std::vector<std::shared_ptr<SimpleMesh>> seg_meshes = generate_arc_from_move(prev, curr);
				for (int i = 0; i < seg_meshes.size(); i++) {
					Segment segment;
					segment.begin_move_id = move_id - 1;
					segment.end_move_id = move_id - 1;
					if (i == seg_meshes.size() - 1)
						segment.end_move_id = move_id;
					segment.mesh = seg_meshes[i];

					ExtrusionRole curr_role = curr.extrusion_role;
					if (prev_role != curr_role) {
						prev_role = curr_role;
						Polyline polyline;
						polyline.append_segment(segment);
						m_lines_batches[curr_role].append_polyline(polyline);
					}
					else {
						m_lines_batches[curr_role].polylines.back().append_segment(segment);
					}
				}
			}
			else {
				Segment segment;
				segment.begin_move_id = move_id - 1;
				segment.end_move_id = move_id;
				segment.mesh = generate_cuboid_from_move(prev, curr);

				ExtrusionRole curr_role = curr.extrusion_role;
				if (prev_role != curr_role) {
					prev_role = curr_role;
					Polyline polyline;
					polyline.append_segment(segment);
					m_lines_batches[curr_role].append_polyline(polyline);
				}
				else {
					m_lines_batches[curr_role].polylines.back().append_segment(segment);
				}
			}
		}
	}

	m_layer_range = { 0, (int)m_layers.size() - 1 };
	m_layer_scope = m_layer_range;
	m_move_range = { m_layers[m_layer_scope[1]].begin_move_id, m_layers[m_layer_scope[1]].end_move_id };
	m_move_scope = m_move_range;

	for (auto& batch : m_lines_batches) {
		std::vector<std::shared_ptr<SimpleMesh>> can_merge_meshes;
		can_merge_meshes.reserve((m_lines_batches.size() * m_lines_batches.front().polylines.size()));
		for (auto& polyline : batch.polylines) {
			for (auto& seg : polyline.segments) {
				if (seg.mesh)
					can_merge_meshes.push_back(std::move(seg.mesh));
			}
		}
		if (!can_merge_meshes.empty()) {
			batch.merged_mesh = SimpleMesh::merge(can_merge_meshes);
		}
	}

	for (int role_type = ExtrusionRole::erNone + 1; role_type < ExtrusionRole::erCount; role_type++) {
		if (!m_lines_batches[role_type].empty()) {
			m_role_visible[role_type] = true;
		}
	}

	refresh();
}

void GcodeViewer::refresh()
{
	clipping_indices();
	coloring();

	m_dirty = true;
}

void GcodeViewer::clipping_indices()
{
	m_clipped_indices = {};
	int begin_move_id = m_layers[m_layer_scope[0]].begin_move_id;
	int end_move_id = m_move_scope[1];
	assert(begin_move_id <= end_move_id);

	for (int role_type = ExtrusionRole::erNone + 1; role_type < ExtrusionRole::erCount; role_type++) {
		const auto& batch = m_lines_batches[role_type];
		if (batch.empty() || !m_role_visible[role_type])
			continue;

		int begin_index_offset = batch.calculate_index_offset_of(begin_move_id);
		int end_index_offset = batch.calculate_index_offset_of(end_move_id);

		m_clipped_indices[role_type] = std::make_pair(begin_index_offset, end_index_offset);
	}
}

void GcodeViewer::coloring()
{
	//m_visual_meshes.clear();
	//int end_move_id = m_move_scope[1];
	//if (end_move_id == m_layers[m_layer_scope[1]].end_move_id) {
	//	for (int role_type = ExtrusionRole::erNone + 1; role_type < ExtrusionRole::erCount; role_type++) {
	//		if (!m_clipped_mesh[role_type] || m_clipped_mesh[role_type]->indices.empty())
	//			continue;
	//		std::shared_ptr<Material> material = std::make_shared<Material>();
	//		material->albedo = Vec3(Extrusion_Role_Colors[role_type]);
	//		m_visual_meshes.push_back(std::make_shared<Mesh>(m_clipped_mesh[role_type], material));
	//	}
	//}
	//else {
	//	for (int role_type = ExtrusionRole::erNone + 1; role_type < ExtrusionRole::erCount; role_type++) {
	//		if (!m_clipped_mesh[role_type] || m_clipped_mesh[role_type]->indices.empty())
	//			continue;
	//		std::shared_ptr<Material> material = std::make_shared<Material>();
	//		material->albedo = Vec3(Silent_Color);
	//		m_visual_meshes.push_back(std::make_shared<Mesh>(m_clipped_mesh[role_type], material));
	//	}
	//}
}
