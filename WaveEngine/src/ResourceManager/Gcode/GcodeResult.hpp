#ifndef GcodeResult_hpp
#define GcodeResult_hpp

#include "Core/Common.hpp"

enum class EMoveType : unsigned char
{
    Noop,
    Retract,
    Unretract,
    Seam,
    Tool_change,
    Color_change,
    Pause_Print,
    Custom_GCode,
    Travel,
    Wipe,
    Extrude,
    Count
};

enum ExtrusionRole : uint8_t {
    erNone,
    erPerimeter,
    erExternalPerimeter,
    erOverhangPerimeter,
    erInternalInfill,
    erSolidInfill,
    erTopSolidInfill,
    erBottomSurface,
    erIroning,
    erBridgeInfill,
    erGapFill,
    erSkirt,
    erBrim,
    erSupportMaterial,
    erSupportMaterialInterface,
    erSupportTransition,
    erWipeTower,
    erCustom,
    // Extrusion role for a collection with multiple extrusion roles.
    erMixed,
    erCount
};

enum class EMovePathType : unsigned char
{
    Noop_move,
    Linear_move,
    Arc_move_cw,
    Arc_move_ccw,
    Count
};

enum NozzleType {
    ntUndefine = 0,
    ntHardenedSteel,
    ntStainlessSteel,
    ntBrass,
    ntCount
};

enum BedType {
    btDefault = 0,
    btSuperTack,
    btPC,
    btEP,
    btPEI,
    btPTE,
    btCount
};

enum GCodeFlavor : unsigned char {
    gcfMarlinLegacy, gcfKlipper, gcfRepRapSprinter, gcfRepRapFirmware, gcfRepetier, gcfTeacup, gcfMakerWare, gcfMarlinFirmware, gcfSailfish, gcfMach3, gcfMachinekit,
    gcfSmoothie, gcfNoExtrusion
};

struct MoveVertex
{
    unsigned int gcode_id{ 0 };

    EMoveType type{ EMoveType::Noop };
    ExtrusionRole extrusion_role{ erNone };

    unsigned char extruder_id{ 0 };

    //unsigned char cp_color_id{ 0 };

    Vec3f position{ }; // mm
    float delta_extruder{ 0.0f }; // mm
    float feedrate{ 0.0f }; // mm/s
    float width{ 0.0f }; // mm
    float height{ 0.0f }; // mm
    float mm3_per_mm{ 0.0f };
    float fan_speed{ 0.0f }; // percentage
    float temperature{ 0.0f }; // Celsius degrees
    float time{ 0.0f }; // s
    float layer_duration{ 0.0f }; // s (layer id before finalize)


    //BBS: arc move related data
    EMovePathType move_path_type{ EMovePathType::Noop_move };
    Vec3f arc_center_position{ };      // mm
    std::vector<Vec3f> interpolation_points;     // interpolation points of arc for drawing

    float volumetric_rate() const { return feedrate * mm3_per_mm; }
    //BBS: new function to support arc move
    bool is_arc_move_with_interpolation_points() const {
        return (move_path_type == EMovePathType::Arc_move_ccw || move_path_type == EMovePathType::Arc_move_cw) && interpolation_points.size();
    }
    bool is_arc_move() const {
        return move_path_type == EMovePathType::Arc_move_ccw || move_path_type == EMovePathType::Arc_move_cw;
    }
};


struct GCodeProcessorResult
{
    struct SettingsIds
    {
        std::string print;
        std::vector<std::string> filament;
        std::string printer;

        void reset() {
            print.clear();
            filament.clear();
            printer.clear();
        }
    };

    struct SliceWarning {
        int         level;                  // 0: normal tips, 1: warning; 2: error
        std::string msg;                    // enum string
        std::string error_code;             // error code for studio
        std::vector<std::string> params;    // extra msg info
    };

    std::string filename;
    unsigned int id;
    std::vector<MoveVertex> moves;
    // Positions of ends of lines of the final G-code this->filename after TimeProcessor::post_process() finalizes the G-code.
    std::vector<size_t> lines_ends;
    Pointfs printable_area;
    //BBS: add bed exclude area
    Pointfs bed_exclude_area;
    std::vector<Pointfs> extruder_areas;
    //BBS: add toolpath_outside
    bool toolpath_outside;
    //BBS: add object_label_enabled
    bool label_object_enabled;
    //BBS : extra retraction when change filament,experiment func
    bool long_retraction_when_cut{ 0 };
    int timelapse_warning_code{ 0 };
    bool support_traditional_timelapse{ true };
    float printable_height;
    SettingsIds settings_ids;
    size_t filaments_count;
    std::vector<std::string> extruder_colors;
    std::vector<float> filament_diameters;
    std::vector<int>   required_nozzle_HRC;
    std::vector<float> filament_densities;
    std::vector<float> filament_costs;
    std::vector<int> filament_vitrification_temperature;
    std::vector<int>   filament_maps;
    std::vector<int>   limit_filament_maps;
    std::vector<std::pair<float, std::pair<size_t, size_t>>> spiral_vase_layers;
    //BBS
    std::vector<SliceWarning> warnings;
    std::vector<NozzleType> nozzle_type;
    BedType bed_type = BedType::btCount;
#if ENABLE_GCODE_VIEWER_STATISTICS
    int64_t time{ 0 };
#endif // ENABLE_GCODE_VIEWER_STATISTICS
    void reset();

    //BBS: add mutex for protection of gcode result
    mutable std::mutex result_mutex;
    void  lock() const { result_mutex.lock(); }
    void  unlock() const { result_mutex.unlock(); }
};

#endif // !GcodeResult_hpp
