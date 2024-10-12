#ifndef GCodeLoader_hpp
#define GCodeLoader_hpp

#include "GcodeResult.hpp"  

using Vec3f = Math::Vec3;
using Pointfs = std::vector<Math::Vec2>;

// slice warnings enum strings
#define NOZZLE_HRC_CHECKER                                          "the_actual_nozzle_hrc_smaller_than_the_required_nozzle_hrc"
#define BED_TEMP_TOO_HIGH_THAN_FILAMENT                             "bed_temperature_too_high_than_filament"
#define NOT_SUPPORT_TRADITIONAL_TIMELAPSE                           "not_support_traditional_timelapse"
#define NOT_GENERATE_TIMELAPSE                                      "not_generate_timelapse"
#define LONG_RETRACTION_WHEN_CUT                                    "activate_long_retraction_when_cut"

class GCodeProcessor
{
    static const std::vector<std::string> Reserved_Tags;
    static const std::string Flush_Start_Tag;
    static const std::string Flush_End_Tag;
    static const std::string VFlush_Start_Tag;
    static const std::string VFlush_End_Tag;
public:
    enum class ETags : unsigned char
    {
        Role,
        Wipe_Start,
        Wipe_End,
        Height,
        Width,
        Layer_Change,
        Color_Change,
        Pause_Print,
        Custom_Code,
        First_Line_M73_Placeholder,
        Last_Line_M73_Placeholder,
        Estimated_Printing_Time_Placeholder,
        Total_Layer_Number_Placeholder,
        Wipe_Tower_Start,
        Wipe_Tower_End,
        Used_Filament_Weight_Placeholder,
        Used_Filament_Volume_Placeholder,
        Used_Filament_Length_Placeholder
    };

    static const std::string& reserved_tag(ETags tag) { return Reserved_Tags[static_cast<unsigned char>(tag)]; }
    // checks the given gcode for reserved tags and returns true when finding the 1st (which is returned into found_tag)
    static bool contains_reserved_tag(const std::string& gcode, std::string& found_tag);
    // checks the given gcode for reserved tags and returns true when finding any
    // (the first max_count found tags are returned into found_tag)
    static bool contains_reserved_tags(const std::string& gcode, unsigned int max_count, std::vector<std::string>& found_tag);

    static int get_gcode_last_filament(const std::string& gcode_str);
    static bool get_last_z_from_gcode(const std::string& gcode_str, double& z);

    static const float Wipe_Width;
    static const float Wipe_Height;

    static bool s_IsBBLPrinter;

#if ENABLE_GCODE_VIEWER_DATA_CHECKING
    static const std::string Mm3_Per_Mm_Tag;
#endif // ENABLE_GCODE_VIEWER_DATA_CHECKING

private:
    using AxisCoords = std::array<double, 4>;
    using ExtruderColors = std::vector<unsigned char>;
    using ExtruderTemps = std::vector<float>;

    enum class EUnits : unsigned char
    {
        Millimeters,
        Inches
    };

    enum class EPositioningType : unsigned char
    {
        Absolute,
        Relative
    };

    struct CachedPosition
    {
        AxisCoords position; // mm
        float feedrate; // mm/s

        void reset();
    };

    struct CpColor
    {
        unsigned char counter;
        unsigned char current;

        void reset();
    };

public:
    struct FeedrateProfile
    {
        float entry{ 0.0f }; // mm/s
        float cruise{ 0.0f }; // mm/s
        float exit{ 0.0f }; // mm/s
    };

    struct Trapezoid
    {
        float accelerate_until{ 0.0f }; // mm
        float decelerate_after{ 0.0f }; // mm
        float cruise_feedrate{ 0.0f }; // mm/sec

        float acceleration_time(float entry_feedrate, float acceleration) const;
        float cruise_time() const;
        float deceleration_time(float distance, float acceleration) const;
        float cruise_distance() const;
    };

    struct TimeBlock
    {
        struct Flags
        {
            bool recalculate{ false };
            bool nominal_length{ false };
            bool prepare_stage{ false };
        };

        EMoveType move_type{ EMoveType::Noop };
        ExtrusionRole role{ erNone };
        unsigned int g1_line_id{ 0 };
        unsigned int layer_id{ 0 };
        float distance{ 0.0f }; // mm
        float acceleration{ 0.0f }; // mm/s^2
        float max_entry_speed{ 0.0f }; // mm/s
        float safe_feedrate{ 0.0f }; // mm/s
        Flags flags;
        FeedrateProfile feedrate_profile;
        Trapezoid trapezoid;

        // Calculates this block's trapezoid
        void calculate_trapezoid();

        float time() const;
    };


private:
    struct TimeMachine
    {
        struct State
        {
            float feedrate; // mm/s
            float safe_feedrate; // mm/s
            //BBS: feedrate of X-Y-Z-E axis. But when the move is G2 and G3, X-Y will be
            //same value which means feedrate in X-Y plane.
            AxisCoords axis_feedrate; // mm/s
            AxisCoords abs_axis_feedrate; // mm/s

            //BBS: unit vector of enter speed and exit speed in x-y-z space.
            //For line move, there are same. For arc move, there are different.
            Vec3f enter_direction;
            Vec3f exit_direction;

            void reset();
        };

        struct G1LinesCacheItem
        {
            unsigned int id;
            float elapsed_time;
        };

        bool enabled;
        float acceleration; // mm/s^2
        // hard limit for the acceleration, to which the firmware will clamp.
        float max_acceleration; // mm/s^2
        float retract_acceleration; // mm/s^2
        // hard limit for the acceleration, to which the firmware will clamp.
        float max_retract_acceleration; // mm/s^2
        float travel_acceleration; // mm/s^2
        // hard limit for the travel acceleration, to which the firmware will clamp.
        float max_travel_acceleration; // mm/s^2
        float extrude_factor_override_percentage;
        float time; // s
        struct StopTime
        {
            unsigned int g1_line_id;
            float elapsed_time;
        };
        std::vector<StopTime> stop_times;
        std::string line_m73_main_mask;
        std::string line_m73_stop_mask;
        State curr;
        State prev;
        std::vector<TimeBlock> blocks;
        std::vector<G1LinesCacheItem> g1_times_cache;
        std::array<float, static_cast<size_t>(EMoveType::Count)> moves_time;
        std::array<float, static_cast<size_t>(ExtrusionRole::erCount)> roles_time;
        std::vector<float> layers_time;
        //BBS: prepare stage time before print model, including start gcode time and mostly same with start gcode time
        float prepare_time;

        void reset();

        // Simulates firmware st_synchronize() call
        void simulate_st_synchronize(float additional_time = 0.0f);
        void calculate_time(size_t keep_last_n_blocks = 0, float additional_time = 0.0f);
    };

    struct UsedFilaments  // filaments per ColorChange
    {
        double color_change_cache;
        std::vector<double> volumes_per_color_change;

        double model_extrude_cache;
        std::map<size_t, double> model_volumes_per_filament;

        double wipe_tower_cache;
        std::map<size_t, double>wipe_tower_volumes_per_filament;

        double support_volume_cache;
        std::map<size_t, double>support_volumes_per_filament;

        //BBS: the flush amount of every filament
        std::map<size_t, double> flush_per_filament;

        double total_volume_cache;
        std::map<size_t, double>total_volumes_per_filament;

        double role_cache;
        std::map<ExtrusionRole, std::pair<double, double>> filaments_per_role;

        void reset();

        void increase_support_caches(double extruded_volume);
        void increase_model_caches(double extruded_volume);
        void increase_wipe_tower_caches(double extruded_volume);

        void process_color_change_cache();
        void process_model_cache(GCodeProcessor* processor);
        void process_wipe_tower_cache(GCodeProcessor* processor);
        void process_support_cache(GCodeProcessor* processor);
        void process_total_volume_cache(GCodeProcessor* processor);

        void update_flush_per_filament(size_t extrude_id, float flush_length);
        void process_role_cache(GCodeProcessor* processor);
        void process_caches(GCodeProcessor* processor);

        friend class GCodeProcessor;
    };

    struct TimeProcessContext
    {
        size_t total_layer_num;
        std::vector<Extruder> filament_lists;
        UsedFilaments used_filaments;
        TimeProcessContext(size_t total_layer_num_,
            const std::vector<Extruder>& filament_lists_,
            const UsedFilaments& used_filaments_)
            :total_layer_num(total_layer_num_), filament_lists(filament_lists_), used_filaments(used_filaments_) {}
    };

    struct TimeProcessor
    {
        struct Planner
        {
            // Size of the firmware planner queue. The old 8-bit Marlins usually just managed 16 trapezoidal blocks.
            // Let's be conservative and plan for newer boards with more memory.
            static constexpr size_t queue_size = 64;
            // The firmware recalculates last planner_queue_size trapezoidal blocks each time a new block is added.
            // We are not simulating the firmware exactly, we calculate a sequence of blocks once a reasonable number of blocks accumulate.
            static constexpr size_t refresh_threshold = queue_size * 4;
        };

        // extruder_id is currently used to correctly calculate filament load / unload times into the total print time.
        // This is currently only really used by the MK3 MMU2:
        // extruder_unloaded = true means no filament is loaded yet, all the filaments are parked in the MK3 MMU2 unit.
        bool extruder_unloaded;
        // allow to skip the lines M201/M203/M204/M205 generated by GCode::print_machine_envelope() for non-Normal time estimate mode
        bool machine_envelope_processing_enabled;
        MachineEnvelopeConfig machine_limits;
        // Additional load / unload times for a filament exchange sequence.
        float filament_load_times;
        float filament_unload_times;
        float extruder_change_times;

        std::array<TimeMachine, static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Count)> machines;

        void reset();

        // post process the file with the given filename to add remaining time lines M73
        // and updates moves' gcode ids accordingly
        void post_process(const std::string& filename, std::vector<MoveVertex>& moves, std::vector<size_t>& lines_ends, const TimeProcessContext& context);
    };
public:
    class SeamsDetector
    {
        bool m_active{ false };
        std::optional<Vec3f> m_first_vertex;

    public:
        void activate(bool active) {
            if (m_active != active) {
                m_active = active;
                if (m_active)
                    m_first_vertex.reset();
            }
        }

        std::optional<Vec3f> get_first_vertex() const { return m_first_vertex; }
        void set_first_vertex(const Vec3f& vertex) { m_first_vertex = vertex; }

        bool is_active() const { return m_active; }
        bool has_first_vertex() const { return m_first_vertex.has_value(); }
    };

    // Helper class used to fix the z for color change, pause print and
    // custom gcode markes
    class OptionsZCorrector
    {
        GCodeProcessorResult& m_result;
        std::optional<size_t> m_move_id;
        std::optional<size_t> m_custom_gcode_per_print_z_id;

    public:
        explicit OptionsZCorrector(GCodeProcessorResult& result) : m_result(result) {
        }

        void set() {
            m_move_id = m_result.moves.size() - 1;
            m_custom_gcode_per_print_z_id = m_result.custom_gcode_per_print_z.size() - 1;
        }

        void update(float height) {
            if (!m_move_id.has_value() || !m_custom_gcode_per_print_z_id.has_value())
                return;

            const Vec3f position = m_result.moves.back().position;

            MoveVertex& move = m_result.moves.emplace_back(m_result.moves[*m_move_id]);
            move.position = position;
            move.height = height;
            m_result.moves.erase(m_result.moves.begin() + *m_move_id);
            m_result.custom_gcode_per_print_z[*m_custom_gcode_per_print_z_id].print_z = position.z();
            reset();
        }

        void reset() {
            m_move_id.reset();
            m_custom_gcode_per_print_z_id.reset();
        }
    };

private:
    GCodeReader m_parser;
    EUnits m_units;
    EPositioningType m_global_positioning_type;
    EPositioningType m_e_local_positioning_type;
    std::vector<Vec3f> m_extruder_offsets;
    GCodeFlavor m_flavor;
    std::vector<float> m_nozzle_volume;
    AxisCoords m_start_position; // mm
    AxisCoords m_end_position; // mm
    AxisCoords m_origin; // mm
    CachedPosition m_cached_position;
    bool m_wiping;
    bool m_flushing; // mark a section with real flush
    bool m_virtual_flushing; // mark a section with virtual flush, only for statistics
    bool m_wipe_tower;
    std::vector<float> m_remaining_volume;
    std::vector<Extruder> m_filament_lists;

    //BBS: x, y offset for gcode generated
    double          m_x_offset{ 0 };
    double          m_y_offset{ 0 };
    //BBS: arc move related data
    EMovePathType m_move_path_type{ EMovePathType::Noop_move };
    Vec3f m_arc_center{ };    // mm
    std::vector<Vec3f> m_interpolation_points;

    unsigned int m_line_id;
    unsigned int m_last_line_id;
    float m_feedrate; // mm/s
    float m_width; // mm
    float m_height; // mm
    float m_forced_width; // mm
    float m_forced_height; // mm
    float m_mm3_per_mm;
    float m_fan_speed; // percentage
    ExtrusionRole m_extrusion_role;
    std::vector<int> m_filament_maps;
    std::vector<unsigned char> m_last_filament_id;
    std::vector<unsigned char> m_filament_id;
    unsigned char m_extruder_id;
    ExtruderColors m_extruder_colors;
    ExtruderTemps m_extruder_temps;
    int m_highest_bed_temp;
    float m_extruded_last_z;
    float m_first_layer_height; // mm
    float m_zero_layer_height; // mm
    bool m_processing_start_custom_gcode;
    unsigned int m_g1_line_id;
    unsigned int m_layer_id;
    CpColor m_cp_color;
    SeamsDetector m_seams_detector;
    OptionsZCorrector m_options_z_corrector;
    size_t m_last_default_color_id;
    bool m_detect_layer_based_on_tag{ false };
    int m_seams_count;
#if ENABLE_GCODE_VIEWER_STATISTICS
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
#endif // ENABLE_GCODE_VIEWER_STATISTICS

    enum class EProducer
    {
        Unknown,
        BambuStudio,
        Slic3rPE,
        Slic3r,
        SuperSlicer,
        Cura,
        Simplify3D,
        CraftWare,
        ideaMaker,
        KissSlicer
    };

    static const std::vector<std::pair<GCodeProcessor::EProducer, std::string>> Producers;
    EProducer m_producer;

    TimeProcessor m_time_processor;
    UsedFilaments m_used_filaments;

    GCodeProcessorResult m_result;
    static unsigned int s_result_id;

#if ENABLE_GCODE_VIEWER_DATA_CHECKING
    DataChecker m_mm3_per_mm_compare{ "mm3_per_mm", 0.01f };
    DataChecker m_height_compare{ "height", 0.01f };
    DataChecker m_width_compare{ "width", 0.01f };
#endif // ENABLE_GCODE_VIEWER_DATA_CHECKING

public:
    GCodeProcessor();

    // check whether the gcode path meets the filament_map grouping requirements
    bool check_multi_extruder_gcode_valid(const std::vector<Polygons>& unprintable_areas, const std::vector<int>& filament_map);
    void apply_config(const PrintConfig& config);

    void set_filaments(const std::vector<Extruder>& filament_lists) { m_filament_lists = filament_lists; }

    void enable_stealth_time_estimator(bool enabled);
    bool is_stealth_time_estimator_enabled() const {
        return m_time_processor.machines[static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Stealth)].enabled;
    }
    void enable_machine_envelope_processing(bool enabled) { m_time_processor.machine_envelope_processing_enabled = enabled; }
    void reset();

    const GCodeProcessorResult& get_result() const { return m_result; }
    GCodeProcessorResult& result() { return m_result; }
    GCodeProcessorResult&& extract_result() { return std::move(m_result); }

    // Load a G-code into a stand-alone G-code viewer.
    // throws CanceledException through print->throw_if_canceled() (sent by the caller as callback).
    void process_file(const std::string& filename, std::function<void()> cancel_callback = nullptr);

    // Streaming interface, for processing G-codes just generated by PrusaSlicer in a pipelined fashion.
    void initialize(const std::string& filename);
    void process_buffer(const std::string& buffer);
    void finalize(bool post_process);

    float get_time(PrintEstimatedStatistics::ETimeMode mode) const;
    float get_prepare_time(PrintEstimatedStatistics::ETimeMode mode) const;
    std::string get_time_dhm(PrintEstimatedStatistics::ETimeMode mode) const;
    std::vector<std::pair<CustomGCode::Type, std::pair<float, float>>> get_custom_gcode_times(PrintEstimatedStatistics::ETimeMode mode, bool include_remaining) const;

    std::vector<std::pair<EMoveType, float>> get_moves_time(PrintEstimatedStatistics::ETimeMode mode) const;
    std::vector<std::pair<ExtrusionRole, float>> get_roles_time(PrintEstimatedStatistics::ETimeMode mode) const;
    std::vector<float> get_layers_time(PrintEstimatedStatistics::ETimeMode mode) const;

    //BBS: set offset for gcode writer
    void set_xy_offset(double x, double y) { m_x_offset = x; m_y_offset = y; }
    // Orca: if true, only change new layer if ETags::Layer_Change occurs
    // otherwise when we got a lift of z during extrusion, a new layer will be added
    void detect_layer_based_on_tag(bool enabled) {
        m_detect_layer_based_on_tag = enabled;
    }

private:
    void apply_config(const DynamicPrintConfig& config);
    void apply_config_simplify3d(const std::string& filename);
    void apply_config_superslicer(const std::string& filename);
    void process_gcode_line(const GCodeReader::GCodeLine& line, bool producers_enabled);

    // Process tags embedded into comments
    void process_tags(const std::string_view comment, bool producers_enabled);
    bool process_producers_tags(const std::string_view comment);
    bool process_bambuslicer_tags(const std::string_view comment);
    bool process_cura_tags(const std::string_view comment);
    bool process_simplify3d_tags(const std::string_view comment);
    bool process_craftware_tags(const std::string_view comment);
    bool process_ideamaker_tags(const std::string_view comment);
    bool process_kissslicer_tags(const std::string_view comment);

    bool detect_producer(const std::string_view comment);

    // Move
    void process_G0(const GCodeReader::GCodeLine& line);
    void process_G1(const GCodeReader::GCodeLine& line);
    void process_G2_G3(const GCodeReader::GCodeLine& line);

    void process_VG1(const GCodeReader::GCodeLine& line);


    // BBS: handle delay command
    void process_G4(const GCodeReader::GCodeLine& line);

    // Retract
    void process_G10(const GCodeReader::GCodeLine& line);

    // Unretract
    void process_G11(const GCodeReader::GCodeLine& line);

    // Set Units to Inches
    void process_G20(const GCodeReader::GCodeLine& line);

    // Set Units to Millimeters
    void process_G21(const GCodeReader::GCodeLine& line);

    // Firmware controlled Retract
    void process_G22(const GCodeReader::GCodeLine& line);

    // Firmware controlled Unretract
    void process_G23(const GCodeReader::GCodeLine& line);

    // Move to origin
    void process_G28(const GCodeReader::GCodeLine& line);

    // BBS
    void process_G29(const GCodeReader::GCodeLine& line);

    // Set to Absolute Positioning
    void process_G90(const GCodeReader::GCodeLine& line);

    // Set to Relative Positioning
    void process_G91(const GCodeReader::GCodeLine& line);

    // Set Position
    void process_G92(const GCodeReader::GCodeLine& line);

    // Sleep or Conditional stop
    void process_M1(const GCodeReader::GCodeLine& line);

    // Set extruder to absolute mode
    void process_M82(const GCodeReader::GCodeLine& line);

    // Set extruder to relative mode
    void process_M83(const GCodeReader::GCodeLine& line);

    // Set extruder temperature
    void process_M104(const GCodeReader::GCodeLine& line);

    // Set fan speed
    void process_M106(const GCodeReader::GCodeLine& line);

    // Disable fan
    void process_M107(const GCodeReader::GCodeLine& line);

    // Set tool (Sailfish)
    void process_M108(const GCodeReader::GCodeLine& line);

    // Set extruder temperature and wait
    void process_M109(const GCodeReader::GCodeLine& line);

    // Recall stored home offsets
    void process_M132(const GCodeReader::GCodeLine& line);

    // Set tool (MakerWare)
    void process_M135(const GCodeReader::GCodeLine& line);

    //BBS: Set bed temperature
    void process_M140(const GCodeReader::GCodeLine& line);

    //BBS: wait bed temperature
    void process_M190(const GCodeReader::GCodeLine& line);

    //BBS: wait chamber temperature
    void process_M191(const GCodeReader::GCodeLine& line);

    // Set max printing acceleration
    void process_M201(const GCodeReader::GCodeLine& line);

    // Set maximum feedrate
    void process_M203(const GCodeReader::GCodeLine& line);

    // Set default acceleration
    void process_M204(const GCodeReader::GCodeLine& line);

    // Advanced settings
    void process_M205(const GCodeReader::GCodeLine& line);

    // Klipper SET_VELOCITY_LIMIT
    void process_SET_VELOCITY_LIMIT(const GCodeReader::GCodeLine& line);

    // Set extrude factor override percentage
    void process_M221(const GCodeReader::GCodeLine& line);

    // BBS: handle delay command. M400 is defined by BBL only
    void process_M400(const GCodeReader::GCodeLine& line);

    // Repetier: Store x, y and z position
    void process_M401(const GCodeReader::GCodeLine& line);

    // Repetier: Go to stored position
    void process_M402(const GCodeReader::GCodeLine& line);

    // Set allowable instantaneous speed change
    void process_M566(const GCodeReader::GCodeLine& line);

    // Unload the current filament into the MK3 MMU2 unit at the end of print.
    void process_M702(const GCodeReader::GCodeLine& line);

    // Processes T line (Select Tool)
    void process_T(const GCodeReader::GCodeLine& line);
    void process_T(const std::string_view command);
    void process_M1020(const GCodeReader::GCodeLine& line);

    void process_filament_change(int id);

    //BBS: different path_type is only used for arc move
    void store_move_vertex(EMoveType type, EMovePathType path_type = EMovePathType::Noop_move);

    void set_extrusion_role(ExtrusionRole role);

    float minimum_feedrate(PrintEstimatedStatistics::ETimeMode mode, float feedrate) const;
    float minimum_travel_feedrate(PrintEstimatedStatistics::ETimeMode mode, float feedrate) const;
    float get_axis_max_feedrate(PrintEstimatedStatistics::ETimeMode mode, Axis axis, int extruder_id) const;
    float get_axis_max_acceleration(PrintEstimatedStatistics::ETimeMode mode, Axis axis, int extruder_id) const;
    float get_axis_max_jerk(PrintEstimatedStatistics::ETimeMode mode, Axis axis) const;
    Vec3f get_xyz_max_jerk(PrintEstimatedStatistics::ETimeMode mode) const;
    float get_retract_acceleration(PrintEstimatedStatistics::ETimeMode mode) const;
    void  set_retract_acceleration(PrintEstimatedStatistics::ETimeMode mode, float value);
    float get_acceleration(PrintEstimatedStatistics::ETimeMode mode) const;
    void  set_acceleration(PrintEstimatedStatistics::ETimeMode mode, float value);
    float get_travel_acceleration(PrintEstimatedStatistics::ETimeMode mode) const;
    void  set_travel_acceleration(PrintEstimatedStatistics::ETimeMode mode, float value);
    float get_filament_load_time(size_t extruder_id);
    float get_filament_unload_time(size_t extruder_id);
    float get_extruder_change_time(size_t extruder_id);
    int   get_filament_vitrification_temperature(size_t extrude_id);
    void process_custom_gcode_time(CustomGCode::Type code);
    void process_filaments(CustomGCode::Type code);

    // Simulates firmware st_synchronize() call
    void simulate_st_synchronize(float additional_time = 0.0f);

    void update_estimated_times_stats();
    //BBS:
    void update_slice_warnings();

    // get current used filament
    int get_filament_id(bool force_initialize = true) const;
    // get last used filament in the same extruder with current filament
    int get_last_filament_id(bool force_initialize = true) const;
    //get current used extruder
    int get_extruder_id(bool force_initialize = true)const;
};


#endif