#include "GcodeViewer.hpp"

void LineCollection::Polyline::append_segment(const Segment& segment)
{
	if (segment.begin_move_id < begin_move_id)
		begin_move_id = segment.begin_move_id;

	if (segment.end_move_id > end_move_id)
		end_move_id = segment.end_move_id;

	segments.push_back(segment);
}

void LineCollection::append_polyline(const Polyline& polyline)
{
	polylines.push_back(polyline);
}

int LineCollection::calculate_index_offset_of(int move_id) const
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

int LineCollection::calculate_reverse_index_offset_of(int move_id) const
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

	m_line_collections = {};
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
	Vec3 first_left_dir = Math::Normalize(Math::Cross(Vec3(0, 0, 1), to_curr_dir));
	Vec3 second_left_dir = Math::Normalize(Math::Cross(Vec3(0, 0, 1), to_curr_dir));
	if (prev.type == EMoveType::Extrude) {
		Vec3 to_prev_dir = Math::Normalize(prev.position - prev_2.position);
		first_left_dir = Math::Cross(to_prev_dir, to_curr_dir).z >= 0 ? Math::Normalize(-to_prev_dir + to_curr_dir) : -Math::Normalize(-to_prev_dir + to_curr_dir);
	}
	if (next.type == EMoveType::Extrude) {
		Vec3 to_next_dir = Math::Normalize(next.position - curr.position);
		second_left_dir = Math::Cross(to_curr_dir, to_next_dir).z >= 0 ? Math::Normalize(-to_curr_dir + to_next_dir) : -Math::Normalize(-to_curr_dir + to_next_dir);
	}

	Vec3 up_dir = Vec3(0, 0, 1);
	Vec3 down_dir = -up_dir;

	Vec3 first_right_dir = -first_left_dir;
	Vec3 second_right_dir = -second_left_dir;

	std::array<Vec3, 8> vertices_positions;
	vertices_positions[0] = prev.position + (curr.width / 2.0f) * first_left_dir;
	vertices_positions[1] = prev.position + (curr.height / 2.0f) * down_dir;
	vertices_positions[2] = prev.position + (curr.width / 2.0f) * first_right_dir;
	vertices_positions[3] = prev.position + (curr.height / 2.0f) * up_dir;
	vertices_positions[4] = curr.position + (curr.height / 2.0f) * up_dir;
	vertices_positions[5] = curr.position + (curr.width / 2.0f) * second_right_dir;
	vertices_positions[6] = curr.position + (curr.height / 2.0f) * down_dir;
	vertices_positions[7] = curr.position + (curr.width / 2.0f) * second_left_dir;

	return Mesh::create_vertex_normal_cuboid_mesh(vertices_positions);
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
			LineCollection::Segment segment;
			segment.begin_move_id = move_id - 1;
			segment.end_move_id = move_id;
			segment.mesh = generate_cuboid_from_move(prev_2, prev, curr, next);

			ExtrusionRole curr_role = curr.extrusion_role;
			if (prev_role != curr_role) {
				prev_role = curr_role;
				LineCollection::Polyline polyline;
				polyline.append_segment(segment);
				m_line_collections[curr_role].append_polyline(polyline);
			}
			else {
				m_line_collections[curr_role].polylines.back().append_segment(segment);
			}
		}

		move_id++;
	}

	m_layer_range = { 0, (int)m_layers.size() - 1 };
	m_layer_scope = m_layer_range;
	m_move_range = { m_layers[m_layer_scope[1]].begin_move_id, m_layers[m_layer_scope[1]].end_move_id };
	m_move_scope = m_move_range;

	for (auto& collection : m_line_collections) {
		std::vector<std::shared_ptr<Mesh>> can_merge_meshes;
		can_merge_meshes.reserve((m_line_collections.size() * m_line_collections.front().polylines.size()));
		for (const auto& polyline : collection.polylines) {
			for (const auto& seg : polyline.segments) {
				if (seg.mesh)
					can_merge_meshes.push_back(seg.mesh);
			}
		}
		if (!can_merge_meshes.empty())
			collection.merged_mesh = Mesh::merge(can_merge_meshes);
	}

	for (int role_type = ExtrusionRole::erNone + 1; role_type < ExtrusionRole::erCount; role_type++) {
		if (!m_line_collections[role_type].empty()) {
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
		const auto& collection = m_line_collections[role_type];
		if (collection.empty() || !m_role_visible[role_type])
			continue;

		int begin_index_offset = collection.calculate_index_offset_of(begin_move_id);
		int end_index_offset = collection.calculate_reverse_index_offset_of(end_move_id);

		auto old_indices = collection.merged_mesh->indices;
		old_indices.erase(old_indices.begin(), old_indices.begin() + begin_index_offset);
		old_indices.erase(old_indices.end() - end_index_offset, old_indices.end());
		m_clipped_mesh[role_type] = std::make_shared<Mesh>(*collection.merged_mesh);
		m_clipped_mesh[role_type]->indices = old_indices;
		m_clipped_mesh[role_type]->material = std::make_shared<Material>();
		m_clipped_mesh[role_type]->material->albedo = Vec3(Extrusion_Role_Colors[role_type]);
	}

	m_dirty = true;
}
