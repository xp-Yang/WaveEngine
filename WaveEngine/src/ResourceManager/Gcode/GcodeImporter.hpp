#ifndef GCodeLoader_hpp
#define GCodeLoader_hpp

#include "GcodeData.hpp"  
#include "GcodeReader.hpp"  
#include "GcodeResultData.hpp"  

class GCodeProcessor
{
    using ExtruderColors = std::vector<unsigned char>;
    using ExtruderTemps = std::vector<float>;

    static inline const std::string Flush_Start_Tag = " FLUSH_START";
    static inline const std::string Flush_End_Tag = " FLUSH_END";

public:
    // checks the given gcode for reserved tags and returns true when finding the 1st (which is returned into found_tag)
    static bool contains_reserved_tag(const std::string& gcode, std::string& found_tag);
    // checks the given gcode for reserved tags and returns true when finding any
    // (the first max_count found tags are returned into found_tag)
    static bool contains_reserved_tags(const std::string& gcode, unsigned int max_count, std::vector<std::string>& found_tag);

    static int get_gcode_last_filament(const std::string& gcode_str);
    static bool get_last_z_from_gcode(const std::string& gcode_str, double& z);

    static inline const float Wipe_Width = 0.05f;
    static inline const float Wipe_Height = 0.05f;

public:
    GCodeProcessor();

    void enable_stealth_time_estimator(bool enabled);
    bool is_stealth_time_estimator_enabled() const {
        return m_time_processor.machines[static_cast<size_t>(PrintEstimatedStatistics::ETimeMode::Stealth)].enabled;
    }
    void enable_machine_envelope_processing(bool enabled) { m_time_processor.machine_envelope_processing_enabled = enabled; }
    void reset();

    const GCodeProcessorResult& get_result() const { return m_result; }
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

protected:
    //void apply_config(const PrintConfig& config);
    //void apply_config(const DynamicPrintConfig& config);
    //void apply_config_simplify3d(const std::string& filename);
    //void apply_config_superslicer(const std::string& filename);
    void process_gcode_line(const GCodeLine& line, bool producers_enabled);

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
    void process_G0(const GCodeLine& line);
    void process_G1(const GCodeLine& line);
    void process_G2_G3(const GCodeLine& line);
    // BBS: handle delay command
    void process_G4(const GCodeLine& line);
    // Retract
    void process_G10(const GCodeLine& line);
    // Unretract
    void process_G11(const GCodeLine& line);
    // Set Units to Inches
    void process_G20(const GCodeLine& line);
    // Set Units to Millimeters
    void process_G21(const GCodeLine& line);
    // Firmware controlled Retract
    void process_G22(const GCodeLine& line);
    // Firmware controlled Unretract
    void process_G23(const GCodeLine& line);
    // Move to origin
    void process_G28(const GCodeLine& line);
    // BBS
    void process_G29(const GCodeLine& line);
    // Set to Absolute Positioning
    void process_G90(const GCodeLine& line);
    // Set to Relative Positioning
    void process_G91(const GCodeLine& line);
    // Set Position
    void process_G92(const GCodeLine& line);
    // Sleep or Conditional stop
    void process_M1(const GCodeLine& line);
    // Set extruder to absolute mode
    void process_M82(const GCodeLine& line);
    // Set extruder to relative mode
    void process_M83(const GCodeLine& line);
    // Set extruder temperature
    void process_M104(const GCodeLine& line);
    // Set fan speed
    void process_M106(const GCodeLine& line);
    // Disable fan
    void process_M107(const GCodeLine& line);
    // Set tool (Sailfish)
    void process_M108(const GCodeLine& line);
    // Set extruder temperature and wait
    void process_M109(const GCodeLine& line);
    // Recall stored home offsets
    void process_M132(const GCodeLine& line);
    // Set tool (MakerWare)
    void process_M135(const GCodeLine& line);
    //BBS: Set bed temperature
    void process_M140(const GCodeLine& line);
    //BBS: wait bed temperature
    void process_M190(const GCodeLine& line);
    // Set max printing acceleration
    void process_M201(const GCodeLine& line);
    // Set maximum feedrate
    void process_M203(const GCodeLine& line);
    // Set default acceleration
    void process_M204(const GCodeLine& line);
    // Advanced settings
    void process_M205(const GCodeLine& line);
    // Klipper SET_VELOCITY_LIMIT
    void process_SET_VELOCITY_LIMIT(const GCodeLine& line);
    // Set extrude factor override percentage
    void process_M221(const GCodeLine& line);
    // BBS: handle delay command. M400 is defined by BBL only
    void process_M400(const GCodeLine& line);
    // Repetier: Store x, y and z position
    void process_M401(const GCodeLine& line);
    // Repetier: Go to stored position
    void process_M402(const GCodeLine& line);
    // Set allowable instantaneous speed change
    void process_M566(const GCodeLine& line);
    // Unload the current filament into the MK3 MMU2 unit at the end of print.
    void process_M702(const GCodeLine& line);
    // Processes T line (Select Tool)
    void process_T(const GCodeLine& line);
    void process_T(const std::string_view command);

    //different path_type is only used for arc move
    void store_move_vertex(EMoveType type, EMovePathType path_type = EMovePathType::Noop_move);
    void set_extrusion_role(ExtrusionRole role);
    float minimum_feedrate(PrintEstimatedStatistics::ETimeMode mode, float feedrate) const;
    float minimum_travel_feedrate(PrintEstimatedStatistics::ETimeMode mode, float feedrate) const;
    float get_axis_max_feedrate(PrintEstimatedStatistics::ETimeMode mode, Axis axis) const;
    float get_axis_max_acceleration(PrintEstimatedStatistics::ETimeMode mode, Axis axis) const;
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
    int   get_filament_vitrification_temperature(size_t extrude_id);
    void process_custom_gcode_time(CustomGCode::Type code);
    void process_filaments(CustomGCode::Type code);
    // Simulates firmware st_synchronize() call
    void simulate_st_synchronize(float additional_time = 0.0f);
    void update_estimated_times_stats();
    void update_slice_warnings();

private:
    friend struct UsedFilaments;

    GCodeReader m_parser;

    EUnits m_units;
    EPositioningType m_global_positioning_type;
    EPositioningType m_e_local_positioning_type;
    std::vector<Vec3f> m_extruder_offsets;
    GCodeFlavor m_flavor;
    float       m_nozzle_volume;
    AxisCoords m_start_position; // mm
    AxisCoords m_end_position; // mm
    AxisCoords m_origin; // mm
    CachedPosition m_cached_position;
    bool m_wiping;
    bool m_flushing;
    float m_remaining_volume;

    //BBS: x, y offset for gcode generated
    double          m_x_offset{ 0 };
    double          m_y_offset{ 0 };
    //BBS: arc move related data
    EMovePathType m_move_path_type{ EMovePathType::Noop_move };
    Vec3f m_arc_center{ Vec3f() };    // mm
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
    unsigned char m_extruder_id;
    unsigned char m_last_extruder_id;
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
    bool m_spiral_vase_active;

    static inline const std::vector<std::pair<EProducer, std::string>> Producers = {
        //BBS: BambuStudio is also "bambu". Otherwise the time estimation didn't work.
        //FIXME: Workaround and should be handled when do removing-bambu
        { EProducer::BambuStudio, "@SLIC3R_APP_NAME@"},
        { EProducer::BambuStudio, "generated by BambuStudio" }
    };
    EProducer m_producer;

    TimeProcessor m_time_processor;
    UsedFilaments m_used_filaments;

    GCodeProcessorResult m_result;
    static inline unsigned int s_result_id = 0;
};


#endif