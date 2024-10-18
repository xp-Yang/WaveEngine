#include "GcodeViewer.hpp"

void GcodeViewer::load(const GCodeProcessorResult& result)
{
	generate_vertices_from_moves(result.moves);
}

void GcodeViewer::generate_vertices_from_moves(std::vector<MoveVertex> moves)
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
			float last_z = m_layers.empty() ? -FLT_MAX : m_layers.back().z;
			float curr_z = curr.position.z;
			if (last_z < curr_z)
				m_layers.emplace_back(curr.position.z, /*curr.gcode_id*/move_id, /*curr.gcode_id*/move_id);
			else
				m_layers.back().end_move_id = /*curr.gcode_id*/move_id;
		}

		move_id++;
	}

	m_layer_ranges = { 0, static_cast<unsigned int>(m_layers.size() - 1) };
	m_move_ranges[0] = 0;
	m_move_ranges[1] = move_id;
}
