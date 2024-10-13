#include "GcodeData.hpp"
#include "GcodeImporter.hpp"

void CachedPosition::reset()
{
    std::fill(position.begin(), position.end(), FLT_MAX);
    feedrate = FLT_MAX;
}

void CpColor::reset()
{
    counter = 0;
    current = 0;
}

void UsedFilaments::reset()
{
    color_change_cache = 0.0f;
    volumes_per_color_change = std::vector<double>();

    tool_change_cache = 0.0f;
    volumes_per_extruder.clear();

    flush_per_filament.clear();

    role_cache = 0.0f;
    filaments_per_role.clear();
}

void UsedFilaments::increase_caches(double extruded_volume)
{
    color_change_cache += extruded_volume;
    tool_change_cache += extruded_volume;
    role_cache += extruded_volume;
}

void UsedFilaments::process_color_change_cache()
{
    if (color_change_cache != 0.0f) {
        volumes_per_color_change.push_back(color_change_cache);
        color_change_cache = 0.0f;
    }
}

void UsedFilaments::process_extruder_cache(GCodeProcessor* processor)
{
    size_t active_extruder_id = processor->m_extruder_id;
    if (tool_change_cache != 0.0f) {
        if (volumes_per_extruder.find(active_extruder_id) != volumes_per_extruder.end())
            volumes_per_extruder[active_extruder_id] += tool_change_cache;
        else
            volumes_per_extruder[active_extruder_id] = tool_change_cache;
        tool_change_cache = 0.0f;
    }
}

void UsedFilaments::update_flush_per_filament(size_t extrude_id, float flush_volume)
{
    if (flush_per_filament.find(extrude_id) != flush_per_filament.end())
        flush_per_filament[extrude_id] += flush_volume;
    else
        flush_per_filament[extrude_id] = flush_volume;
}

void UsedFilaments::process_role_cache(GCodeProcessor* processor)
{
    if (role_cache != 0.0f) {
        std::pair<double, double> filament = { 0.0f, 0.0f };

        double s = Math::Constant::PI * sqr(0.5 * processor->m_result.filament_diameters[processor->m_extruder_id]);
        filament.first = role_cache / s * 0.001;
        filament.second = role_cache * processor->m_result.filament_densities[processor->m_extruder_id] * 0.001;

        ExtrusionRole active_role = processor->m_extrusion_role;
        if (filaments_per_role.find(active_role) != filaments_per_role.end()) {
            filaments_per_role[active_role].first += filament.first;
            filaments_per_role[active_role].second += filament.second;
        }
        else
            filaments_per_role[active_role] = filament;
        role_cache = 0.0f;
    }
}

void UsedFilaments::process_caches(GCodeProcessor* processor)
{
    process_color_change_cache();
    process_extruder_cache(processor);
    process_role_cache(processor);
}

float Trapezoid::acceleration_time(float entry_feedrate, float acceleration) const
{
    return acceleration_time_from_distance(entry_feedrate, accelerate_until, acceleration);
}

float Trapezoid::cruise_time() const
{
    return (cruise_feedrate != 0.0f) ? cruise_distance() / cruise_feedrate : 0.0f;
}

float Trapezoid::deceleration_time(float distance, float acceleration) const
{
    return acceleration_time_from_distance(cruise_feedrate, (distance - decelerate_after), -acceleration);
}

float Trapezoid::cruise_distance() const
{
    return decelerate_after - accelerate_until;
}

void TimeBlock::calculate_trapezoid()
{
    trapezoid.cruise_feedrate = feedrate_profile.cruise;

    float accelerate_distance = std::max(0.0f, estimated_acceleration_distance(feedrate_profile.entry, feedrate_profile.cruise, acceleration));
    float decelerate_distance = std::max(0.0f, estimated_acceleration_distance(feedrate_profile.cruise, feedrate_profile.exit, -acceleration));
    float cruise_distance = distance - accelerate_distance - decelerate_distance;

    // Not enough space to reach the nominal feedrate.
    // This means no cruising, and we'll have to use intersection_distance() to calculate when to abort acceleration
    // and start braking in order to reach the exit_feedrate exactly at the end of this block.
    if (cruise_distance < 0.0f) {
        accelerate_distance = std::clamp(intersection_distance(feedrate_profile.entry, feedrate_profile.exit, acceleration, distance), 0.0f, distance);
        cruise_distance = 0.0f;
        trapezoid.cruise_feedrate = speed_from_distance(feedrate_profile.entry, accelerate_distance, acceleration);
    }

    trapezoid.accelerate_until = accelerate_distance;
    trapezoid.decelerate_after = accelerate_distance + cruise_distance;
}

float TimeBlock::time() const
{
    return trapezoid.acceleration_time(feedrate_profile.entry, acceleration)
        + trapezoid.cruise_time()
        + trapezoid.deceleration_time(distance, acceleration);
}

void TimeMachine::State::reset()
{
    feedrate = 0.0f;
    safe_feedrate = 0.0f;
    axis_feedrate = { 0.0f, 0.0f, 0.0f, 0.0f };
    abs_axis_feedrate = { 0.0f, 0.0f, 0.0f, 0.0f };
    //BBS
    enter_direction = { 0.0f, 0.0f, 0.0f };
    exit_direction = { 0.0f, 0.0f, 0.0f };
}

void TimeMachine::CustomGCodeTime::reset()
{
    needed = false;
    cache = 0.0f;
    times = std::vector<std::pair<CustomGCode::Type, float>>();
}

void TimeMachine::reset()
{
    enabled = false;
    acceleration = 0.0f;
    max_acceleration = 0.0f;
    retract_acceleration = 0.0f;
    max_retract_acceleration = 0.0f;
    travel_acceleration = 0.0f;
    max_travel_acceleration = 0.0f;
    extrude_factor_override_percentage = 1.0f;
    time = 0.0f;
    stop_times = std::vector<StopTime>();
    curr.reset();
    prev.reset();
    gcode_time.reset();
    blocks = std::vector<TimeBlock>();
    g1_times_cache = std::vector<G1LinesCacheItem>();
    std::fill(moves_time.begin(), moves_time.end(), 0.0f);
    std::fill(roles_time.begin(), roles_time.end(), 0.0f);
    layers_time = std::vector<float>();
    prepare_time = 0.0f;
}

void TimeMachine::simulate_st_synchronize(float additional_time)
{
    if (!enabled)
        return;

    calculate_time(0, additional_time);
}

static void planner_forward_pass_kernel(TimeBlock& prev, TimeBlock& curr)
{
    // If the previous block is an acceleration block, but it is not long enough to complete the
    // full speed change within the block, we need to adjust the entry speed accordingly. Entry
    // speeds have already been reset, maximized, and reverse planned by reverse planner.
    // If nominal length is true, max junction speed is guaranteed to be reached. No need to recheck.
    if (!prev.flags.nominal_length) {
        if (prev.feedrate_profile.entry < curr.feedrate_profile.entry) {
            float entry_speed = std::min(curr.feedrate_profile.entry, max_allowable_speed(-prev.acceleration, prev.feedrate_profile.entry, prev.distance));

            // Check for junction speed change
            if (curr.feedrate_profile.entry != entry_speed) {
                curr.feedrate_profile.entry = entry_speed;
                curr.flags.recalculate = true;
            }
        }
    }
}

void planner_reverse_pass_kernel(TimeBlock& curr, TimeBlock& next)
{
    // If entry speed is already at the maximum entry speed, no need to recheck. Block is cruising.
    // If not, block in state of acceleration or deceleration. Reset entry speed to maximum and
    // check for maximum allowable speed reductions to ensure maximum possible planned speed.
    if (curr.feedrate_profile.entry != curr.max_entry_speed) {
        // If nominal length true, max junction speed is guaranteed to be reached. Only compute
        // for max allowable speed if block is decelerating and nominal length is false.
        if (!curr.flags.nominal_length && curr.max_entry_speed > next.feedrate_profile.entry)
            curr.feedrate_profile.entry = std::min(curr.max_entry_speed, max_allowable_speed(-curr.acceleration, next.feedrate_profile.entry, curr.distance));
        else
            curr.feedrate_profile.entry = curr.max_entry_speed;

        curr.flags.recalculate = true;
    }
}

static void recalculate_trapezoids(std::vector<TimeBlock>& blocks)
{
    TimeBlock* curr = nullptr;
    TimeBlock* next = nullptr;

    for (size_t i = 0; i < blocks.size(); ++i) {
        TimeBlock& b = blocks[i];

        curr = next;
        next = &b;

        if (curr != nullptr) {
            // Recalculate if current block entry or exit junction speed has changed.
            if (curr->flags.recalculate || next->flags.recalculate) {
                // NOTE: Entry and exit factors always > 0 by all previous logic operations.
                TimeBlock block = *curr;
                block.feedrate_profile.exit = next->feedrate_profile.entry;
                block.calculate_trapezoid();
                curr->trapezoid = block.trapezoid;
                curr->flags.recalculate = false; // Reset current only to ensure next trapezoid is computed
            }
        }
    }

    // Last/newest block in buffer. Always recalculated.
    if (next != nullptr) {
        TimeBlock block = *next;
        block.feedrate_profile.exit = next->safe_feedrate;
        block.calculate_trapezoid();
        next->trapezoid = block.trapezoid;
        next->flags.recalculate = false;
    }
}

void TimeMachine::calculate_time(size_t keep_last_n_blocks, float additional_time)
{
    if (!enabled || blocks.size() < 2)
        return;

    assert(keep_last_n_blocks <= blocks.size());

    // forward_pass
    for (size_t i = 0; i + 1 < blocks.size(); ++i) {
        planner_forward_pass_kernel(blocks[i], blocks[i + 1]);
    }

    // reverse_pass
    for (int i = static_cast<int>(blocks.size()) - 1; i > 0; --i)
        planner_reverse_pass_kernel(blocks[i - 1], blocks[i]);

    recalculate_trapezoids(blocks);

    size_t n_blocks_process = blocks.size() - keep_last_n_blocks;
    for (size_t i = 0; i < n_blocks_process; ++i) {
        const TimeBlock& block = blocks[i];
        float block_time = block.time();
        if (i == 0)
            block_time += additional_time;

        time += block_time;
        gcode_time.cache += block_time;
        //BBS: don't calculate travel of start gcode into travel time
        if (!block.flags.prepare_stage || block.move_type != EMoveType::Travel)
            moves_time[static_cast<size_t>(block.move_type)] += block_time;
        roles_time[static_cast<size_t>(block.role)] += block_time;
        if (block.layer_id >= layers_time.size()) {
            const size_t curr_size = layers_time.size();
            layers_time.resize(block.layer_id);
            for (size_t i = curr_size; i < layers_time.size(); ++i) {
                layers_time[i] = 0.0f;
            }
        }
        layers_time[block.layer_id - 1] += block_time;
        //BBS
        if (block.flags.prepare_stage)
            prepare_time += block_time;
        g1_times_cache.push_back({ block.g1_line_id, time });
        // update times for remaining time to printer stop placeholders
        auto it_stop_time = std::lower_bound(stop_times.begin(), stop_times.end(), block.g1_line_id,
            [](const StopTime& t, unsigned int value) { return t.g1_line_id < value; });
        if (it_stop_time != stop_times.end() && it_stop_time->g1_line_id == block.g1_line_id)
            it_stop_time->elapsed_time = time;
    }

    if (keep_last_n_blocks)
        blocks.erase(blocks.begin(), blocks.begin() + n_blocks_process);
    else
        blocks.clear();
}

void TimeProcessor::reset()
{
    extruder_unloaded = true;
    machine_envelope_processing_enabled = false;
    machine_limits = MachineEnvelopeConfig();
    filament_load_times = 0.0f;
    filament_unload_times = 0.0f;
    for (size_t i = 0; i < static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Count); ++i) {
        machines[i].reset();
    }
    machines[static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Normal)].enabled = true;
}

void TimeProcessor::post_process(const std::string& filename, std::vector<MoveVertex>& moves, std::vector<size_t>& lines_ends, size_t total_layer_num)
{
    FilePtr in{ boost::nowide::fopen(filename.c_str(), "rb") };
    if (in.f == nullptr)
        assert(false);// throw Slic3r::RuntimeError(std::string("Time estimator post process export failed.\nCannot open file for reading.\n"));

    // temporary file to contain modified gcode
    std::string out_path = filename + ".postprocess";
    FilePtr out{ boost::nowide::fopen(out_path.c_str(), "wb") };
    if (out.f == nullptr) {
        assert(false);//throw Slic3r::RuntimeError(std::string("Time estimator post process export failed.\nCannot open file for writing.\n"));
    }

    auto time_in_minutes = [](float time_in_seconds) {
        assert(time_in_seconds >= 0.f);
        return int((time_in_seconds + 0.5f) / 60.0f);
    };

    auto time_in_last_minute = [](float time_in_seconds) {
        assert(time_in_seconds <= 60.0f);
        return time_in_seconds / 60.0f;
    };

    auto format_line_M73_main = [](const std::string& mask, int percent, int time) {
        char line_M73[64];
        sprintf(line_M73, mask.c_str(),
            std::to_string(percent).c_str(),
            std::to_string(time).c_str());
        return std::string(line_M73);
    };

    auto format_line_M73_stop_int = [](const std::string& mask, int time) {
        char line_M73[64];
        sprintf(line_M73, mask.c_str(), std::to_string(time).c_str());
        return std::string(line_M73);
    };

    auto format_time_float = [](float time) {
        return float_to_string_decimal_point(time, 2);
    };

    auto format_line_M73_stop_float = [format_time_float](const std::string& mask, float time) {
        char line_M73[64];
        sprintf(line_M73, mask.c_str(), format_time_float(time).c_str());
        return std::string(line_M73);
    };

    std::string gcode_line;
    size_t g1_lines_counter = 0;
    // keeps track of last exported pair <percent, remaining time>
    std::array<std::pair<int, int>, static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Count)> last_exported_main;
    for (size_t i = 0; i < static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Count); ++i) {
        last_exported_main[i] = { 0, time_in_minutes(machines[i].time) };
    }

    // keeps track of last exported remaining time to next printer stop
    std::array<int, static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Count)> last_exported_stop;
    for (size_t i = 0; i < static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Count); ++i) {
        last_exported_stop[i] = time_in_minutes(machines[i].time);
    }

    // buffer line to export only when greater than 64K to reduce writing calls
    std::string export_line;

    // replace placeholder lines with the proper final value
    // gcode_line is in/out parameter, to reduce expensive memory allocation
    auto process_placeholders = [&](std::string& gcode_line) {
        unsigned int extra_lines_count = 0;

        // remove trailing '\n'
        auto line = std::string_view(gcode_line).substr(0, gcode_line.length() - 1);

        std::string ret;
        if (line.length() > 1) {
            line = line.substr(1);
            if (line == reserved_tag(ETags::First_Line_M73_Placeholder) || line == reserved_tag(ETags::Last_Line_M73_Placeholder)) {
                for (size_t i = 0; i < static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Count); ++i) {
                    const TimeMachine& machine = machines[i];
                    if (machine.enabled) {
                        // export pair <percent, remaining time>
                        ret += format_line_M73_main(machine.line_m73_main_mask.c_str(),
                            (line == reserved_tag(ETags::First_Line_M73_Placeholder)) ? 0 : 100,
                            (line == reserved_tag(ETags::First_Line_M73_Placeholder)) ? time_in_minutes(machine.time) : 0);
                        ++extra_lines_count;

                        // export remaining time to next printer stop
                        if (line == reserved_tag(ETags::First_Line_M73_Placeholder) && !machine.stop_times.empty()) {
                            int to_export_stop = time_in_minutes(machine.stop_times.front().elapsed_time);
                            ret += format_line_M73_stop_int(machine.line_m73_stop_mask.c_str(), to_export_stop);
                            last_exported_stop[i] = to_export_stop;
                            ++extra_lines_count;
                        }
                    }
                }
            }
            else if (line == reserved_tag(ETags::Estimated_Printing_Time_Placeholder)) {
                for (size_t i = 0; i < static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Count); ++i) {
                    const TimeMachine& machine = machines[i];
                    PrintEstimatedStatistics::ETimeMode mode = static_cast<PrintEstimatedStatistics::ETimeMode>(i);
                    if (mode == PrintEstimatedStatistics::ETimeMode::Normal || machine.enabled) {
                        char buf[128];
                        //sprintf(buf, "; estimated printing time (%s mode) = %s\n",
                        //    (mode == PrintEstimatedStatistics::ETimeMode::Normal) ? "normal" : "silent",
                        //    get_time_dhms(machine.time).c_str());
                        sprintf(buf, "; model printing time: %s; total estimated time: %s\n",
                            get_time_dhms(machine.time - machine.prepare_time).c_str(),
                            get_time_dhms(machine.time).c_str());
                        ret += buf;
                    }
                }
            }
            //BBS: write total layer number
            else if (line == reserved_tag(ETags::Total_Layer_Number_Placeholder)) {
                char buf[128];
                sprintf(buf, "; total layer number: %zd\n", total_layer_num);
                ret += buf;
            }
        }

        if (!ret.empty())
            // Not moving the move operator on purpose, so that the gcode_line allocation will grow and it will not be reallocated after handful of lines are processed.
            gcode_line = ret;
        return std::tuple(!ret.empty(), (extra_lines_count == 0) ? extra_lines_count : extra_lines_count - 1);
    };

    // check for temporary lines
    auto is_temporary_decoration = [](const std::string_view gcode_line) {
        // remove trailing '\n'
        assert(!gcode_line.empty());
        assert(gcode_line.back() == '\n');

        // return true for decorations which are used in processing the gcode but that should not be exported into the final gcode
        // i.e.:
        // bool ret = gcode_line.substr(0, gcode_line.length() - 1) == ";" + Layer_Change_Tag;
        // ...
        // return ret;
        return false;
    };

    // Iterators for the normal and silent cached time estimate entry recently processed, used by process_line_G1.
    auto g1_times_cache_it = reserve_vector<std::vector<TimeMachine::G1LinesCacheItem>::const_iterator>(machines.size());
    for (const auto& machine : machines)
        g1_times_cache_it.emplace_back(machine.g1_times_cache.begin());

    // add lines M73 to exported gcode
    auto process_line_move = [
        // Lambdas, mostly for string formatting, all with an empty capture block.
        time_in_minutes, format_time_float, format_line_M73_main, format_line_M73_stop_int, format_line_M73_stop_float, time_in_last_minute,
            &self = std::as_const(*this),
            // Caches, to be modified
            &g1_times_cache_it, &last_exported_main, &last_exported_stop,
            // String output
            &export_line]
            (const size_t g1_lines_counter) {
        unsigned int exported_lines_count = 0;
        for (size_t i = 0; i < static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Count); ++i) {
            const TimeMachine& machine = self.machines[i];
            if (machine.enabled) {
                // export pair <percent, remaining time>
                // Skip all machine.g1_times_cache below g1_lines_counter.
                auto& it = g1_times_cache_it[i];
                while (it != machine.g1_times_cache.end() && it->id < g1_lines_counter)
                    ++it;
                if (it != machine.g1_times_cache.end() && it->id == g1_lines_counter) {
                    std::pair<int, int> to_export_main = { int(100.0f * it->elapsed_time / machine.time),
                                                            time_in_minutes(machine.time - it->elapsed_time) };
                    if (last_exported_main[i] != to_export_main) {
                        export_line += format_line_M73_main(machine.line_m73_main_mask.c_str(),
                            to_export_main.first, to_export_main.second);
                        last_exported_main[i] = to_export_main;
                        ++exported_lines_count;
                    }
                    // export remaining time to next printer stop
                    auto it_stop = std::upper_bound(machine.stop_times.begin(), machine.stop_times.end(), it->elapsed_time,
                        [](float value, const TimeMachine::StopTime& t) { return value < t.elapsed_time; });
                    if (it_stop != machine.stop_times.end()) {
                        int to_export_stop = time_in_minutes(it_stop->elapsed_time - it->elapsed_time);
                        if (last_exported_stop[i] != to_export_stop) {
                            if (to_export_stop > 0) {
                                if (last_exported_stop[i] != to_export_stop) {
                                    export_line += format_line_M73_stop_int(machine.line_m73_stop_mask.c_str(), to_export_stop);
                                    last_exported_stop[i] = to_export_stop;
                                    ++exported_lines_count;
                                }
                            }
                            else {
                                bool is_last = false;
                                auto next_it = it + 1;
                                is_last |= (next_it == machine.g1_times_cache.end());

                                if (next_it != machine.g1_times_cache.end()) {
                                    auto next_it_stop = std::upper_bound(machine.stop_times.begin(), machine.stop_times.end(), next_it->elapsed_time,
                                        [](float value, const TimeMachine::StopTime& t) { return value < t.elapsed_time; });
                                    is_last |= (next_it_stop != it_stop);

                                    std::string time_float_str = format_time_float(time_in_last_minute(it_stop->elapsed_time - it->elapsed_time));
                                    std::string next_time_float_str = format_time_float(time_in_last_minute(it_stop->elapsed_time - next_it->elapsed_time));
                                    is_last |= (string_to_double_decimal_point(time_float_str) > 0. && string_to_double_decimal_point(next_time_float_str) == 0.);
                                }

                                if (is_last) {
                                    if (std::distance(machine.stop_times.begin(), it_stop) == static_cast<ptrdiff_t>(machine.stop_times.size() - 1))
                                        export_line += format_line_M73_stop_int(machine.line_m73_stop_mask.c_str(), to_export_stop);
                                    else
                                        export_line += format_line_M73_stop_float(machine.line_m73_stop_mask.c_str(), time_in_last_minute(it_stop->elapsed_time - it->elapsed_time));

                                    last_exported_stop[i] = to_export_stop;
                                    ++exported_lines_count;
                                }
                            }
                        }
                    }
                }
            }
        }
        return exported_lines_count;
    };

        // helper function to write to disk
        size_t out_file_pos = 0;
        lines_ends.clear();
        auto write_string = [&export_line, &out, &out_path, &out_file_pos, &lines_ends](const std::string& str) {
            fwrite((const void*)export_line.c_str(), 1, export_line.length(), out.f);
            if (ferror(out.f)) {
                out.close();
                boost::nowide::remove(out_path.c_str());
                assert(false);//throw RuntimeError(std::string("Time estimator post process export failed.\nIs the disk full?\n"));
            }
            for (size_t i = 0; i < export_line.size(); ++i)
                if (export_line[i] == '\n')
                    lines_ends.emplace_back(out_file_pos + i + 1);
            out_file_pos += export_line.size();
            export_line.clear();
        };

        unsigned int line_id = 0;
        std::vector<std::pair<unsigned int, unsigned int>> offsets;

        {
            // Read the input stream 64kB at a time, extract lines and process them.
            std::vector<char> buffer(65536 * 10, 0);
            // Line buffer.
            assert(gcode_line.empty());
            for (;;) {
                size_t cnt_read = ::fread(buffer.data(), 1, buffer.size(), in.f);
                if (::ferror(in.f))
                    assert(false);//throw RuntimeError(std::string("Time estimator post process export failed.\nError while reading from file.\n"));
                bool eof = cnt_read == 0;
                auto it = buffer.begin();
                auto it_bufend = buffer.begin() + cnt_read;
                while (it != it_bufend || (eof && !gcode_line.empty())) {
                    // Find end of line.
                    bool eol = false;
                    auto it_end = it;
                    for (; it_end != it_bufend && !(eol = *it_end == '\r' || *it_end == '\n'); ++it_end);
                    // End of line is indicated also if end of file was reached.
                    eol |= eof && it_end == it_bufend;
                    gcode_line.insert(gcode_line.end(), it, it_end);
                    if (eol) {
                        ++line_id;

                        gcode_line += "\n";
                        // replace placeholder lines
                        auto [processed, lines_added_count] = process_placeholders(gcode_line);
                        if (processed && lines_added_count > 0)
                            offsets.push_back({ line_id, lines_added_count });
                        if (!processed && !is_temporary_decoration(gcode_line) &&
                            (GCodeLine::cmd_is(gcode_line, "G1") ||
                                GCodeLine::cmd_is(gcode_line, "G2") ||
                                GCodeLine::cmd_is(gcode_line, "G3"))) {
                            // remove temporary lines, add lines M73 where needed
                            unsigned int extra_lines_count = process_line_move(g1_lines_counter++);
                            if (extra_lines_count > 0)
                                offsets.push_back({ line_id, extra_lines_count });
                        }

                        export_line += gcode_line;
                        if (export_line.length() > 65535)
                            write_string(export_line);
                        gcode_line.clear();
                    }
                    // Skip EOL.
                    it = it_end;
                    if (it != it_bufend && *it == '\r')
                        ++it;
                    if (it != it_bufend && *it == '\n')
                        ++it;
                }
                if (eof)
                    break;
            }
        }

        if (!export_line.empty())
            write_string(export_line);

        out.close();
        in.close();

        // updates moves' gcode ids which have been modified by the insertion of the M73 lines
        unsigned int curr_offset_id = 0;
        unsigned int total_offset = 0;
        for (MoveVertex& move : moves) {
            while (curr_offset_id < static_cast<unsigned int>(offsets.size()) && offsets[curr_offset_id].first <= move.gcode_id) {
                total_offset += offsets[curr_offset_id].second;
                ++curr_offset_id;
            }
            move.gcode_id += total_offset;
        }
}
