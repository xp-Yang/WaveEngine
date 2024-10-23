#include "GcodeViewer.hpp"

void Polyline::append_segment(const Segment& segment)
{
	if (segment.begin_move_id < begin_move_id)
		begin_move_id = segment.begin_move_id;

	if (segment.end_move_id > end_move_id)
		end_move_id = segment.end_move_id;

	segments.push_back(segment);
}

void LinesBatch::append_polyline(const Polyline& polyline)
{
	polylines.push_back(polyline);
}

int LinesBatch::calculate_index_offset_of(int move_id) const
{
	int index_offset = 0;
	int single_segment_indices_count = 36;
	for (int i = 0; i < polylines.size(); i++) {
		const auto& polyline = polylines[i];
		if (polyline.end_move_id < move_id) {
			index_offset += polyline.segments.size() * single_segment_indices_count;
			continue;
		}
		if (polyline.begin_move_id <= move_id && move_id <= polyline.end_move_id) {
			for (int j = 0; j < polyline.segments.size(); j++) {
				const auto& segment = polyline.segments[j];
				if (segment.begin_move_id <= move_id && move_id <= segment.end_move_id) {
					index_offset += j * single_segment_indices_count;
					return index_offset;
				}
			}
		}
	}
	return index_offset;
}

int LinesBatch::calculate_reverse_index_offset_of(int move_id) const
{
	int index_offset = 0;
	int single_segment_indices_count = 36;
	for (int i = 0; i < polylines.size(); i++) {
		const auto& polyline = polylines[polylines.size() - 1 - i];
		if (move_id < polyline.begin_move_id) {
			index_offset += polyline.segments.size() * single_segment_indices_count;
			continue;
		}
		if (polyline.begin_move_id <= move_id && move_id <= polyline.end_move_id) {
			for (int j = 0; j < polyline.segments.size(); j++) {
				const auto& segment = polyline.segments[polyline.segments.size() - 1 - j];
				if (segment.begin_move_id <= move_id && move_id <= segment.end_move_id) {
					index_offset += j * single_segment_indices_count;
					return index_offset;
				}
			}
		}
	}
	return index_offset;
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
	m_clipped_mesh = {};

	m_dirty = false;
	m_valid = false;
}

void GcodeViewer::load(const GCodeProcessorResult& result)
{
	reset();
	parse_moves(result.moves);
	m_valid = true;
	emit loaded(m_clipped_mesh);
}

const std::array<std::shared_ptr<Mesh>, ExtrusionRole::erCount>& GcodeViewer::meshes() const
{
	return m_clipped_mesh;
}

void GcodeViewer::set_layer_scope(std::array<int, 2> layer_scope)
{
	m_layer_scope = layer_scope;

	m_move_range[0] = m_layers[m_layer_scope[1]].begin_move_id;
	m_move_range[1] = m_layers[m_layer_scope[1]].end_move_id;
	m_move_scope = m_move_range;

	refresh();
}

void GcodeViewer::set_move_scope(std::array<int, 2> move_scope)
{
	m_move_scope[0] = std::max(move_scope[0], m_move_range[0]);
	m_move_scope[1] = std::min(move_scope[1], m_move_range[1]);

	refresh();
}

void GcodeViewer::set_visible(ExtrusionRole role_type, bool visible)
{
	m_role_visible[role_type] = visible;
	m_dirty = true;
	//update_moves_slider();
}

std::shared_ptr<Mesh> GcodeViewer::generate_cuboid_from_move(const MoveVertex& prev_2, const MoveVertex& prev, const MoveVertex& curr, const MoveVertex& next)
{
	Vec3 to_curr_dir = Math::Normalize(curr.position - prev.position);
	Vec3 to_prev_dir = Vec3(0);
	Vec3 to_next_dir = Vec3(0);
	if (prev.type == EMoveType::Extrude) {
		to_prev_dir = Math::Normalize(prev.position - prev_2.position);
	}
	if (next.type == EMoveType::Extrude) {
		to_next_dir = Math::Normalize(next.position - curr.position);
	}
	return generate_cuboid_from_move(to_prev_dir, to_curr_dir, to_next_dir, prev.position, curr.position, curr.width, curr.height);
}

std::shared_ptr<Mesh> GcodeViewer::generate_cuboid_from_move(const Vec3& to_prev_dir, const Vec3& to_curr_dir, const Vec3& to_next_dir, const Vec3& prev_pos, const Vec3& curr_pos, float move_width, float move_height)
{
	Vec3 up_dir = Vec3(0, 0, 1);
	Vec3 down_dir = -up_dir;
	Vec3 first_left_dir = Math::Normalize(Math::Cross(Vec3(0, 0, 1), to_curr_dir));
	Vec3 second_left_dir = Math::Normalize(Math::Cross(Vec3(0, 0, 1), to_curr_dir));
	if (to_prev_dir != Vec3(0))
		first_left_dir = Math::Cross(to_prev_dir, to_curr_dir).z >= 0 ? Math::Normalize(-to_prev_dir + to_curr_dir) : -Math::Normalize(-to_prev_dir + to_curr_dir);
	if (to_next_dir != Vec3(0))
		second_left_dir = Math::Cross(to_curr_dir, to_next_dir).z >= 0 ? Math::Normalize(-to_curr_dir + to_next_dir) : -Math::Normalize(-to_curr_dir + to_next_dir);
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

	return Mesh::create_vertex_normal_cuboid_mesh(vertices_positions);
}

std::vector<std::shared_ptr<Mesh>> GcodeViewer::generate_arc_from_move(const MoveVertex& prev_2, const MoveVertex& prev, const MoveVertex& curr, const MoveVertex& next)
{
	std::vector<std::shared_ptr<Mesh>> res;
	size_t loop_num = curr.is_arc_move_with_interpolation_points() ? curr.interpolation_points.size() : 0;
	for (size_t i = 0; i < loop_num + 1; i++) {
		const Vec3f& prev_pos = (i == 0 ? prev.position : curr.interpolation_points[i - 1]);
		const Vec3f& curr_pos = (i == loop_num ? curr.position : curr.interpolation_points[i]);
		Vec3 to_curr_dir = Math::Normalize(curr_pos - prev_pos);

		Vec3 to_prev_dir = Vec3(0);
		Vec3 to_next_dir = Vec3(0);
		if (i == 0) {
			if (prev.type == EMoveType::Extrude) {
				to_prev_dir = Math::Normalize(prev.position - prev_2.position);
			}
		}
		else if (i == 1)
			to_prev_dir = Math::Normalize(curr.interpolation_points[0] - prev.position);
		else
			to_prev_dir = Math::Normalize(curr.interpolation_points[i - 1] - curr.interpolation_points[i - 2]);

		if (i == loop_num) {
			if (next.type == EMoveType::Extrude) {
				to_next_dir = Math::Normalize(next.position - curr.position);
			}
		}
		else if (i == loop_num - 1)
			to_next_dir = Math::Normalize(curr.position - curr.interpolation_points[loop_num - 1]);
		else
			to_next_dir = Math::Normalize(curr.interpolation_points[i + 1] - curr.interpolation_points[i]);

		res.push_back(generate_cuboid_from_move(to_prev_dir, to_curr_dir, to_next_dir, prev_pos, curr_pos, curr.width, curr.height));
	}
	return res;
}

void GcodeViewer::parse_moves(std::vector<MoveVertex> moves)
{
	int move_id = 2;
	int point_count = 0;
	ExtrusionRole prev_role = ExtrusionRole::erNone;
	for (size_t i = 2; i < moves.size() - 1; i++) {
		const MoveVertex& prev_2 = moves[i - 2];
		const MoveVertex& prev = moves[i - 1];
		const MoveVertex& curr = moves[i];
		const MoveVertex& next = moves[i + 1];
		if (prev.position == curr.position) {
			point_count++;
			continue;
		}

		if (curr.type == EMoveType::Extrude) {
			// parse layers
			float last_height = m_layers.empty() ? -FLT_MAX : m_layers.back().height;
			float curr_height = curr.position.z;
			if (last_height < curr_height)
				m_layers.emplace_back(curr.position.z, move_id, move_id);
			else
				m_layers.back().end_move_id = move_id;

			// parse segment

			if (curr.is_arc_move_with_interpolation_points()) {
				std::vector<std::shared_ptr<Mesh>> seg_meshes = generate_arc_from_move(prev_2, prev, curr, next);
				for (int mesh_id = 0; mesh_id < seg_meshes.size(); mesh_id++) {
					Segment segment;
					segment.begin_move_id = move_id - 1;
					segment.end_move_id = move_id;
					segment.mesh = seg_meshes[mesh_id];

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
				segment.mesh = generate_cuboid_from_move(prev_2, prev, curr, next);

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

		move_id++;
	}

	m_layer_range = { 0, (int)m_layers.size() - 1 };
	m_layer_scope = m_layer_range;
	m_move_range = { m_layers[m_layer_scope[1]].begin_move_id, m_layers[m_layer_scope[1]].end_move_id };
	m_move_scope = m_move_range;

	for (auto& batch : m_lines_batches) {
		std::vector<std::shared_ptr<Mesh>> can_merge_meshes;
		can_merge_meshes.reserve((m_lines_batches.size() * m_lines_batches.front().polylines.size()));
		for (const auto& polyline : batch.polylines) {
			for (const auto& seg : polyline.segments) {
				if (seg.mesh)
					can_merge_meshes.push_back(seg.mesh);
			}
		}
		if (!can_merge_meshes.empty())
			batch.merged_mesh = Mesh::merge(can_merge_meshes);
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
	int begin_move_id = m_layers[m_layer_scope[0]].begin_move_id;
	int end_move_id = m_move_scope[1];
	end_move_id = end_move_id > begin_move_id ? end_move_id : begin_move_id + 1;

	for (int role_type = ExtrusionRole::erNone + 1; role_type < ExtrusionRole::erCount; role_type++) {
		const auto& batch = m_lines_batches[role_type];
		if (batch.empty() || !m_role_visible[role_type])
			continue;

		int begin_index_offset = batch.calculate_index_offset_of(begin_move_id);
		int end_index_offset = batch.calculate_reverse_index_offset_of(end_move_id);

		auto old_indices = batch.merged_mesh->indices;
		old_indices.erase(old_indices.begin(), old_indices.begin() + begin_index_offset);
		old_indices.erase(old_indices.end() - end_index_offset, old_indices.end());
		m_clipped_mesh[role_type] = std::make_shared<Mesh>(*batch.merged_mesh);
		m_clipped_mesh[role_type]->indices = old_indices;
		m_clipped_mesh[role_type]->material = std::make_shared<Material>();
		m_clipped_mesh[role_type]->material->albedo = Vec3(Extrusion_Role_Colors[role_type]);
	}

	m_dirty = true;
}
