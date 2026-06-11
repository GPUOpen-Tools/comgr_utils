//============================================================================================
// Copyright (c) 2026 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools
/// @file
/// @brief  This is a high level C++ interface of comgr utility functionality for tools.
//============================================================================================

#ifndef COMGR_UTILS_H_
#define COMGR_UTILS_H_

#ifdef COMGR_DYNAMIC_LINKING
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <tchar.h>
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#endif

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "amd_comgr.h"

namespace amdt
{
    class MDNode;
    class CodeObj;

    /// @brief Singleton struct to hold the entry points of the comgr library.
    struct ComgrEntryPoints
    {
        decltype(amd_comgr_status_string)*                          amd_comgr_status_string_fn;                           ///< comgr library entry point.
        decltype(amd_comgr_get_version)*                            amd_comgr_get_version_fn;                             ///< comgr library entry point.
        decltype(amd_comgr_get_isa_count)*                          amd_comgr_get_isa_count_fn;                           ///< comgr library entry point.
        decltype(amd_comgr_get_isa_name)*                           amd_comgr_get_isa_name_fn;                            ///< comgr library entry point.
        decltype(amd_comgr_get_isa_metadata)*                       amd_comgr_get_isa_metadata_fn;                        ///< comgr library entry point.
        decltype(amd_comgr_create_data)*                            amd_comgr_create_data_fn;                             ///< comgr library entry point.
        decltype(amd_comgr_release_data)*                           amd_comgr_release_data_fn;                            ///< comgr library entry point.
        decltype(amd_comgr_get_data_kind)*                          amd_comgr_get_data_kind_fn;                           ///< comgr library entry point.
        decltype(amd_comgr_set_data)*                               amd_comgr_set_data_fn;                                ///< comgr library entry point.
        decltype(amd_comgr_set_data_name)*                          amd_comgr_set_data_name_fn;                           ///< comgr library entry point.
        decltype(amd_comgr_get_data)*                               amd_comgr_get_data_fn;                                ///< comgr library entry point.
        decltype(amd_comgr_get_data_name)*                          amd_comgr_get_data_name_fn;                           ///< comgr library entry point.
        decltype(amd_comgr_get_data_isa_name)*                      amd_comgr_get_data_isa_name_fn;                       ///< comgr library entry point.
        decltype(amd_comgr_get_data_metadata)*                      amd_comgr_get_data_metadata_fn;                       ///< comgr library entry point.
        decltype(amd_comgr_destroy_metadata)*                       amd_comgr_destroy_metadata_fn;                        ///< comgr library entry point.
        decltype(amd_comgr_create_data_set)*                        amd_comgr_create_data_set_fn;                         ///< comgr library entry point.
        decltype(amd_comgr_destroy_data_set)*                       amd_comgr_destroy_data_set_fn;                        ///< comgr library entry point.
        decltype(amd_comgr_data_set_add)*                           amd_comgr_data_set_add_fn;                            ///< comgr library entry point.
        decltype(amd_comgr_data_set_remove)*                        amd_comgr_data_set_remove_fn;                         ///< comgr library entry point.
        decltype(amd_comgr_action_data_count)*                      amd_comgr_action_data_count_fn;                       ///< comgr library entry point.
        decltype(amd_comgr_action_data_get_data)*                   amd_comgr_action_data_get_data_fn;                    ///< comgr library entry point.
        decltype(amd_comgr_create_action_info)*                     amd_comgr_create_action_info_fn;                      ///< comgr library entry point.
        decltype(amd_comgr_destroy_action_info)*                    amd_comgr_destroy_action_info_fn;                     ///< comgr library entry point.
        decltype(amd_comgr_action_info_set_isa_name)*               amd_comgr_action_info_set_isa_name_fn;                ///< comgr library entry point.
        decltype(amd_comgr_action_info_get_isa_name)*               amd_comgr_action_info_get_isa_name_fn;                ///< comgr library entry point.
        decltype(amd_comgr_action_info_set_language)*               amd_comgr_action_info_set_language_fn;                ///< comgr library entry point.
        decltype(amd_comgr_action_info_get_language)*               amd_comgr_action_info_get_language_fn;                ///< comgr library entry point.
        decltype(amd_comgr_action_info_set_option_list)*            amd_comgr_action_info_set_option_list_fn;             ///< comgr library entry point.
        decltype(amd_comgr_action_info_get_option_list_count)*      amd_comgr_action_info_get_option_list_count_fn;       ///< comgr library entry point.
        decltype(amd_comgr_action_info_get_option_list_item)*       amd_comgr_action_info_get_option_list_item_fn;        ///< comgr library entry point.
        decltype(amd_comgr_action_info_set_working_directory_path)* amd_comgr_action_info_set_working_directory_path_fn;  ///< comgr library entry point.
        decltype(amd_comgr_action_info_get_working_directory_path)* amd_comgr_action_info_get_working_directory_path_fn;  ///< comgr library entry point.
        decltype(amd_comgr_action_info_set_logging)*                amd_comgr_action_info_set_logging_fn;                 ///< comgr library entry point.
        decltype(amd_comgr_action_info_get_logging)*                amd_comgr_action_info_get_logging_fn;                 ///< comgr library entry point.
        decltype(amd_comgr_do_action)*                              amd_comgr_do_action_fn;                               ///< comgr library entry point.
        decltype(amd_comgr_get_metadata_kind)*                      amd_comgr_get_metadata_kind_fn;                       ///< comgr library entry point.
        decltype(amd_comgr_get_metadata_string)*                    amd_comgr_get_metadata_string_fn;                     ///< comgr library entry point.
        decltype(amd_comgr_get_metadata_map_size)*                  amd_comgr_get_metadata_map_size_fn;                   ///< comgr library entry point.
        decltype(amd_comgr_iterate_map_metadata)*                   amd_comgr_iterate_map_metadata_fn;                    ///< comgr library entry point.
        decltype(amd_comgr_metadata_lookup)*                        amd_comgr_metadata_lookup_fn;                         ///< comgr library entry point.
        decltype(amd_comgr_get_metadata_list_size)*                 amd_comgr_get_metadata_list_size_fn;                  ///< comgr library entry point.
        decltype(amd_comgr_index_list_metadata)*                    amd_comgr_index_list_metadata_fn;                     ///< comgr library entry point.
        decltype(amd_comgr_iterate_symbols)*                        amd_comgr_iterate_symbols_fn;                         ///< comgr library entry point.
        decltype(amd_comgr_symbol_lookup)*                          amd_comgr_symbol_lookup_fn;                           ///< comgr library entry point.
        decltype(amd_comgr_symbol_get_info)*                        amd_comgr_symbol_get_info_fn;                         ///< comgr library entry point.
        decltype(amd_comgr_demangle_symbol_name)*                   amd_comgr_demangle_symbol_name_fn;                    ///< comgr library entry point.

        /// @brief Gets the static singleton instance.
        ///
        /// @return The singleton instance.
        static ComgrEntryPoints* Instance()
        {
            std::call_once(init_instance_flag_, InitInstance);

            return instance_;
        }

        /// @brief Deletes the static singleton instance.
        static void DeleteInstance()
        {
            if (nullptr != instance_)
            {
                ComgrEntryPoints* copy_of_instance = instance_;
                instance_                          = nullptr;
                delete copy_of_instance;
            }
        }

        /// @brief Indicates if the comgr library entry points are valid.
        ///
        /// Always true when COMGR_DYNAMIC_LINKING is not defined.
        ///
        /// @return true if the comgr entry points are valid.
        bool EntryPointsValid()
        {
            return entry_points_valid_;
        }

    private:
        bool entry_points_valid_ = true;  ///< Flag indicating if the comgr library entry points are valid.

#ifdef COMGR_DYNAMIC_LINKING

#ifdef _WIN32
        HMODULE module_;  ///< The comgr library module handle.
#else
        void* module_;  ///< The comgr library module handle.
#endif

        /// @brief Attempts to initialize the specified comgr library entry point
        ///
        /// @param [in] entry_point_name The name of the entry point to initialize.
        ///
        /// @return The address of the entry point or nullptr if the entry point could not be initialized.
        void* InitEntryPoint(const char* entry_point_name)
        {
            if (nullptr != module_)
            {
#ifdef _WIN32
                return GetProcAddress(module_, entry_point_name);
#else
                return dlsym(module_, entry_point_name);
#endif
            }

            return nullptr;
        }
#endif

        /// @brief Private constructor.
        ComgrEntryPoints()
        {
#ifdef COMGR_DYNAMIC_LINKING
#ifdef _WIN32
            // Resolve the path of the module (DLL or EXE) containing this code,
            // so we can load amd_comgr_3.dll from the same directory.
            HMODULE this_module = nullptr;
            if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                  reinterpret_cast<LPCTSTR>(&ComgrEntryPoints::init_instance_flag_),
                                  &this_module))
            {
                TCHAR dll_path[MAX_PATH] = {0};
                DWORD len                = GetModuleFileName(this_module, dll_path, MAX_PATH);
                if (len > 0 && len < MAX_PATH)
                {
                    // Remove the DLL name to get the directory
                    TCHAR* last_backslash = _tcsrchr(dll_path, _T('\\'));
                    if (last_backslash)
                    {
                        *(last_backslash + 1) = _T('\0');
                    }

                    // Append the comgr DLL name
                    _tcscat_s(dll_path, MAX_PATH, _T("amd_comgr_3.dll"));
                    module_ = LoadLibrary(dll_path);
                }
            }
#else
            // Use bare filename so the dynamic linker's standard search order
            // (including RPATH set on the consuming executable) can locate the library.
            module_ = dlopen("libamd_comgr.so.3.0.0", RTLD_LAZY);
#endif
#define INIT_COMGR_ENTRY_POINT(func)                          \
    reinterpret_cast<decltype(func)*>(InitEntryPoint(#func)); \
    entry_points_valid_ &= nullptr != func##_fn
#else
#define INIT_COMGR_ENTRY_POINT(func) func
#endif
            amd_comgr_status_string_fn                          = INIT_COMGR_ENTRY_POINT(amd_comgr_status_string);
            amd_comgr_get_version_fn                            = INIT_COMGR_ENTRY_POINT(amd_comgr_get_version);
            amd_comgr_get_isa_count_fn                          = INIT_COMGR_ENTRY_POINT(amd_comgr_get_isa_count);
            amd_comgr_get_isa_name_fn                           = INIT_COMGR_ENTRY_POINT(amd_comgr_get_isa_name);
            amd_comgr_get_isa_metadata_fn                       = INIT_COMGR_ENTRY_POINT(amd_comgr_get_isa_metadata);
            amd_comgr_create_data_fn                            = INIT_COMGR_ENTRY_POINT(amd_comgr_create_data);
            amd_comgr_release_data_fn                           = INIT_COMGR_ENTRY_POINT(amd_comgr_release_data);
            amd_comgr_get_data_kind_fn                          = INIT_COMGR_ENTRY_POINT(amd_comgr_get_data_kind);
            amd_comgr_set_data_fn                               = INIT_COMGR_ENTRY_POINT(amd_comgr_set_data);
            amd_comgr_set_data_name_fn                          = INIT_COMGR_ENTRY_POINT(amd_comgr_set_data_name);
            amd_comgr_get_data_fn                               = INIT_COMGR_ENTRY_POINT(amd_comgr_get_data);
            amd_comgr_get_data_name_fn                          = INIT_COMGR_ENTRY_POINT(amd_comgr_get_data_name);
            amd_comgr_get_data_isa_name_fn                      = INIT_COMGR_ENTRY_POINT(amd_comgr_get_data_isa_name);
            amd_comgr_get_data_metadata_fn                      = INIT_COMGR_ENTRY_POINT(amd_comgr_get_data_metadata);
            amd_comgr_destroy_metadata_fn                       = INIT_COMGR_ENTRY_POINT(amd_comgr_destroy_metadata);
            amd_comgr_create_data_set_fn                        = INIT_COMGR_ENTRY_POINT(amd_comgr_create_data_set);
            amd_comgr_destroy_data_set_fn                       = INIT_COMGR_ENTRY_POINT(amd_comgr_destroy_data_set);
            amd_comgr_data_set_add_fn                           = INIT_COMGR_ENTRY_POINT(amd_comgr_data_set_add);
            amd_comgr_data_set_remove_fn                        = INIT_COMGR_ENTRY_POINT(amd_comgr_data_set_remove);
            amd_comgr_action_data_count_fn                      = INIT_COMGR_ENTRY_POINT(amd_comgr_action_data_count);
            amd_comgr_action_data_get_data_fn                   = INIT_COMGR_ENTRY_POINT(amd_comgr_action_data_get_data);
            amd_comgr_create_action_info_fn                     = INIT_COMGR_ENTRY_POINT(amd_comgr_create_action_info);
            amd_comgr_destroy_action_info_fn                    = INIT_COMGR_ENTRY_POINT(amd_comgr_destroy_action_info);
            amd_comgr_action_info_set_isa_name_fn               = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_set_isa_name);
            amd_comgr_action_info_get_isa_name_fn               = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_get_isa_name);
            amd_comgr_action_info_set_language_fn               = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_set_language);
            amd_comgr_action_info_get_language_fn               = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_get_language);
            amd_comgr_action_info_set_option_list_fn            = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_set_option_list);
            amd_comgr_action_info_get_option_list_count_fn      = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_get_option_list_count);
            amd_comgr_action_info_get_option_list_item_fn       = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_get_option_list_item);
            amd_comgr_action_info_set_working_directory_path_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_set_working_directory_path);
            amd_comgr_action_info_get_working_directory_path_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_get_working_directory_path);
            amd_comgr_action_info_set_logging_fn                = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_set_logging);
            amd_comgr_action_info_get_logging_fn                = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_get_logging);
            amd_comgr_do_action_fn                              = INIT_COMGR_ENTRY_POINT(amd_comgr_do_action);
            amd_comgr_get_metadata_kind_fn                      = INIT_COMGR_ENTRY_POINT(amd_comgr_get_metadata_kind);
            amd_comgr_get_metadata_string_fn                    = INIT_COMGR_ENTRY_POINT(amd_comgr_get_metadata_string);
            amd_comgr_get_metadata_map_size_fn                  = INIT_COMGR_ENTRY_POINT(amd_comgr_get_metadata_map_size);
            amd_comgr_iterate_map_metadata_fn                   = INIT_COMGR_ENTRY_POINT(amd_comgr_iterate_map_metadata);
            amd_comgr_metadata_lookup_fn                        = INIT_COMGR_ENTRY_POINT(amd_comgr_metadata_lookup);
            amd_comgr_get_metadata_list_size_fn                 = INIT_COMGR_ENTRY_POINT(amd_comgr_get_metadata_list_size);
            amd_comgr_index_list_metadata_fn                    = INIT_COMGR_ENTRY_POINT(amd_comgr_index_list_metadata);
            amd_comgr_iterate_symbols_fn                        = INIT_COMGR_ENTRY_POINT(amd_comgr_iterate_symbols);
            amd_comgr_symbol_lookup_fn                          = INIT_COMGR_ENTRY_POINT(amd_comgr_symbol_lookup);
            amd_comgr_symbol_get_info_fn                        = INIT_COMGR_ENTRY_POINT(amd_comgr_symbol_get_info);
            amd_comgr_demangle_symbol_name_fn                   = INIT_COMGR_ENTRY_POINT(amd_comgr_demangle_symbol_name);
#undef INIT_COMGR_ENTRY_POINT
        }

        /// Destructor
        virtual ~ComgrEntryPoints()
        {
#ifdef COMGR_DYNAMIC_LINKING
            if (nullptr != module_)
            {
#ifdef _WIN32
                FreeLibrary(module_);
#else
                dlclose(module_);
#endif
            }
#endif
            DeleteInstance();
        }

    private:
        static ComgrEntryPoints* instance_;            ///< Static singleton instance.
        static std::once_flag    init_instance_flag_;  ///< Static instance init flag for multithreading.

        /// @brief Create the singleton instance.
        static void InitInstance()
        {
            instance_ = new ComgrEntryPoints;
        }
    };

    /// @brief PAL pipeline version struct.
    struct PalPipelineVersion
    {
        uint32_t major_version;  ///< Version major.
        uint32_t minor_version;  ///< Version minor.

        /// @brief Default constructor
        PalPipelineVersion()
            : major_version(0)
            , minor_version(0)
        {
        }
    };

    /// @brief HSA pipeline version struct.
    struct HsaKernelVersion
    {
        uint32_t major_version;  ///< Version major.
        uint32_t minor_version;  ///< Version minor.

        /// @brief Default constructor.
        HsaKernelVersion()
            : major_version(0)
            , minor_version(0)
        {
        }
    };

    /// @brief The error status.
    enum ComgrUtilsStatus
    {
        kComgrUtilsStatusSuccess = 0,           ///< The operation completed successfully.
        kComgrUtilsStatusError,                 ///< The operation failed.
        kComgrUtilsStatusInvalidShaderInfo,     ///< The operation failed due to invalid shader info.
        kComgrUtilsStatusInvalidHwStageInfo,    ///< The operation failed due to invalid hardware stage info.
        kComgrUtilsStatusInvalidRegisterInfo,   ///< The operation failed due to invalid register info.
        kComgrUtilsStatusInvalidPipelineInfo,   ///< The operation failed due to invalid pipeline info.
        kComgrUtilsStatusInvalidHsaKernelInfo,  ///< The operation failed due to invalid HSA kernel info.
        kComgrUtilsStatusInvalidSymbolInfo      ///< The operation failed due to invalid symbol info.
    };

    /// @brief API level shader types.
    enum ShaderInfoType
    {
        kVertexShader = 0,  ///< Vertex shader.
        kHullShader,        ///< Hull shader.
        kDomainShader,      ///< Domain shader.
        kGeometryShader,    ///< Geometry shader.
        kPixelShader,       ///< Pixel shader.
        kComputeShader,     ///< Compute shader.
        kMeshShader,        ///< Mesh shader.
        kTaskShader,        ///< Task shader.
        kShaderTypeCount    ///< Count.
    };

    /// @brief Shader info struct.
    struct ShaderInfo
    {
        ShaderInfoType shader_type;                 ///< Shader type.
        uint64_t       api_shader_hash_lo;          ///< 128 bit hash lower bits.
        uint64_t       api_shader_hash_hi;          ///< 128 bit hash higher bits.
        uint16_t       hardware_mapping_bit_field;  ///< HW Mapping.
        uint32_t       num_hardware_mappings;       ///< Number of HW mappings.
        char*          shader_subtype;              ///< Shader function subtype.
    };

    /// @brief Shader function info struct.
    struct ShaderFunctionInfo
    {
        uint64_t api_shader_hash_lo;         ///< 128 bit hash lower bits.
        uint64_t api_shader_hash_hi;         ///< 128 bit hash higher bits.
        char*    name;                       ///< Shader function name.
        uint64_t lds_size;                   ///< Local data share size in bytes.
        uint64_t scratch_memory_size;        ///< Scratch memory size in bytes.
        uint64_t sgpr_count;                 ///< SGPR count.
        uint64_t vgpr_count;                 ///< VGPR count.
        uint64_t stack_frame_size_in_bytes;  ///< Stack frame size in bytes.
        char*    shader_subtype;             ///< Shader function subtype.
    };

    struct PaClClipCntlMetadata
    {
        /// Whether User Clip Plane 0 is enabled.
        uint16_t user_clip_plane_0_ena : 1;
        /// Whether User Clip Plane 1 is enabled.
        uint16_t user_clip_plane_1_ena : 1;
        /// Whether User Clip Plane 2 is enabled.
        uint16_t user_clip_plane_2_ena : 1;
        /// Whether User Clip Plane 3 is enabled.
        uint16_t user_clip_plane_3_ena : 1;
        /// Whether User Clip Plane 4 is enabled.
        uint16_t user_clip_plane_4_ena : 1;
        /// Whether User Clip Plane 5 is enabled.
        uint16_t user_clip_plane_5_ena : 1;
        /// Whether the clipper performs special t_factor adjustment from DX10 to calculate the attribute
        /// barycentric coordinates to allow for linear gradient appearance across clipped triangle fan. If reset,
        /// vertices will use perspective correct barycentric coordinates.
        uint16_t dx_linear_attr_clip_ena : 1;
        /// Whether depth range is zero to one.
        uint16_t dx_clip_space_def : 1;
        /// Whether depth near clipping is disabled.
        uint16_t z_clip_near_disable : 1;
        /// Whether depth far clipping is disabled.
        uint16_t z_clip_far_disable : 1;
        /// Whether rasterization kill is enabled.
        uint16_t rasterization_kill : 1;
        /// Whether clipping is disabled. Must be set if the VS ouputs window coordinates.
        uint16_t clip_disable : 1;
    };

    struct PaClVteCntlMetadata
    {
        /// Indicates that the incoming X, Y have already been multiplied by 1/W0. Must be set if the vertex shader
        /// outputs window coordinates.
        uint16_t vtx_xy_fmt : 1;
        /// Indicates that the incoming Z has already been multiplied by 1/W0. Must be set if the vertex shader
        /// outputs window coordinates.
        uint16_t vtx_z_fmt : 1;
        /// Whether the Viewport Transform performs scaling on the X component. Must be false if the vertex shader
        /// outputs window coordinates.
        uint16_t x_scale_ena : 1;
        /// Whether the Viewport Transform adds the offset on the X component. Must be false if the vertex shader
        /// outputs window coordinates.
        uint16_t x_offset_ena : 1;
        /// Whether the Viewport Transform performs scaling on the Y component. Must be false if the vertex shader
        /// outputs window coordinates.
        uint16_t y_scale_ena : 1;
        /// Whether the Viewport Transform adds the offset on the Y component. Must be false if the vertex shader
        /// outputs window coordinates.
        uint16_t y_offset_ena : 1;
        /// Whether the Viewport Transform performs scaling on the Z component. Must be false if the vertex shader
        /// outputs window coordinates.
        uint16_t z_scale_ena : 1;
        /// Whether the Viewport Transform adds the offset on the Z component. Must be false if the vertex shader
        /// outputs window coordinates.
        uint16_t z_offset_ena : 1;
        /// Indicates that the incoming W0 is not 1/W0. Must be false if the vertex shader outputs window
        /// coordinates.
        uint16_t vtx_w0_fmt : 1;
    };

    struct PaSuVtxCntlMetadata
    {
        /// Controls conversion of X,Y coordinates from IEEE to fixed-point - 0 = Truncate - 1 = Round - 2 = Round to Even -
        /// 3 = Round to Odd
        uint8_t round_mode;
        /// Controls conversion of X,Y coordinates from IEEE to fixed-point.
        /// Determines fixed point format and how many fractional bits are actually utilized.
        /// Modes 0-4 are not supported when conservative rasterization is enabled.
        /// - 0 = 16.8 fixed point. 1/16th ( 4 fractional bits used)
        /// - 1 = 16.8 fixed point. 1/8th ( 3 fractional bits used)
        /// - 2 = 16.8 fixed point. 1/4th ( 2 fractional bits used)
        /// - 3 = 16.8 fixed point. 1/2 ( 1 fractional bit used)
        /// - 4 = 16.8 fixed point. 1 ( 0 fractional bits used)
        /// - 5 = 16.8 fixed point. 1/256th ( 8 fractional bits)
        /// - 6 = 14.10 fixed point. 1/1024th (10 fractional bits)
        /// - 7 = 12.12 fixed point. 1/4096th (12 fractional bits)
        uint8_t quant_mode;
        /// Specifies where the pixel center of the incoming vertex is. The drawing engine itself has pixel centers
        /// @ 0.5, so if this bit is `0`, 0.5 will be added to the X,Y coordinates to move the incoming vertex onto
        /// our internal grid.
        uint8_t pix_center : 1;
    };

    struct VgtShaderStagesEnMetadata
    {
        /// Whether the ES stage is enabled.
        /// - 0 - ES stage off.
        /// - 1 - ES stage on, and the ES is a Domain shader.
        /// - 2 - ES stage on, and the ES is a Vertex shader.
        uint8_t es_stage_en;
        /// Whether the VS stage is enabled.
        /// - 0 - VS stage is on, and is an API Vertex Shader.
        /// - 1 - VS stage is on, and is an API Domain Shader.
        /// - 2 - VS stage is on, and is a copy shader.
        uint8_t vs_stage_en;
        /// Maximum number of primgroups that can be combined into a single ES or VS wave.
        uint8_t max_prim_group_in_wave;
        /// Whether NGG subgroups should be launched in a different mode, possibly at a faster rate.
        uint8_t gs_fast_launch;

        uint16_t ls_stage_en : 1;
        /// Whether the HS stage is enabled.
        uint16_t hs_stage_en : 1;
        /// Whether the GS stage is enabled.
        uint16_t gs_stage_en : 1;
        /// Whether the output of the HS stage stays on chip or whether it is dynamically decided to use offchip.
        uint16_t dynamic_hs : 1;
        /// Whether or not Next Generation Geometry (Prim Shader) is enabled.
        uint16_t prim_gen_en : 1;
        /// Whether the ordered wave id for the primitive shader is created per sub-group or per wave.
        /// - false - WaveId per sub-group
        /// - true  - WaveId per wave
        uint16_t ordered_id_mode : 1;
        /// Whether the NGG wave ID will be incremented.
        uint16_t ngg_wave_id_en : 1;
        /// Whether the NGG pipeline is run in passthrough mode.
        uint16_t prim_gen_passthru_en : 1;
        /// When the NGG pipeline is in passthrough mode, whether or not the shader must send the allocation
        /// message.
        uint16_t prim_gen_passthru_no_msg : 1;
    };

    struct VgtGsModeMetadata
    {
        /// Which GS scenario to enable.
        uint8_t mode;
        /// On-chip mode for ESGS and GSVS communication.
        /// - 0 - EsGs and GsVs data is offchip.
        /// - 1 - GsVs data is offchip.
        /// - 3 - EsGs and GsVs data in onchip.
        uint8_t on_chip;
        /// Cut length, dependent on how many vertices the GS emits.
        /// - 0 - More than 512 GS emit vertices.
        /// - 1 - More than 256 GS emit vertices and less than equal to 512.
        /// - 2 - More than 128 GS emit vertices and less than equal to 256.
        /// - 3 - Less than equal to 128 GS emit vertices.
        uint8_t cut_mode;

        /// Whether the ESGS ring is optimized for write combining.
        uint8_t es_write_optimize : 1;
        /// Whether the GSVS ring is optimized for write combining.
        uint8_t gs_write_optimize : 1;
    };

    struct VgtTfParamMetadata
    {
        /// Tessellation type.
        /// - 0 - Isoline
        /// - 1 - Triangle
        /// - 2 - Quad
        uint8_t type;
        /// Partition type.
        /// - 0 - Integer
        /// - 1 - Pow2
        /// - 2 - Fractional Odd
        /// - 3 - Fractional Even
        uint8_t partitioning;
        /// Output primitive topology.
        /// - 0 - Point
        /// - 1 - Line
        /// - 2 - Triangle Clockwise
        /// - 3 - Triangle Counter-clockwise
        uint8_t topology;
        /// How many DS waves (ES/VS) are sent to the same SIMD before spilling to other SIMDs to use the offchip LDS data
        uint8_t num_ds_waves_per_simd;
        /// Mode used for distributed tessellation.
        /// Requires offchip tessellation to be enabled for PATCHES and DONUT modes of distribution.
        /// - 0 - No distribution.
        /// - 1 - Patches
        /// - 2 - Donuts
        /// - 3 - Trapezoids
        uint8_t distribution_mode;
        /// Whether to disable donut walking pattern is used by the tessellator.
        uint8_t disable_donuts : 1;
    };

    struct VgtLsHsConfigMetadata
    {
        /// Number of patches in a threadgroup. Max verts/threadgroup is 256.
        uint8_t num_patches;
        /// Number of control points in HS input patch. Valid range is 1-32.
        uint8_t hs_num_input_cp;
        /// Number of control points in HS output patch. Valid range is 1-32.
        uint8_t hs_num_output_cp;
    };

    struct IaMultiVgtParamMetadata
    {
        /// Number of primitives sent to one of the frontends before switching to the next frontend. Implied +1.
        uint16_t prim_group_size;

        /// Whether the frontend will issue a VS wave as soon as a primgroup is finished, or if it will continue a
        /// VS wave from one primgroup into the next within a draw call.
        uint8_t partial_vs_wave_on : 1;
        /// Whether the frontend will issue an ES wave as soon as a primgroup is finished, or if it will continue a
        /// ES wave from one primgroup into the next within a draw call.
        uint8_t partial_es_wave_on : 1;
        /// Whether the overall frontend will switch between frontends at packet boundaries, otherwise will switch
        /// based on size of primgroups.
        uint8_t switch_on_eop : 1;
        /// Whether the overall frontend will switch between frontends at instance boundaries, otherwise will switch
        /// based on size of primgroups.
        uint8_t switch_on_eoi : 1;
    };

    /// @brief Point sprite override selection.
    enum PointSpriteSelect
    {
        kComgrUtilsZero,  ///< Select 0.0f.
        kComgrUtilsOne,   ///< Select 1.0f.
        kComgrUtilsS,     ///< Select S component value.
        kComgrUtilsT,     ///< Select T component value.
        kComgrUtilsNone,  ///< Keep interpolated result.
    };

    struct SpiInterpControlMetadata
    {
        PointSpriteSelect point_sprite_override_x;

        PointSpriteSelect point_sprite_override_y;

        PointSpriteSelect point_sprite_override_z;

        PointSpriteSelect point_sprite_override_w;

        /// Global flat shade enable used in conjunction with per-parameter flat shader control.
        uint8_t flat_shade_en : 1;
        /// Enable point sprite override for point primitives.
        uint8_t point_sprite_ena : 1;
    };

    struct SpiPsInputCntlMetadata
    {
        /// PS input offset – specifies which parameter cache outputs are for this input.
        uint8_t offset;
        /// Selects default value if no semantic match is found.
        uint8_t default_val;
        /// Cylindrical wrap control.
        uint8_t cyl_wrap;
        /// Flat shade select. Set if interpolation mode is constant.
        uint8_t flat_shade : 1;
        /// Whether this parameter should be overridden with texture coordinates if global point sprite enable is
        /// set.
        uint8_t pt_sprite_tex : 1;
        /// Specifies that up to two parameters are interpolated in FP16 mode and loaded as an FP16 pair in the PS
        /// input GPR.
        uint8_t fp16_interp_mode : 1;
        /// Whether the first FP16 parameter is valid. Only valid if fp16_interp_mode is set.
        uint8_t attr_0_valid : 1;
        /// Whether the second FP16 parameter is valid. Only valid if fp16_interp_mode is set.
        uint8_t attr_1_valid : 1;
        /// Whether the hardware will provide provoking vertex ID and rotate the raw attribute parameter cache
        /// pointers accordingly.
        uint8_t rotate_pc_ptr : 1;
        /// Whether this parameter is a primitive attribute.
        uint8_t prim_attr : 1;
    };

    struct SpiShaderGsMeshletDimMetadata
    {
        /// Threadgroup size in the X dimension.
        uint16_t num_thread_x;
        /// Threadgroup size in the Y dimension.
        uint16_t num_thread_y;
        /// Threadgroup size in the Z dimension.
        uint16_t num_thread_z;
        /// Threadgroup size (X * Y * Z).
        uint32_t threadgroup_size;
    };

    struct SpiShaderGsMeshletExpAllocMetadata
    {
        /// Maximum position export space per meshlet subgroup.
        uint16_t max_exp_verts;
        /// Maximum primitive export space per meshlet subgroup.
        uint16_t max_exp_prims;
    };

    struct VgtGsInstanceCntMetadata
    {
        /// Number of GS primitive instances. If set to 0, GS instancing is treated as disabled.
        uint8_t count;
        /// Whether or not GS instancing is enabled.
        uint8_t enable : 1;
        /// Allows each GS instance to emit max_vert_out.
        uint8_t en_max_vert_out_per_gs_instance : 1;
    };

    struct GeNggSubgrpCntlMetadata
    {
        /// Controls the maximum amplification factor applied to each primitive in a subgroup.
        uint16_t prim_amp_factor;
        /// Controls the number of threads launched per subgroup in NGG fast launch mode.
        uint16_t threads_per_subgroup;
    };

    struct VgtGsOnchipCntlMetadata
    {
        /// Worst case number of ES vertices needed to create the GS prims specified in gs_prims_per_subgroup.
        uint16_t es_verts_per_subgroup;
        /// Number of GS primitives that can fit into LDS.
        uint16_t gs_prims_per_subgroup;
        /// Total number of GS primitives taking into account GS instancing.
        uint16_t gs_inst_prims_per_subgrp;
    };

    struct PaClVsOutCntlMetadata
    {
        /// Enable ClipDistance 0 to be used for user-defined clipping.
        uint32_t clip_dist_ena_0 : 1;
        /// Enable ClipDistance 1 to be used for user-defined clipping.
        uint32_t clip_dist_ena_1 : 1;
        /// Enable ClipDistance 2 to be used for user-defined clipping.
        uint32_t clip_dist_ena_2 : 1;
        /// Enable ClipDistance 3 to be used for user-defined clipping.
        uint32_t clip_dist_ena_3 : 1;
        /// Enable ClipDistance 4 to be used for user-defined clipping.
        uint32_t clip_dist_ena_4 : 1;
        /// Enable ClipDistance 5 to be used for user-defined clipping.
        uint32_t clip_dist_ena_5 : 1;
        /// Enable ClipDistance 6 to be used for user-defined clipping.
        uint32_t clip_dist_ena_6 : 1;
        /// Enable ClipDistance 7 to be used for user-defined clipping.
        uint32_t clip_dist_ena_7 : 1;
        /// Enable CullDistance 0 to be used for user-defined clip discard.
        uint32_t cull_dist_ena_0 : 1;
        /// Enable CullDistance 1 to be used for user-defined clip discard.
        uint32_t cull_dist_ena_1 : 1;
        /// Enable CullDistance 2 to be used for user-defined clip discard.
        uint32_t cull_dist_ena_2 : 1;
        /// Enable CullDistance 3 to be used for user-defined clip discard.
        uint32_t cull_dist_ena_3 : 1;
        /// Enable CullDistance 4 to be used for user-defined clip discard.
        uint32_t cull_dist_ena_4 : 1;
        /// Enable CullDistance 5 to be used for user-defined clip discard.
        uint32_t cull_dist_ena_5 : 1;
        /// Enable CullDistance 6 to be used for user-defined clip discard.
        uint32_t cull_dist_ena_6 : 1;
        /// Enable CullDistance 7 to be used for user-defined clip discard.
        uint32_t cull_dist_ena_7 : 1;
        /// Use the PointSize output from the VS.
        uint32_t use_vtx_point_size : 1;
        /// Use the EdgeFlag output from the VS.
        uint32_t use_vtx_edge_flag : 1;
        /// Use the RenderTargetArrayIndex output from the VS.
        uint32_t use_vtx_render_target_indx : 1;
        /// Use the ViewportArrayIndex output from the VS.
        uint32_t use_vtx_viewport_indx : 1;
        /// Use the KillFlag output from the VS.
        uint32_t use_vtx_kill_flag : 1;
        /// Output the VS output misc vector from the VS.
        uint32_t vs_out_misc_vec_ena : 1;
        /// Output the VS output ccdist0 vector from the VS.
        uint32_t vs_out_cc_dist_0_vec_ena : 1;
        /// Output the VS output ccdist1 vector from the VS.
        uint32_t vs_out_cc_dist_1_vec_ena : 1;
        /// Enable performance optimization where SX outputs vs_out_misc_vec data on extra side bus.
        uint32_t vs_out_misc_side_bus_ena : 1;
        /// Use the LineWidth output from the VS.
        uint32_t use_vtx_line_width : 1;
        /// Use the VRS rates output from the VS.
        uint32_t use_vtx_vrs_rate : 1;
        /// Force the vertex rate combiner into bypass mode.
        uint32_t bypass_vtx_rate_combiner : 1;
        /// Force the primitive rate combiner into bypass mode.
        uint32_t bypass_prim_rate_combiner : 1;
        /// Use the FSR select output from the VS.
        uint32_t use_vtx_fsr_select : 1;
    };

    struct SpiVsOutConfigMetadata
    {
        /// Number of vectors exported by the VS.
        uint8_t vs_export_count;
        /// Number of vectors exported by the primitive shader as a primitive attribute.
        uint8_t prim_export_count;
        /// Whether the associated draw's waves or groups will allocate zero parameter cache space.
        uint8_t no_pc_export : 1;
    };

    struct VgtStrmoutConfigMetadata
    {
        /// Stream for which rasterization is enabled.
        uint8_t rast_stream;
        /// Mask indicating which stream is enabled.
        uint8_t rast_stream_mask;
        /// Whether stream output to stream 0 is enabled.
        uint8_t stream_out_0_en : 1;
        /// Whether stream output to stream 1 is enabled.
        uint8_t stream_out_1_en : 1;
        /// Whether stream output to stream 2 is enabled.
        uint8_t stream_out_2_en : 1;
        /// Whether stream output to stream 3 is enabled.
        uint8_t stream_out_3_en : 1;
        /// Whether the hardware will count output prims seen irrespective of streamout enabled.
        uint8_t prims_needed_cnt_en : 1;
        /// Whether rast_stream_mask is valid and should be used, otherwise use rast_stream.
        uint8_t use_rast_stream_mask : 1;
    };

    struct VgtStrmoutBufferConfigMetadata
    {
        /// Mask of which buffers are bound for stream 0.
        uint8_t stream_0_buffer_en;
        /// Mask of which buffers are bound for stream 1.
        uint8_t stream_1_buffer_en;
        /// Mask of which buffers are bound for stream 2.
        uint8_t stream_2_buffer_en;
        /// Mask of which buffers are bound for stream 3.
        uint8_t stream_3_buffer_en;
    };

    struct CbShaderMaskMetadata
    {
        /// 4-bit mask of which color RT0's components are enabled.
        uint8_t output_0_enable;
        /// 4-bit mask of which color RT1's components are enabled.
        uint8_t output_1_enable;
        /// 4-bit mask of which color RT2's components are enabled.
        uint8_t output_2_enable;
        /// 4-bit mask of which color RT3's components are enabled.
        uint8_t output_3_enable;
        /// 4-bit mask of which color RT4's components are enabled.
        uint8_t output_4_enable;
        /// 4-bit mask of which color RT5's components are enabled.
        uint8_t output_5_enable;
        /// 4-bit mask of which color RT6's components are enabled.
        uint8_t output_6_enable;
        /// 4-bit mask of which color RT7's components are enabled.
        uint8_t output_7_enable;
    };

    struct DbShaderControlMetadata
    {
        /// Indicates shader's preference for which type of Z testing.
        uint8_t z_order;
        /// Forces Z exports to be either less than or greater than the source Z value.
        uint8_t conservative_z_export;
        /// Whether to use DB shader export's red channel as Z instead of the interpolated Z value.
        uint16_t z_export_enable : 1;
        /// Whether to use DB shader export's green[7:0] as the stencil test value.
        uint16_t stencil_test_val_export_enable : 1;
        /// Whether to use DB shader export's green [15:8] as the stencil operation value.
        uint16_t stencil_op_val_export_enable : 1;
        /// Whether the shader can kill pixels through texkill.
        uint16_t kill_enable : 1;
        /// Whether to use DB shader export's alpha channel as an independent alpha to mask operation.
        uint16_t coverage_to_mask_en : 1;
        /// Whether to use DB shader export's blue channel as sample mask for pixel.
        uint16_t mask_export_enable : 1;
        /// Will execute the shader even if hierarchical Z or Stencil would kill the quad.
        uint16_t exec_on_hier_fail : 1;
        /// Will execute the shader even if nothing uses the shader's color or depth exports.
        uint16_t exec_on_noop : 1;
        /// Whether to disable alpha to mask.
        uint16_t alpha_to_mask_disable : 1;
        /// Whether the shader is declared after to run after depth by definition.
        uint16_t depth_before_shader : 1;
        /// Enables primitive ordered pixel shader.
        uint16_t primitive_ordered_pixel_shader : 1;
        /// If sample_coverage_ena is set, override the pre-culling sample coverage mask.
        uint16_t pre_shader_depth_coverage_enable : 1;
    };

    struct SpiPsInControlMetadata
    {
        /// Number of vertex parameters to interpolate.
        uint8_t num_interps;
        /// Number of primitive parameters to interpolate.
        uint8_t num_prim_interp;

        /// Whether to generate gradients for ST coordinates.
        uint8_t param_gen : 1;
        /// Indicates that attribute data was written offchip.
        uint8_t offchip_param_en : 1;
        /// Indicates PS wave controller should wait until after a wave has completed before acting on a dealloc.
        uint8_t late_pc_dealloc : 1;
        /// Disable barycentric optimization which only transfers one set of I, J values when center equals
        /// centroid.
        uint8_t bc_optimize_disable : 1;
    };

    struct PaScShaderControlMetadata
    {
        /// If next available quad falls outside tile aligned region of size specified here, the scan converter will force
        /// end of vector.
        uint8_t wave_break_region_size;
        /// Enables loading of POPS overlay term into an SGPR.
        uint8_t load_collision_wave_id : 1;
        /// Enables loading of POPS intrawave collision term into an SGPR.
        uint8_t load_intrawave_collision : 1;
    };

    struct SpiBarycCntlMetadata
    {
        /// Per-pixel floating point position (at center, centroid, or iterated sample).
        uint8_t pos_float_location;
        /// Scaling resolution for X dimension.
        uint8_t pos_x_scaling;

        /// Scaling resolution for Y dimension.
        uint8_t pos_y_scaling;

        /// Whether to use the entire 32b value to determine front-facing.
        uint8_t front_face_all_bits : 1;

        /// Whether to enable screen x/y position scaling.
        uint8_t pos_scaling_enable : 1;
    };

    struct SpiPsInputMetadata
    {
        /// Whether perspective gradients at sample are enabled.
        uint16_t persp_sample_ena : 1;
        /// Whether perspective gradients at center are enabled.
        uint16_t persp_center_ena : 1;
        /// Whether perspective gradients at centroid are enabled.
        uint16_t persp_centroid_ena : 1;
        /// Whether to provide I, J, 1/W to VGPR for pull model interpolation.
        uint16_t persp_pull_model_ena : 1;
        /// Whether linear gradients at sample are enabled.
        uint16_t linear_sample_ena : 1;
        /// Whether linear gradients at center are enabled.
        uint16_t linear_center_ena : 1;
        /// Whether linear gradients at centroid are enabled.
        uint16_t linear_centroid_ena : 1;
        /// Whether line stipple texture generation, per pixel calculation, and VGPR are loaded.
        uint16_t line_stipple_tex_ena : 1;
        /// Whether per-pixel floating point X position is enabled.
        uint16_t pos_x_float_ena : 1;
        /// Whether per-pixel floating point Y position is enabled.
        uint16_t pos_y_float_ena : 1;
        /// Whether per-pixel floating point Z position is enabled.
        uint16_t pos_z_float_ena : 1;
        /// Whether per-pixel floating point W position is enabled.
        uint16_t pos_w_float_ena : 1;
        /// Whether front face is enabled.
        uint16_t front_face_ena : 1;
        /// Whether ancillary data, including render target array index, iterated sample number, and primitive type
        /// are enabled.
        uint16_t ancillary_ena : 1;
        /// Whether sample coverage is enabled.
        uint16_t sample_coverage_ena : 1;
        /// Whether per-pixel fixed point position is enabled.
        uint16_t pos_fixed_pt_ena : 1;
    };

    struct SpiShaderColFormatMetadata
    {
        /// Specifies the format of color export 0.
        /// - 0 - No exports done
        /// - 1 - Can be FP32 or SINT32/UINT32 R Component
        /// - 2 - Can be FP32 or SINT32/UINT32 GR components
        /// - 3 - Can be FP32 or SINT32/UINT32 AR Components
        /// - 4 - FP16 ABGR Components
        /// - 5 - UNORM16 ABGR Components
        /// - 6 - SNORM16 ABGR Components
        /// - 7 - UINT16 ABGR Components
        /// - 8 - SINT16 ABGR Components
        /// - 9 - Can be FP32 or SINT32/UINT32 ABGR Components
        uint8_t col_0_export_format;
        /// Specifies the format of color export 1.
        /// - 0 - No exports done
        /// - 1 - Can be FP32 or SINT32/UINT32 R Component
        /// - 2 - Can be FP32 or SINT32/UINT32 GR components
        /// - 3 - Can be FP32 or SINT32/UINT32 AR Components
        /// - 4 - FP16 ABGR Components
        /// - 5 - UNORM16 ABGR Components
        /// - 6 - SNORM16 ABGR Components
        /// - 7 - UINT16 ABGR Components
        /// - 8 - SINT16 ABGR Components
        /// - 9 - Can be FP32 or SINT32/UINT32 ABGR Components
        uint8_t col_1_export_format;
        /// Specifies the format of color export 2.
        /// - 0 - No exports done
        /// - 1 - Can be FP32 or SINT32/UINT32 R Component
        /// - 2 - Can be FP32 or SINT32/UINT32 GR components
        /// - 3 - Can be FP32 or SINT32/UINT32 AR Components
        /// - 4 - FP16 ABGR Components
        /// - 5 - UNORM16 ABGR Components
        /// - 6 - SNORM16 ABGR Components
        /// - 7 - UINT16 ABGR Components
        /// - 8 - SINT16 ABGR Components
        /// - 9 - Can be FP32 or SINT32/UINT32 ABGR Components
        uint8_t col_2_export_format;
        /// Specifies the format of color export 3.
        /// - 0 - No exports done
        /// - 1 - Can be FP32 or SINT32/UINT32 R Component
        /// - 2 - Can be FP32 or SINT32/UINT32 GR components
        /// - 3 - Can be FP32 or SINT32/UINT32 AR Components
        /// - 4 - FP16 ABGR Components
        /// - 5 - UNORM16 ABGR Components
        /// - 6 - SNORM16 ABGR Components
        /// - 7 - UINT16 ABGR Components
        /// - 8 - SINT16 ABGR Components
        /// - 9 - Can be FP32 or SINT32/UINT32 ABGR Components
        uint8_t col_3_export_format;
        /// Specifies the format of color export 4.
        /// - 0 - No exports done
        /// - 1 - Can be FP32 or SINT32/UINT32 R Component
        /// - 2 - Can be FP32 or SINT32/UINT32 GR components
        /// - 3 - Can be FP32 or SINT32/UINT32 AR Components
        /// - 4 - FP16 ABGR Components
        /// - 5 - UNORM16 ABGR Components
        /// - 6 - SNORM16 ABGR Components
        /// - 7 - UINT16 ABGR Components
        /// - 8 - SINT16 ABGR Components
        /// - 9 - Can be FP32 or SINT32/UINT32 ABGR Components
        uint8_t col_4_export_format;
        /// Specifies the format of color export 5.
        /// - 0 - No exports done
        /// - 1 - Can be FP32 or SINT32/UINT32 R Component
        /// - 2 - Can be FP32 or SINT32/UINT32 GR components
        /// - 3 - Can be FP32 or SINT32/UINT32 AR Components
        /// - 4 - FP16 ABGR Components
        /// - 5 - UNORM16 ABGR Components
        /// - 6 - SNORM16 ABGR Components
        /// - 7 - UINT16 ABGR Components
        /// - 8 - SINT16 ABGR Components
        /// - 9 - Can be FP32 or SINT32/UINT32 ABGR Components
        uint8_t col_5_export_format;
        /// Specifies the format of color export 6.
        /// - 0 - No exports done
        /// - 1 - Can be FP32 or SINT32/UINT32 R Component
        /// - 2 - Can be FP32 or SINT32/UINT32 GR components
        /// - 3 - Can be FP32 or SINT32/UINT32 AR Components
        /// - 4 - FP16 ABGR Components
        /// - 5 - UNORM16 ABGR Components
        /// - 6 - SNORM16 ABGR Components
        /// - 7 - UINT16 ABGR Components
        /// - 8 - SINT16 ABGR Components
        /// - 9 - Can be FP32 or SINT32/UINT32 ABGR Components
        uint8_t col_6_export_format;
        /// Specifies the format of color export 7.
        /// - 0 - No exports done
        /// - 1 - Can be FP32 or SINT32/UINT32 R Component
        /// - 2 - Can be FP32 or SINT32/UINT32 GR components
        /// - 3 - Can be FP32 or SINT32/UINT32 AR Components
        /// - 4 - FP16 ABGR Components
        /// - 5 - UNORM16 ABGR Components
        /// - 6 - SNORM16 ABGR Components
        /// - 7 - UINT16 ABGR Components
        /// - 8 - SINT16 ABGR Components
        /// - 9 - Can be FP32 or SINT32/UINT32 ABGR Components
        uint8_t col_7_export_format;
    };

    /// @brief Geometry Shader output primitive type.
    enum GsOutPrimType
    {
        kComgrUtilsPointList = 0,  ///< A list of individual vertices that make up points.
        kComgrUtilsLineStrip,      ///< Each additional vertex after the first two makes a new line.
        kComgrUtilsTriStrip,       ///< Each additional vertex after the first three makes a new triangle.
        kComgrUtilsRect2d,         ///< Each rect is the bounding box of an arbitrary 2D triangle.
        kComgrUtilsRectList,       ///< Each rect is three 2D axis-aligned rectangle vertices.
        kComgrUtilsLast,
    };

    /// @brief Specifies how to populate the sample mask provided to pixel shaders.
    enum CoverageToShaderSel
    {
        kComgrUtilsInputCoverage = 0,   ///< In over rasterization mode, replicate the overrast result to all detail samples of
                                        ///  the pixel. In standard rasterization mode, leave the sample mask untouched.
        kComgrUtilsInputInnerCoverage,  ///< In under rasterization mode, replicate the underrast result to all detail samples
                                        ///  of the pixel. If under rasterization is disabled output raw mask.
        kComgrUtilsInputDepthCoverage,  ///< The InputCoverage mask bitwise ANDed with the result of Early Depth/Stencil testing.
        kComgrUtilsRaw,                 ///< Output the scan converter's internal mask, unchanged.
    };

    /// @brief Abstracted compute-only register values.
    struct ComputeRegistersInfo
    {
        bool     dynamic_vgpr_en;  ///< Flag indicating if dynamic VGPR allocation is enabled.
        bool     tg_size_en;       ///< TODO: tg_size_en.
        bool     tgid_x_en;        ///< TODO: tgid_x_en.
        bool     tgid_y_en;        ///< TODO: tgid_y_en.
        bool     tgid_z_en;        ///< TODO: tgid_z_en.
        uint32_t tidig_comp_cnt;   ///< TODO: tidig_comp_cnt.
        uint32_t x_interleave;     ///< TODO: x_interleave.
        uint32_t y_interleave;     ///< TODO: y_interleave.
    };

    /// @brief Abstracted graphics-only register values.
    struct GraphicsRegisterInfo
    {
        /// How many LS VGPR components to load.
        uint8_t ls_vgpr_comp_cnt;
        /// How many ES VGPR components to load.
        uint8_t es_vgpr_comp_cnt;
        /// How many GS VGPR components to load.
        uint8_t gs_vgpr_comp_cnt;
        /// How many VS VGPR components to load.
        uint8_t vs_vgpr_comp_cnt;
        /// Extra LDS size to allocate, in bytes.
        uint32_t ps_extra_lds_size;

        PaClClipCntlMetadata pa_cl_clip_cntl;

        PaClVteCntlMetadata pa_cl_vte_cntl;

        PaSuVtxCntlMetadata pa_su_vtx_cntl;

        VgtShaderStagesEnMetadata vgt_shader_stages_en;

        VgtGsModeMetadata vgt_gs_mode;

        VgtTfParamMetadata vgt_tf_param;

        VgtLsHsConfigMetadata vgt_ls_hs_config;

        IaMultiVgtParamMetadata ia_multi_vgt_param;

        SpiInterpControlMetadata spi_interp_control;

        SpiPsInputCntlMetadata spi_ps_input_cntl[32];
        /// Specifies a minimum tessellation level clamp that is applied to
        /// fetched tessellation factors. Values in the range (0.0, 64.0) are
        /// legal. If the incoming factor is a Nan, a negative number or Zero, it
        /// is not clamped against this value.
        float vgt_hos_min_tess_level;
        /// Specifies a maximum tessellation level clamp that is applied to
        /// fetched tessellation factors. Values in the range (0.0, 64.0) are
        /// legal. If the incoming factor is a Nan, a negative number or Zero, it
        /// is not clamped against this value.
        float vgt_hos_max_tess_level;

        SpiShaderGsMeshletDimMetadata spi_shader_gs_meshlet_dim;

        SpiShaderGsMeshletExpAllocMetadata spi_shader_gs_meshlet_exp_alloc;
        /// Maximum number of verts that can be emitted from a geometry shader.
        uint16_t vgt_gs_max_vert_out;

        VgtGsInstanceCntMetadata vgt_gs_instance_cnt;
        /// EsGs ring item size in dwords.
        uint16_t vgt_es_gs_ring_item_size;
        /// Output primitive type from the geometry shader.
        GsOutPrimType vgt_gs_out_prim_type;
        /// Size of each vertex, in dwords, for the specified stream.
        uint16_t vgt_vs_vert_item_size[4];
        /// Offset of each stream (starting at index 1) from the base.
        uint16_t vgt_gs_ring_offset[3];
        /// Size of each primitive exported by the GS, in dwords.
        uint16_t vgt_gs_vs_ring_item_size;
        /// Maximum number of ES vertices per GS thread.
        uint16_t vgt_es_per_gs;
        /// Maximum number of GS prims per ES thread.
        uint16_t vgt_gs_per_es;
        /// Maximum number of GS threads per VS thread.
        uint16_t vgt_gs_per_vs;
        /// Maximum number of prims exported per subgroup. Expected to be
        /// programmed to gs_inst_prims_per_subgrp * max_vert_out.
        uint16_t max_verts_per_subgroup;
        /// Specifies the format of the primitive export.
        /// - 0 - None
        /// - 1 - 1 Component
        /// - 2 - 2 Components
        /// - 3 - 4 Components, Compressed
        /// - 4 - 4 Components
        uint8_t spi_shader_idx_format;

        GeNggSubgrpCntlMetadata ge_ngg_subgrp_cntl;

        VgtGsOnchipCntlMetadata vgt_gs_on_chip_cntl;

        PaClVsOutCntlMetadata pa_cl_vs_out_cntl;
        /// Specifies the format of the position exports coming out of the shader.
        /// - 0 - None
        /// - 1 - 1 Component
        /// - 2 - 2 Components
        /// - 3 - 4 Components, Compressed
        /// - 4 - 4 Components
        uint8_t spi_shader_pos_format[5];

        SpiVsOutConfigMetadata spi_vs_out_config;

        VgtStrmoutConfigMetadata vgt_strm_out_config;

        VgtStrmoutBufferConfigMetadata vgt_strm_out_buffer_config;
        /// Dword stride between vertices in given stream-out-buffer.
        uint16_t stream_out_vertex_strides[4];

        CbShaderMaskMetadata cb_shader_mask;

        DbShaderControlMetadata db_shader_control;

        SpiPsInControlMetadata spi_ps_in_control;
        /// Specifies ow to populate the sample mask provided to the pixel shader.
        CoverageToShaderSel aa_coverage_to_shader_select;

        PaScShaderControlMetadata pa_sc_shader_control;

        SpiBarycCntlMetadata spi_baryc_cntl;

        SpiPsInputMetadata spi_ps_input;

        SpiShaderColFormatMetadata spi_shader_col_format;
        /// Specifies the format of the depth export.
        /// - 0 - No exports done
        /// - 1 - Can be FP32 or SINT32/UINT32 R Component
        /// - 2 - Can be FP32 or SINT32/UINT32 GR components
        /// - 3 - Can be FP32 or SINT32/UINT32 AR Components
        /// - 4 - FP16 ABGR Components
        /// - 5 - UNORM16 ABGR Components
        /// - 6 - SNORM16 ABGR Components
        /// - 7 - UINT16 ABGR Components
        /// - 8 - SINT16 ABGR Components
        /// - 9 - Can be FP32 or SINT32/UINT32 ABGR Components
        uint8_t spi_shader_z_format;

        /// Enables loading of threadgroup related info into SGPR.
        uint16_t hs_tg_size_en;
        /// Whether to enable loading of streamout base0 into SGPR.
        uint16_t vs_so_base0_en;
        /// Whether to enable loading of streamout base1 into SGPR.
        uint16_t vs_so_base1_en;
        /// Whether to enable loading of streamout base2 into SGPR.
        uint16_t vs_so_base2_en;
        /// Whether to enable loading of streamout base3 into SGPR.
        uint16_t vs_so_base3_en;
        /// Whether to enable loading of streamout buffer config into SGPR.
        uint16_t vs_stream_out_en;
        /// Whether to enable loading of offchip parameter cache base into
        /// SGPR.
        uint16_t vs_pc_base_en;
        /// Whether to enable loading of the PS provoking vertex information
        /// into the SGPR.
        uint16_t ps_load_provoking_vtx;
        /// Whether the HW increments a per-wave count for PS and load the
        /// value into SGPR.
        uint16_t ps_wave_cnt_en;
        /// Enables per-sample (i.e. unique shader-computed value per sample)
        /// pixel shader execution
        uint16_t ps_iter_sample;
        /// Whether vertex reuse in the frontend is disabled.
        uint16_t vgt_reuse_off;
        /// Whether the primitive export contains additional payload.
        uint16_t vgt_draw_prim_payload_en;
        /// Whether primitive ID generation is enabled.
        uint16_t vgt_primitive_id_en;
        /// Whether to disable reuse on provoking vertex in NGG.
        uint16_t ngg_disable_provok_reuse;
    };

    /// @brief Pipeline types.
    enum PipelineType
    {
        kComgrUtilsPipelineTypeCs = 0,       ///< CS pipeline.
        kComgrUtilsPipelineTypeVsPs,         ///< VS & PS pipeline.
        kComgrUtilsPipelineTypeGs,           ///< GS pipeline.
        kComgrUtilsPipelineTypeTess,         ///< TESS pipeline.
        kComgrUtilsPipelineTypeGsTess,       ///< GS & TESS pipeline.
        kComgrUtilsPipelineTypeNgg,          ///< NGG pipeline.
        kComgrUtilsPipelineTypeNggPrim,      ///< NGG & PRIM pipeline.
        kComgrUtilsPipelineTypeNggTess,      ///< NGG & TESS pipeline.
        kComgrUtilsPipelineTypeNggSurfPrim,  ///< NGG & SURF & PRIM pipeline.
        kComgrUtilsPipelineTypeMesh,         ///< MESH pipeline.
        kComgrUtilsPipelineTypeTaskMesh      ///< TASK & MESH pipeline.
    };

    /// @brief Hardware stage type.
    enum HwStageType
    {
        kHwStageEs = 0,  ///< ES hardware stage.
        kHwStageGs,      ///< GS hardware stage.
        kHwStageVs,      ///< VS hardware stage.
        kHwStageHs,      ///< HS hardware stage.
        kHwStageLs,      ///< LS hardware stage.
        kHwStageSs,      ///< NGG hardware stage.
        kHwStagePrimS,   ///< NGG hardware stage.
        kHwStagePs,      ///< PS hardware stage.
        kHwStageCs       ///< CS hardware stage.
    };

    /// @brief Hardware stage info.
    struct HWStageInfo
    {
        HwStageType stage_type;                    ///< Stage type.
        uint32_t    scratch_memory_size;           ///< Scratch mem size.
        uint32_t    local_data_share_size;         ///< Local data share size.
        uint32_t    performance_data_buffer_size;  ///< Performance data buffer size.
        uint32_t    num_used_vgprs;                ///< Number of used VGPRs.
        uint32_t    num_used_sgprs;                ///< Number of used SGPRs.
        uint32_t    wavefront_size;                ///< Wavefront size.
        uint32_t    num_available_vgprs;           ///< Number of available VGPRs.
        uint32_t    num_available_sgprs;           ///< Number of available SGPRs.
        uint32_t    waves_per_group;               ///< Waves per group.
        uint32_t    uses_uavs;                     ///< Uses UAVs.
        uint32_t    uses_rovs;                     ///< Uses ROVs.
        uint32_t    writes_uavs;                   ///< Writes UAVs.
        uint32_t    writes_depth;                  ///< Writes Depth.
        uint32_t    max_prims_per_ps_wave;         ///< 0 for PS (added so we dont need a special type for PS).
        char*       entry_point_symbol_name;       ///< Entry point symbol name.
        uint32_t    user_data_reg_map[32];         ///< User data register map.
        uint32_t    threadgroup_dimensions[3];     ///< Thread group dimensions for dispatches.
    };

    /// @brief RegisterData struct.
    struct RegisterData
    {
        uint32_t address;  ///< Data address.
        uint32_t data;     ///< Register data.
    };

    /// @brief Pipeline struct.
    struct Pipeline
    {
        char*        name;  ///< Null terminated string.
        PipelineType type;  ///< Type of pipeline.

        uint64_t internal_pipeline_hash_lo;  ///< 64 bit hash.
        uint64_t internal_pipeline_hash_hi;  ///< 64 bit hash.
        uint32_t num_shaders;                ///< Num entries in shader_list array below.
        uint32_t num_stages;                 ///< Num entries in stage_list array below.
        uint32_t num_register_writes;        ///< Num entries in register_data_list array below.
        uint32_t num_shader_functions;       ///< Num entries in shader_function_list array below.

        uint32_t user_data_limit;              ///< Limit of user data.
        uint32_t spill_threshold;              ///< Threshold of spill.
        uint32_t uses_viewport_array_index;    ///< View port arrary index.
        uint32_t es_gs_local_data_share_size;  ///< ES and GS local data share size.
        uint32_t scratch_memory_size;          ///< Scratch memory size.
        char*    api_name;                     ///< Api name.
        uint32_t api_create_info;              ///< Api create info.
        uint32_t num_interpolants;             ///< Graphics only, number of PS interpolants.
        bool     uses_cps;                     ///< If the pipeline is using CPS (Continuation Passing Shader).

        ShaderInfo*          shader_list;             ///< Shader list.
        HWStageInfo*         stage_list;              ///< Stage list.
        RegisterData*        register_data_list;      ///< Register data list.
        ShaderFunctionInfo*  shader_function_list;    ///< Shader function list.
        GraphicsRegisterInfo graphics_register_info;  ///< Graphics register info.
        ComputeRegistersInfo compute_registers_info;  ///< Compute register info.
    };

    /// @brief HSA kernel arguments info.
    struct HsaKernelArgInfo
    {
        char*    name;       ///< Argument name.
        uint32_t offset;     ///< Argument offset.
        uint32_t size;       ///< Argument size.
        char*    type_name;  ///< Argument type name.
    };

    /// @brief HSA kernel info.
    struct HsaKernel
    {
        char*             name;                        ///< Kernel name.
        uint32_t          num_kernel_args;             ///< Number of kernel arguments.
        HsaKernelArgInfo* kernel_arg_list;             ///< Kernel arguments list.
        uint32_t          group_segment_fixed_size;    ///< Group segment fixed size.
        uint32_t          kernarg_segment_align;       ///< Kernel arguments segment alignment info.
        uint32_t          kernarg_segment_size;        ///< Kernel arguments segment size.
        uint32_t          max_flat_workgroup_size;     ///< Maximum flat workgroup size.
        uint32_t          private_segment_fixed_size;  ///< Private segment fixed size.
        uint32_t          requested_workgroup_size_x;  ///< Requested workgroup size in X dimension.
        uint32_t          requested_workgroup_size_y;  ///< Requested workgroup size in Y dimension.
        uint32_t          requested_workgroup_size_z;  ///< Requested workgroup size in Z dimension.
        uint32_t          sgpr_count;                  ///< SGPR count.
        uint32_t          vgpr_count;                  ///< VGPR count.
        uint32_t          sgpr_spill_count;            ///< SGPR spill count.
        uint32_t          vgpr_spill_count;            ///< VGPR spill count.
        uint32_t          wavefront_size;              ///< Wavefront size.
    };

    /// @brief PAL pipeline data
    struct PalPipelineData
    {
        PalPipelineVersion version;        ///< PAL version info.
        uint32_t           num_pipelines;  ///< Number of pipelines.
        Pipeline*          pipelines;      ///< The list of pipelines.

        /// @brief Default constructor
        PalPipelineData()
            : version()
            , num_pipelines(0)
            , pipelines(nullptr)
        {
        }
    };

    /// @brief HSA pipeline data.
    struct HsaKernelData
    {
        HsaKernelVersion version;      ///< HSA version info.
        uint32_t         num_kernels;  ///< Number of kernels.
        HsaKernel*       kernels;      ///< The HSA kernels.

        /// @brief Default constructor.
        HsaKernelData()
            : version()
            , num_kernels(0)
            , kernels(nullptr)
        {
        }
    };

    /// @brief Symbol type enum.
    enum CodeObjSymbolType
    {
        kComgrUtilsSymbolTypeNoType = 0,  ///< The symbol's type is not specified.
        kComgrUtilsSymbolTypeObject,      ///< The symbol is associated with a data object, such as a variable, an array, and so on.
        kComgrUtilsSymbolTypeFunc,        ///< The symbol is associated with a function or other executable code.
        kComgrUtilsSymbolTypeSection,     ///< The symbol is associated with a section. Symbol table entries of this type exist primarily for relocation.
        kComgrUtilsSymbolTypeFile,        ///< Conventionally, the symbol's name gives the name of the source file associated with the object file.
        kComgrUtilsSymbolTypeCommon       ///< The symbol labels an uninitialized common block.
    };

    /// @brief Symbol of type function
    struct CodeObjSymbolFunction
    {
        uint64_t symbol_size;   ///< Size of the symbol.
        uint64_t name_len;      ///< String length for the name.
        uint64_t symbol_value;  ///< Symbol value.
        char*    name;          ///< Name of the symbol, null terminated.

        /// @brief Default constructor
        CodeObjSymbolFunction()
            : symbol_size()
            , name_len(0)
            , symbol_value(0)
            , name(nullptr)
        {
        }
    };

    /// @brief Symbol of type section
    struct CodeObjSymbolSection
    {
        uint64_t data;  ///< Data for the symbol section.

        /// @brief Default constructor
        CodeObjSymbolSection()
            : data(0)
        {
        }
    };

    /// @brief Symbols that comgr_utils provides
    struct CodeObjSymbol
    {
        CodeObjSymbolType type;  ///< Type of the code object symbol.
        union
        {
            CodeObjSymbolFunction symbol_function;  ///< Function symbol.
            CodeObjSymbolSection  symbol_section;   ///< Section symbol.
        };

        /// @brief Default constructor
        CodeObjSymbol()
            : type(kComgrUtilsSymbolTypeNoType)
            , symbol_function()
        {
        }
    };

    /// @brief Code object symbols
    struct CodeObjSymbolInfo
    {
        uint32_t       num_symbols;  ///< Number of symbols.
        CodeObjSymbol* symbols;      ///< The symbol list.

        // @brief Default constructor
        CodeObjSymbolInfo()
            : num_symbols(0)
            , symbols(nullptr)
        {
        }
    };

    extern const char* kHsaMdTagKernelVersion;         ///< HSA metadata tag.
    extern const char* kHsaMdTagKernels;               ///< HSA metadata tag.
    extern const char* kHsaMdTagKernelArgs;            ///< HSA metadata tag.
    extern const char* kHsaMdTagGroupSegFixedSize;     ///< HSA metadata tag.
    extern const char* kHsaMdTagKernArgSegAlign;       ///< HSA metadata tag.
    extern const char* kHsaMdTagKernArgSegSize;        ///< HSA metadata tag.
    extern const char* kHsaMdTagMaxFlatWorkgroupSize;  ///< HSA metadata tag.
    extern const char* kHsaMdTagKernalName;            ///< HSA metadata tag.
    extern const char* kHsaMdTagPrivateSegFixedSize;   ///< HSA metadata tag.
    extern const char* kHsaMdTagReqdWorkgroupSize;     ///< HSA metadata tag.
    extern const char* kHsaMdTagSgprCount;             ///< HSA metadata tag.
    extern const char* kHsaMdTagVgprCount;             ///< HSA metadata tag.
    extern const char* kHsaMdTagSgprSpillCount;        ///< HSA metadata tag.
    extern const char* kHsaMdTagVgprSpillCount;        ///< HSA metadata tag.
    extern const char* kHsaMdTagWavefrontSize;         ///< HSA metadata tag.
    extern const char* kHsaMdTagKernelArgName;         ///< HSA metadata tag.
    extern const char* kHsaMdTagKernelArgTypeName;     ///< HSA metadata tag.
    extern const char* kHsaMdTagKernelArgOffset;       ///< HSA metadata tag.
    extern const char* kHsaMdTagKernelArgSize;         ///< HSA metadata tag.

    extern const char* kPalMdTagPipelineVersion;         ///< PAL metadata tag.
    extern const char* kPalMdTagPipelineMajor;           ///< PAL metadata tag.
    extern const char* kPalMdTagPipelineMinor;           ///< PAL metadata tag.
    extern const char* kPalMdTagPipelineName;            ///< PAL metadata tag.
    extern const char* kPalMdTagPipelineType;            ///< PAL metadata tag.
    extern const char* kPalMdTagPipelineHash;            ///< PAL metadata tag.
    extern const char* kPalMdTagPipelines;               ///< PAL metadata tag.
    extern const char* kPalMdTagShaders;                 ///< PAL metadata tag.
    extern const char* kPalMdTagShaderTypeVertex;        ///< PAL metadata tag.
    extern const char* kPalMdTagShaderTypeGeometry;      ///< PAL metadata tag.
    extern const char* kPalMdTagShaderTypePixel;         ///< PAL metadata tag.
    extern const char* kPalMdTagShaderTypeCompute;       ///< PAL metadata tag.
    extern const char* kPalMdTagShaderHash;              ///< PAL metadata tag.
    extern const char* kPalMdTagShaderHardwareMapping;   ///< PAL metadata tag.
    extern const char* kPalMdTagHardwareStages;          ///< PAL metadata tag.
    extern const char* kPalMdTagHardwareStageLs;         ///< PAL metadata tag.
    extern const char* kPalMdTagHardwareStageHs;         ///< PAL metadata tag.
    extern const char* kPalMdTagHardwareStageEs;         ///< PAL metadata tag.
    extern const char* kPalMdTagHardwareStageGs;         ///< PAL metadata tag.
    extern const char* kPalMdTagHardwareStageVs;         ///< PAL metadata tag.
    extern const char* kPalMdTagHardwareStagePs;         ///< PAL metadata tag.
    extern const char* kPalMdTagHardwareStageCs;         ///< PAL metadata tag.
    extern const char* kPalMdTagEntryPointSymbolName;    ///< PAL metadata tag.
    extern const char* kPalMdTagScratchMemorySize;       ///< PAL metadata tag.
    extern const char* kPalMdTagLocalDataShareSize;      ///< PAL metadata tag.
    extern const char* kPalMdTagPerfDataBufferSize;      ///< PAL metadata tag.
    extern const char* kPalMdTagNumUsedVgprs;            ///< PAL metadata tag.
    extern const char* kPalMdTagNumUsedSgprs;            ///< PAL metadata tag.
    extern const char* kPalMdTagNumAvailableVgprs;       ///< PAL metadata tag.
    extern const char* kPalMdTagNumAvailableSgprs;       ///< PAL metadata tag.
    extern const char* kPalMdTagWavesPerGroup;           ///< PAL metadata tag.
    extern const char* kPalMdTagUsesUavs;                ///< PAL metadata tag.
    extern const char* kPalMdTagUsesRovs;                ///< PAL metadata tag.
    extern const char* kPalMdTagWritesUavs;              ///< PAL metadata tag.
    extern const char* kPalMdTagWritesDepth;             ///< PAL metadata tag.
    extern const char* kPalMdTagMaxPrimsPerPsWave;       ///< PAL metadata tag.
    extern const char* kPalMdTagNumInterpolants;         ///< PAL metadata tag.
    extern const char* kPalMdTagRegisters;               ///< PAL metadata tag.
    extern const char* kPalMdTagUserDataLimit;           ///< PAL metadata tag.
    extern const char* kPalMdTagSpillThreshold;          ///< PAL metadata tag.
    extern const char* kPalMdTagUsesViewportArrayIndex;  ///< PAL metadata tag.
    extern const char* kPalMdTagEsGsLocalDataShareSize;  ///< PAL metadata tag.
    extern const char* kPalMdTagWavefrontSize;           ///< PAL metadata tag.
    extern const char* kPalMdTagApi;                     ///< PAL metadata tag.
    extern const char* kPalMdTagApiCreateInfo;           ///< PAL metadata tag.

#define CheckStatusAlt(status, ret_val)     \
    if (status != AMD_COMGR_STATUS_SUCCESS) \
    {                                       \
        return ret_val;                     \
    }

#define CheckStatus(current_code_object_ptr, status, ret_val) \
    if (status != AMD_COMGR_STATUS_SUCCESS)                   \
    {                                                         \
        current_code_object_ptr->SetError(status);            \
        return ret_val;                                       \
    }

#define CheckValid(ret_val) \
    if (!IsValid())         \
    {                       \
        return ret_val;     \
    }

#define Check(bool_val, ret_val) \
    if (!bool_val)               \
    {                            \
        return ret_val;          \
    }

#define ReportInvalidRequiredPalMDMapItem(ITEM) CodeObj::SetError(AMD_COMGR_STATUS_ERROR, "ERROR: Failed to get required MD value:" #ITEM);

    /// @brief Code Object class.
    class CodeObj
    {
        friend class MDNode;

    public:
        /// @brief Open Code Object from a file.
        ///
        /// @param [in] file_name The file name.
        ///
        /// @return The unique_ptr pointing to the Codeobj object.
        static std::unique_ptr<CodeObj> OpenFile(const std::string& file_name);

        /// @brief Open Code Object from a file.
        ///
        /// @param [in] file_name The file name.
        /// @param [in] data_kind The data kind.
        ///
        /// @return The unique_ptr pointing to the Codeobj object.
        static std::unique_ptr<CodeObj> OpenFile(const std::string& file_name, const amd_comgr_data_kind_t& data_kind);

        /// @brief Open Code Object from a memory buffer.
        ///
        /// @param [in] buf The memory buffer.
        ///
        /// @return The unique_ptr pointing to the Codeobj object.
        static std::unique_ptr<CodeObj> OpenBuffer(const std::vector<char>& buf);

        /// @brief Open Code Object from a memory buffer.
        ///
        /// @param [in] buf       The memory buffer.
        /// @param [in] data_kind The data kind.
        ///
        /// @return The unique_ptr pointing to the Codeobj object.
        static std::unique_ptr<CodeObj> OpenBuffer(const std::vector<char>& buf, const amd_comgr_data_kind_t& data_kind);

        /// @brief Open Code Object from a pointer to a char buffer.
        ///
        /// Note: The char buffer should remain allocated while it is being operated upon by comgr_utils
        /// for things like metadata parsing, disassembly and so on.
        ///
        /// @param [in] buf           The memory buffer.
        /// @param [in] size_in_bytes The size of the meory buffer in bytes.
        ///
        /// @return The unique_ptr pointing to the Codeobj object.
        static std::unique_ptr<CodeObj> OpenBufferRaw(const char* buf, size_t size_in_bytes);

        /// Extract Metadata (MD).
        ///
        /// @return The metadata node.
        MDNode GetMD();

        /// @brief Extract the PAL Pipeline metadata and fill the provided structure.
        ///
        /// @param [in] data The PalPipelineData type data.
        ///
        /// @return The error status.
        ComgrUtilsStatus ExtractPalPipelineData(PalPipelineData& data);

        /// @brief Extract the HSA kernel metadata and fill the provided structure.
        ///
        /// @param [in] data The HsaKernelData type data.
        ///
        /// @return The error status.
        ComgrUtilsStatus ExtractHsaKernelData(HsaKernelData& data);

        /// @brief Extract the symbol info and fill the provided structure.
        ///
        /// @param [in] data The Symbol structure.
        ///
        /// @return true if successful, false otherwise.
        bool ExtractSymbolData(CodeObjSymbolInfo& data);

        /// @brief Extract the assembly data to a data buffer.
        ///
        /// @param [in] assembly_buffer The memory buffer of assembly data.
        /// @param [in] options         The options for extracting assembly buffer.
        ///
        /// @return true if successful, false otherwise.
        bool ExtractAssemblyData(std::vector<char>& assembly_buffer, std::string options);

        /// @brief Extract the assembly size in bytes to a data buffer.
        ///
        /// @param [in]  options           The options for extracting assembly buffer.
        /// @param [out] out_size_in_bytes Pointer to a uint to get size in bytes.
        ///
        /// @return true if successful, false otherwise.
        bool ExtractAssemblySizeInBytes(const char* options, uint32_t* out_size_in_bytes);

        /// @brief Extract the assembly size in bytes to a data buffer.
        ///
        /// @param [in]  options       The options for extracting assembly buffer.
        /// @param [in]  size_in_bytes The size in bytes of the input buffer, to validate memory is enough.
        /// @param [out] out_data      The buffer that will be filled with ISA.
        ///
        /// @return true if successful, false otherwise.
        bool ExtractAssemblyRaw(const char* options, const uint32_t size_in_bytes, char* out_data);

        /// @brief Convert the source data to a code object.
        ///
        /// @param [in] code_object_buffer The memory buffer of code object.
        /// @param [in] language_info      The language info for source data.
        /// @param [in] isa_name           The ISA name string.
        ///
        /// @return true if successful, false otherwise.
        bool ConvertSourceToCodeObject(std::vector<char>& code_object_buffer, const amd_comgr_language_t& language_info, const std::string& isa_name);

        /// @brief Clear the PAL pipeline data.
        ///
        /// @param [in] data The PalPipelineData type data.
        void ClearPalPipelineData(PalPipelineData& data);

        /// @brief Clear the HSA kernel data.
        ///
        /// @param [in] data The HsaKernelData type data.
        void ClearHsaKernelData(HsaKernelData& data);

        /// @brief Clear the symbol data
        ///
        /// @param [in] data The symbol data
        void ClearSymbolData(CodeObjSymbolInfo& data);

        /// @brief Get the status of the last operation.
        ///
        /// @return The AMD COMGR status.
        amd_comgr_status_t GetStatus() const;

        /// @brief Get the error caused by last unsuccessful operation.
        ///
        /// Restores the error value to "success".
        ///
        /// @return The pair of AMD COMGR status and error message string.
        std::pair<amd_comgr_status_t, std::string> GetLastError();

        /// @brief Set error.
        ///
        /// @param [in] err     The error status.
        /// @param [in] err_msg The error message string.
        ///
        /// @return true if successful, false otherwise.
        void SetError(amd_comgr_status_t err, const std::string& err_msg = "");

        /// @brief Constructor.
        ///
        /// @param [in] buf         The memory buffer.
        /// @param [in] co_data     The amd_comgr_data_t type data.
        /// @param [in] co_data_set The amd_comgr_data_set_t data set.
        CodeObj(const std::vector<char>& buf, amd_comgr_data_t co_data, amd_comgr_data_set_t co_data_set)
            : buf_(buf)
            , data_(co_data)
            , data_set_(co_data_set)
            , status_(AMD_COMGR_STATUS_ERROR)
            , err_msg_("")
        {
        }

        /// @brief Constructor for case where the data buffer is allocated and saved outside the scope of comgr_utils.
        ///
        /// Used for the OpenRaw() function.
        ///
        /// @param [in] co_data     The amd_comgr_data_t type data.
        /// @param [in] co_data_set The amd_comgr_data_set_t data set.
        CodeObj(amd_comgr_data_t co_data, amd_comgr_data_set_t co_data_set)
            : buf_()
            , data_(co_data)
            , data_set_(co_data_set)
            , status_(AMD_COMGR_STATUS_ERROR)
            , err_msg_("")
        {
        }

        /// @brief Destructor.
        ~CodeObj()
        {
            ComgrEntryPoints::Instance()->amd_comgr_release_data_fn(data_);
            ComgrEntryPoints::Instance()->amd_comgr_destroy_data_set_fn(data_set_);
        }

    private:
        /// @brief Helper function for extracting PAL metadata Shader Functions Info.
        ///
        /// @param [in] md_pipeline_data The pipeline data.
        /// @param [in] pp_in            The metadata node.
        ///
        /// @return true if successful, false otherwise.
        bool ExtractPalMDShaderFunctionsInfo(Pipeline& md_pipeline_data, MDNode& pp_in);

        /// @brief Helper function for extracting PAL metadata Shaders Info.
        ///
        /// @param [in] md_pipeline_data The pipeline data.
        /// @param [in] pp_in            The metadata node.
        ///
        /// @return true if successful, false otherwise.
        bool ExtractPalMDShadersInfo(Pipeline& md_pipeline_data, MDNode& pp_in);

        /// @brief Helper function for extracting PAL metadata Hardware Stages.
        ///
        /// @param [in] md_pipeline_data    The pipeline data.
        /// @param [in] version             The pipeline metadata version.
        /// @param [in] pp_in               The metadata node.
        /// @param [in] has_shader_function Flag indicating if the pipeline contains shader function node.
        ///
        /// @return true if successful, false otherwise.
        bool ExtractPalMDHardwareStages(Pipeline& md_pipeline_data, PalPipelineVersion& version, MDNode& pp_in, const bool has_shader_function);

        /// @brief Helper function for extracting PAL metadata for register info.
        ///
        /// @param [in] md_pipeline_data The pipeline data.
        /// @param [in] pp_in            The metadata node.
        /// @return true if successful, false otherwise.
        bool ExtractPalMDRegisterInfo(Pipeline& md_pipeline_data, MDNode& pp_in);

        /// @brief Helper function for extracting PAL metadata for compute register info.
        ///
        /// @param [in] md_pipeline_data The pipeline data.
        /// @param [in] pp_in            The metadata node.
        /// @return true if successful, false otherwise.
        bool ExtractPalMDComputeRegisters(Pipeline& md_pipeline_data, MDNode& pp_in);

        /// @brief Helper function for extracting PAL metadata for graphics register info.
        ///
        /// @param [in] md_pipeline_data The pipeline data.
        /// @param [in] pp_in            The metadata node.
        /// @return true if successful, false otherwise.
        bool ExtractPalMdGraphicsRegisterInfo(Pipeline& md_pipeline_data, MDNode& pp_in);

        /// @brief Helper function for extracting HSA metadata for kernel arguments info.
        ///
        /// @param [in] md_hsa_kernel_data The kernel data.
        /// @param [in] kernel_node        The kernel node.
        ///
        /// @return true if successful, false otherwise.
        bool ExtractHsaMdKernelArgsInfo(HsaKernel& md_hsa_kernel_data, MDNode& kernel_node);

        /// @brief Helper function to map HW mapping string to HW stage bit field.
        ///
        /// @param [in] hw_mapping The HW mapping string.
        /// @param [in] hw_stage   The HW stage bit representation.
        ///
        /// @return true if successful, false otherwise.
        bool SetHwStageBitFieldFromString(const std::string& hw_mapping, uint16_t& hw_stage);

        /// @brief Helper function to set pipeline type from string.
        ///
        /// @param [in] type          The pipeline type string.
        /// @param [in] pipeline_type The output pipeline type.
        ///
        /// @return true if successful, false otherwise.
        bool SetPipelineTypeFromString(const std::string& type, PipelineType& pipeline_type);

        std::vector<char>    buf_;       ///< Data buffer.
        amd_comgr_data_t     data_;      ///< The amd_comgr_data_t type data.
        amd_comgr_data_set_t data_set_;  ///< The amd_comgr_data_set_t type data set.
        amd_comgr_status_t   status_;    ///< The AMD COMGR status.
        std::string          err_msg_;   ///< The error message string.
    };

    /// @brief Metadata Node.
    class MDNode
    {
    public:
        /// @brief Metadata node kind.
        enum class Kind
        {
            kNodeKindNone = 0,  ///< No info for metadata node.
            kNodeKindInt,       ///< Metadata node kind is int.
            kNodeKindString,    ///< Metadata node kind is string.
            kNodeKindList,      ///< Metadata node kind is list.
            kNodeKindMap        ///< Metadata node kind is map.
        };

        /// @brief Constructor.
        ///
        /// @param [in] node                amd_comgr_metadata_node_t type metadata node.
        /// @param [in] current_code_object Pointer to current code object.
        MDNode(amd_comgr_metadata_node_t node, CodeObj* current_code_object = nullptr);

        /// @brief Constructor.
        ///
        /// @param [in] handle              The node handle.
        /// @param [in] current_code_object Pointer to current code object.
        MDNode(int handle, CodeObj* current_code_object = nullptr);

        /// @brief Destructor.
        ~MDNode() = default;

        /// @brief Get the kind of this MD node.
        ///
        /// @return metadata node kind.
        Kind GetKind() const;

        /// @brief Get the MD sub-node by its index (only valid for List MD nodes).
        ///
        /// @param [in] idx The metadata node index.
        ///
        /// @return The metadata node.
        MDNode operator[](int idx) const;

        /// @brief Get the MD sub-node by its index (only valid for List MD nodes).
        ///
        /// @param [in] idx The metadata node index.
        ///
        /// @return The metadata node.
        MDNode operator[](size_t idx) const;

        /// @brief Get the MD sub-node by its string key (only valid for Map MD nodes).
        ///
        /// @param [in] key The key string of metadata sub-node.
        ///
        /// @return The metadata node.
        MDNode operator[](const std::string& key) const;

        /// @brief Get the MD sub-node by its char* key (only valid for Map MD nodes).
        ///
        /// @param [in] key The key of metadata sub-node.
        ///
        /// @return The metadata node.
        MDNode operator[](const char* key) const;

        /// @brief Checks if sub-node with provided key exists in this node (only valid for Map MD nodes).
        ///
        /// @param [in] key The key of metadata sub-node.
        ///
        /// @return true if successful, false otherwise.
        bool Find(const std::string& key) const;

        /// @brief Get the value.
        ///
        /// @return The value.
        template <typename TYPE>
        TYPE value() const;

        /// @brief Get the number of sub-nodes of this MD node (only valid for List and Map nodes).
        ///
        /// @return The size.
        size_t size() const;

        /// @brief Get the list of string keys (only valid for Map MD nodes).
        ///
        /// @return The keys vector.
        std::vector<std::string> GetKeys() const;

        /// @brief Indicates whether the handle is valid.
        ///
        /// @return true if successful, false otherwise.
        bool IsValid() const;

        /// @brief Dump the error info.
        void Dump();

        /// @brief Destroy the underlying metadata node.
        void Destroy();

    private:
        amd_comgr_metadata_node_t handle_;               ///< The metadata node handle.
        CodeObj*                  current_code_object_;  ///< Current code object pointer.
    };

    /// @brief Specialization of "value" function for std::string.
    ///
    /// @return The value string.
    template <>
    std::string MDNode::value<std::string>() const;

    /// @brief Specialization of "value" function for bool.
    ///
    /// @return The bool value.
    template <>
    bool MDNode::value<bool>() const;

    /// @brief Specialization of "value" function for uint8_t.
    ///
    /// @return The uint8_t value.
    template <>
    uint8_t MDNode::value<uint8_t>() const;

    /// @brief Specialization of "value" function for uint16_t.
    ///
    /// @return The uint16_t value.
    template <>
    uint16_t MDNode::value<uint16_t>() const;

    /// @brief Specialization of "value" function for uint32_t.
    ///
    /// @return The uint32_t value.
    template <>
    uint32_t MDNode::value<uint32_t>() const;

    /// @brief Specialization of "value" function for uint64_t.
    ///
    /// @return The uint64_t value.
    template <>
    uint64_t MDNode::value<uint64_t>() const;

    /// @brief Utility function to demangle a mangled symbol name.
    ///
    /// @param [in]  mangled_name   The mangled symbol name that should be demangled.
    /// @param [out] demangled_name The demangled symbol name.
    ///
    /// @return True if the symbol name was successfully demangled. False otherwise.
    bool ComgrGetDemangledName(const std::string& mangled_name, std::string& demangled_name);

}  // namespace amdt

#endif
