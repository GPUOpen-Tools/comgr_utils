//============================================================================================
// Copyright (c) 2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools
/// @file
/// @brief  This is a high level C++ interface of comgr utility functionality for tools.
//============================================================================================

#include "comgr_utils.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

namespace amdt
{
/// Major and minor versions where hash sizes were changed.
#define COMGR_METADATA_MAJOR_VERSION_HASH_UPDATES (2)
#define COMGR_METADATA_MINOR_VERSION_HASH_UPDATES (0)

/// Major and minor versions where entry point tags were changed.
#define COMGR_METADATA_MAJOR_VERSION_ENTRY_POINT_TAG_UPDATES (3)
#define COMGR_METADATA_MINOR_VERSION_ENTRY_POINT_TAG_UPDATES (6)

    ComgrEntryPoints* ComgrEntryPoints::instance_ = nullptr;
    std::once_flag    ComgrEntryPoints::init_instance_flag_;

/// Helper macro to avoid warnings about unused arguments for callbacks.
#define COMGRUTILS_UNUSED(x) ((void)(x))

    /// @brief Safe version of strncpy function.
    ///
    /// @param [in, out] dest               Destination buffer.
    /// @param [in]      src                Source buffer.
    /// @param [in]      copy_size          Size of the data to copy.
    /// @param [in]      source_buffer_size Size of the source buffer.
    static void strncpy_safe(char* dest, const char* src, size_t copy_size, size_t source_buffer_size)
    {
#ifdef _WIN32
        strncpy_s(dest, copy_size, src, source_buffer_size);
#else

        COMGRUTILS_UNUSED(source_buffer_size);
        strncpy(dest, src, copy_size);

#endif
    }

    // HSA metadata tags.
    const char* kHsaMdTagKernelVersion        = "amdhsa.version";
    const char* kHsaMdTagKernels              = "amdhsa.kernels";
    const char* kHsaMdTagKernelArgs           = ".args";
    const char* kHsaMdTagGroupSegFixedSize    = ".group_segment_fixed_size";
    const char* kHsaMdTagKernArgSegAlign      = ".kernarg_segment_align";
    const char* kHsaMdTagKernArgSegSize       = ".kernarg_segment_size";
    const char* kHsaMdTagMaxFlatWorkgroupSize = ".max_flat_workgroup_size";
    const char* kHsaMdTagKernalName           = ".name";
    const char* kHsaMdTagPrivateSegFixedSize  = ".private_segment_fixed_size";
    const char* kHsaMdTagReqdWorkgroupSize    = ".reqd_workgroup_size";
    const char* kHsaMdTagSgprCount            = ".sgpr_count";
    const char* kHsaMdTagVgprCount            = ".vgpr_count";
    const char* kHsaMdTagSgprSpillCount       = ".sgpr_spill_count";
    const char* kHsaMdTagVgprSpillCount       = ".vgpr_spill_count";
    const char* kHsaMdTagWavefrontSize        = ".wavefront_size";
    const char* kHsaMdTagKernelArgName        = ".name";
    const char* kHsaMdTagKernelArgTypeName    = ".type_name";
    const char* kHsaMdTagKernelArgOffset      = ".offset";
    const char* kHsaMdTagKernelArgSize        = ".size";

    // PAL metadata tags.
    const char* kPalMdTagPipelineVersion = "amdpal.version";
    const char* kPalMdTagPipelineName    = ".name";
    const char* kPalMdTagPipelineType    = ".type";
    const char* kPalMdTagPipelineHash    = ".pipeline_compiler_hash";
    // New hash added as of metadata v2.0.
    const char* kPalMdTagInternalPipelineHash   = ".internal_pipeline_hash";
    const char* kPalMdTagPipelines              = "amdpal.pipelines";
    const char* kPalMdTagShaderFunctions        = ".shader_functions";
    const char* kPalMdTagShaders                = ".shaders";
    const char* kPalMdTagShaderTypeVertex       = ".vertex";
    const char* kPalMdTagShaderTypeHull         = ".hull";
    const char* kPalMdTagShaderTypeDomain       = ".domain";
    const char* kPalMdTagShaderTypeGeometry     = ".geometry";
    const char* kPalMdTagShaderTypePixel        = ".pixel";
    const char* kPalMdTagShaderTypeCompute      = ".compute";
    const char* kPalMdTagShaderTypeMesh         = ".mesh";
    const char* kPalMdTagShaderTypeTask         = ".task";
    const char* kPalMdTagShaderHash             = ".api_shader_hash";
    const char* kPalMdTagShaderHardwareMapping  = ".hardware_mapping";
    const char* kPalMdTagShaderStages           = ".hardware_stages";
    const char* kPalMdTagShaderStageLs          = ".ls";
    const char* kPalMdTagShaderStageHs          = ".hs";
    const char* kPalMdTagHardwareStageEs        = ".es";
    const char* kPalMdTagHardwareStageGs        = ".gs";
    const char* kPalMdTagHardwareStageVs        = ".vs";
    const char* kPalMdTagHardwareStagePs        = ".ps";
    const char* kPalMdTagHardwareStageCs        = ".cs";
    const char* kPalMdTagEntryPointName         = ".entry_point";
    const char* kPalMdTagEntryPointSymbolName   = ".entry_point_symbol";
    const char* kPalMdTagScratchMemorySize      = ".scratch_memory_size";
    const char* kPalMdTagLocalDataShareSize     = ".lds_size";
    const char* kPalMdTagPerfDataBufferSize     = "PerformanceDataBufferSize";
    const char* kPalMdTagNumUsedVgprs           = ".vgpr_count";
    const char* kPalMdTagNumUsedSgprs           = ".sgpr_count";
    const char* kPalMdTagNumAvailableVgprs      = ".vgpr_limit";
    const char* kPalMdTagNumAvailableSgprs      = ".sgpr_limit";
    const char* kPalMdTagWavesPerGroup          = ".waves_per_group";
    const char* kPalMdTagUsesUavs               = ".uses_uavs";
    const char* kPalMdTagUsesRovs               = ".uses_rovs";
    const char* kPalMdTagWritesUavs             = ".writes_uavs";
    const char* kPalMdTagWritesDepth            = ".writes_depth";
    const char* kPalMdTagMaxPrimsPerPsWave      = ".max_prims_per_wave";
    const char* kPalMdTagNumInterpolants        = ".num_interpolants";
    const char* kPalMdTagRegisters              = ".registers";
    const char* kPalMdTagUserDataLimit          = ".user_data_limit";
    const char* kPalMdTagSpillThreshold         = ".spill_threshold";
    const char* kPalMdTagUsesViewportArrayIndex = ".uses_viewport_array_index";
    const char* kPalMdTagEsGsLocalDataShareSize = ".es_gs_lds_size";
    const char* kPalMdTagWavefrontSize          = ".wavefront_size";
    const char* kPalMdTagApi                    = ".api";
    const char* kPalMdTagApiCreateInfo          = ".api_create_info";
    const char* kPalMdTagStackFrameSize         = ".stack_frame_size_in_bytes";
    const char* kPalMdTagShaderSubtype          = ".shader_subtype";
    const char* kPalMdTagUserDataRegMap         = ".user_data_reg_map";
    const char* kPalMdTagUsesCps                = ".uses_cps";
    const char* kPalMdTagThreadGroupDimensions  = ".threadgroup_dimensions";
    const char* kPalMdTagComputeRegisters       = ".compute_registers";
    const char* kPalMdTagDynamicVgprEn          = ".dynamic_vgpr_en";
    const char* kPalMdTagTgSizeEn               = ".tg_size_en";
    const char* kPalMdTagTgidXEn                = ".tgid_x_en";
    const char* kPalMdTagTgidYEn                = ".tgid_y_en";
    const char* kPalMdTagTgidZEn                = ".tgid_z_en";
    const char* kPalMdTagTidigCompCnt           = ".tidig_comp_cnt";
    const char* kPalMdTagXInterleave            = ".x_interleave";
    const char* kPalMdTagYInterleave            = ".y_interleave";
    // Graphics register metadata.
    const char* kPalMdTagGraphicsRegisters          = ".graphics_registers";
    const char* kPalMdTagAaCoverageToShaderSelect   = ".aa_coverage_to_shader_select";
    const char* kPalMdTagCbRegisterMask             = ".cb_shader_mask";
    const char* kPalMdTagDbShaderControl            = ".db_shader_control";
    const char* kPalMdTagEsVgprCompCnt              = ".es_vgpr_comp_cnt";
    const char* kPalMdTagGeNggSubgrpCntl            = ".ge_ngg_subgrp_cntl";
    const char* kPalMdTagGsVgprCompCnt              = ".gs_vgpr_comp_cnt";
    const char* kPalMdTagIaMultiVgtParam            = ".ia_multi_vgt_param";
    const char* kPalMdTagMaxVertsPerSubgroup        = ".max_verts_per_subgroup";
    const char* kPalMdTagNggCullingDataReg          = ".ngg_culling_data_reg";
    const char* kPalMdTagPaClClipCntl               = ".pa_cl_clip_cntl";
    const char* kPalMdTagPaClVsOutCntl              = ".pa_cl_vs_out_cntl";
    const char* kPalMdTagPaClVteCntl                = ".pa_cl_vte_cntl";
    const char* kPalMdTagPaScShaderControl          = ".pa_sc_shader_control";
    const char* kPalMdTagPaSuVtxCntl                = ".pa_su_vtx_cntl";
    const char* kPalMdTagPsExtraLdsSize             = ".ps_extra_lds_size";
    const char* kPalMdTagPsIterSample               = ".ps_iter_sample";
    const char* kPalMdTagPsLoadProvokingVtx         = ".ps_load_provoking_vtx";
    const char* kPalMdTagPsWaveCntEn                = ".ps_wave_cnt_en";
    const char* kPalMdTagSpiBarycCntl               = ".spi_baryc_cntl";
    const char* kPalMdTagSpiInterpControl           = ".spi_interp_control";
    const char* kPalMdTagSpiPsInControl             = ".spi_ps_in_control";
    const char* kPalMdTagSpiPsInputAddr             = ".spi_ps_input_addr";
    const char* kPalMdTagSpiPsInputCntl             = ".spi_ps_input_cntl";
    const char* kPalMdTagSpiPsInputEna              = ".spi_ps_input_ena";
    const char* kPalMdTagSpiShaderColFormat         = ".spi_shader_col_format";
    const char* kPalMdTagSpiShaderGsMeshletDim      = ".spi_shader_gs_meshlet_dim";
    const char* kPalMdTagSpiShaderGsMeshletExpAlloc = ".spi_shader_gs_meshlet_exp_alloc";
    const char* kPalMdTagSpiShaderIdxFormat         = ".spi_shader_idx_format";
    const char* kPalMdTagSpiShaderPosFormat         = ".spi_shader_pos_format";
    const char* kPalMdTagSpiShaderZFormat           = ".spi_shader_z_format";
    const char* kPalMdTagSpiVsOutConfig             = ".spi_vs_out_config";
    const char* kPalMdTagVgtDrawPrimPayloadEn       = ".vgt_draw_prim_payload_en";
    const char* kPalMdTagVgtEsGsRingItemSize        = ".vgt_esgs_ring_itemsize";
    const char* kPalMdTagVgtGsInstanceCnt           = ".vgt_gs_instance_cnt";
    const char* kPalMdTagVgtGsMaxVertOut            = ".vgt_gs_max_vert_out";
    const char* kPalMdTagVgtGsMode                  = ".vgt_gs_mode";
    const char* kPalMdTagVgtGsOnchipCntl            = ".vgt_gs_onchip_cntl";
    const char* kPalMdTagVgtGsOutPrimType           = ".vgt_gs_out_prim_type";
    const char* kPalMdTagVgtLsHsConfig              = ".vgt_ls_hs_config";
    const char* kPalMdTagVgtPrimitiveIdEn           = ".vgt_primitive_id_en";
    const char* kPalMdTagVgtReuseOff                = ".vgt_reuse_off";
    const char* kPalMdTagVgtShaderStagesEn          = ".vgt_shader_stages_en";
    const char* kPalMdTagVgtTfParam                 = ".vgt_tf_param";

    /// @brief Iteration state for symbols.
    struct CodeObjSymbolIterState
    {
        void*          scratch_buffer;                ///< Scratch buffer used in iteration.
        size_t         scratch_buffer_size_in_bytes;  ///< Scratch buffer size.
        uint32_t       symbol_count;                  ///< Num symbols.
        uint32_t       current_position;              ///< Current position.
        CodeObjSymbol* code_object_symbols;           ///< Array of symbols that is filled.

        /// @brief Constructor.
        CodeObjSymbolIterState()
            : scratch_buffer(nullptr)
            , scratch_buffer_size_in_bytes(0)
            , symbol_count(0)
            , current_position(0)
            , code_object_symbols(nullptr)
        {
        }
    };

    /// @brief Parses an uint32 from a string using the C stdlib function.
    ///
    /// @param [in]  str The string to parse the value from.
    /// @param [out] val The parsed value, only updated when parsing completed successfully.
    ///
    /// @return True on success, false otherwise.
    static bool convert_string_to_uint32(const char* str, uint32_t& val)
    {
        errno                = 0;
        char*         endptr = nullptr;
        unsigned long ret    = strtoul(str, &endptr, 0);

        if (str == endptr)
        {
            // No conversion was performed. ret is set to 0.
            return false;
        }
        else if (errno == ERANGE || ret > UINT32_MAX)
        {
            // The value is out of range. ret is set to ULONG_MAX.
            return false;
        }
        else if (*endptr != '\0')
        {
            // The string contains invalid characters or extra text after the number.
            return false;
        }
        val = static_cast<uint32_t>(ret);

        return true;
    }

    /// @brief Parses an uint64 from a string using the C stdlib function.
    ///
    /// @param [in]  str The string to parse the value from.
    /// @param [out] val The parsed value, only updated when parsing completed successfully.
    ///
    /// @return          True on success, false otherwise.
    static bool convert_string_to_uint64(const char* str, uint64_t& val)
    {
        errno                     = 0;
        char*              endptr = nullptr;
        unsigned long long ret    = strtoull(str, &endptr, 0);

        if (str == endptr)
        {
            // No conversion was performed. ret is set to 0.
            return false;
        }
        else if (errno == ERANGE)
        {
            // The value is out of range. ret is set to ULONG_MAX.
            return false;
        }
        else if (*endptr != '\0')
        {
            // The string contains invalid characters or extra text after the number.
            return false;
        }
        val = static_cast<uint64_t>(ret);

        return true;
    }

    /// @brief User data struct for the iterator callback function.
    struct MapIterCallbackUserData
    {
        std::vector<std::string> keys;         ///< Key storage for the iterator.
        amdt::CodeObj*           code_object;  ///< The code object being iterated over.
    };

    /// @brief Callback function for amd_comgr_iterate_map_metadata.
    ///
    /// @param [in] key  amd_comgr_metadata_node_t type key.
    /// @param [in] val  amd_comgr_metadata_node_t type value.
    /// @param [in] data Callback data pointer.
    ///
    /// @return AMD COMGR status.
    static amd_comgr_status_s MapIterCallback(amd_comgr_metadata_node_t key, amd_comgr_metadata_node_t val, void* data)
    {
        COMGRUTILS_UNUSED(val);

        MapIterCallbackUserData* user_data = static_cast<MapIterCallbackUserData*>(data);
        user_data->keys.push_back(MDNode(key, user_data->code_object).value<std::string>());
        return user_data->code_object->GetStatus();
    };

    /// @brief Callback function for amd_comgr_iterate_symbols.
    ///
    /// @param symbol    Handle to the symbol.
    /// @param user_data Callback user data pointer.
    ///
    /// @return AMD COMGR status.
    static amd_comgr_status_t CountFuncSymbolCallback(amd_comgr_symbol_t symbol, void* user_data)
    {
        amd_comgr_status_t status = AMD_COMGR_STATUS_ERROR;
        if (user_data != nullptr)
        {
            CodeObjSymbolIterState* state  = static_cast<CodeObjSymbolIterState*>(user_data);
            void*                   buffer = state->scratch_buffer;
            status                         = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_TYPE, buffer);

            if (*((amd_comgr_symbol_type_t*)buffer) == AMD_COMGR_SYMBOL_TYPE_FUNC)
            {
                // if its a function print some more info
                state->symbol_count += 1;
            }
        }

        return status;
    }

    /// @brief Callback function for amd_comgr_iterate_symbols.
    ///
    /// @param symbol    Handle to the symbol.
    /// @param user_data Callback user data pointer.
    ///
    /// @return AMD COMGR status.
    static amd_comgr_status_t AppendToSymbolVectorCallback(amd_comgr_symbol_t symbol, void* user_data)
    {
        amd_comgr_status_t status = AMD_COMGR_STATUS_ERROR;

        CodeObjSymbolIterState* state = static_cast<CodeObjSymbolIterState*>(user_data);
        if (state == nullptr)
        {
            return status;
        }

        // Abbreviated name to use in this function.
        void*  buffer      = state->scratch_buffer;
        size_t buffer_size = state->scratch_buffer_size_in_bytes;
        memset(buffer, '\0', buffer_size);

        status = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_TYPE, buffer);

        if (*((amd_comgr_symbol_type_t*)buffer) == AMD_COMGR_SYMBOL_TYPE_FUNC)
        {
            CodeObjSymbol* function_symbol = &(state->code_object_symbols[state->current_position]);

            function_symbol->type = kComgrUtilsSymbolTypeFunc;

            memset(buffer, '\0', buffer_size);
            status = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_NAME_LENGTH, buffer);
            function_symbol->symbol_function.name_len = *(size_t*)(buffer);

            function_symbol->symbol_function.name = (char*)malloc(function_symbol->symbol_function.name_len + 1);
            if (function_symbol->symbol_function.name != nullptr)
            {
                memset(function_symbol->symbol_function.name, '\0', function_symbol->symbol_function.name_len + 1);
                status = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_NAME, function_symbol->symbol_function.name);
            }

            memset(buffer, 0, buffer_size);
            status = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_SIZE, buffer);
            function_symbol->symbol_function.symbol_size = *(uint64_t*)buffer;

            memset(buffer, 0, buffer_size);
            status = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_VALUE, buffer);
            function_symbol->symbol_function.symbol_value = *(uint64_t*)buffer;

            // Next position.
            state->current_position += 1;
        }

        return status;
    }

    std::unique_ptr<CodeObj> CodeObj::OpenFile(const std::string& file_name)
    {
        std::ifstream     file(file_name, std::ios::in | std::ios::binary);
        std::vector<char> buf;
        file.seekg(0, std::ios::end);
        int size = static_cast<int>(file.tellg());
        file.seekg(0, std::ios::beg);
        buf.resize(size);
        file.read(&buf[0], size);
        return OpenBuffer(buf);
    }

    std::unique_ptr<CodeObj> CodeObj::OpenFile(const std::string& file_name, const amd_comgr_data_kind_t& data_kind)
    {
        std::ifstream     file(file_name, std::ios::in | std::ios::binary);
        std::vector<char> buf;
        file.seekg(0, std::ios::end);
        int size = static_cast<int>(file.tellg());
        file.seekg(0, std::ios::beg);
        buf.resize(size);
        file.read(&buf[0], size);
        return OpenBuffer(buf, data_kind);
    }

    std::unique_ptr<CodeObj> CodeObj::OpenBufferRaw(const char* buf, const size_t size_in_bytes)
    {
        amd_comgr_data_t     co_data;
        amd_comgr_data_set_t co_data_set;
        amd_comgr_status_t   status = AMD_COMGR_STATUS_ERROR;

        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_fn(AMD_COMGR_DATA_KIND_RELOCATABLE, &co_data);
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_set_data_fn(co_data, size_in_bytes, buf);
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_set_data_name_fn(co_data, "data");
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&co_data_set);
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_data_set_add_fn(co_data_set, co_data);
        CheckStatusAlt(status, nullptr);

        std::unique_ptr<CodeObj> code_obj(new (std::nothrow) CodeObj(co_data, co_data_set));

        return code_obj;
    }

    std::unique_ptr<CodeObj> CodeObj::OpenBuffer(const std::vector<char>& buf)
    {
        amd_comgr_data_t     co_data;
        amd_comgr_data_set_t co_data_set;
        amd_comgr_status_t   status = AMD_COMGR_STATUS_ERROR;

        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_fn(AMD_COMGR_DATA_KIND_RELOCATABLE, &co_data);
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_set_data_fn(co_data, buf.size(), &buf[0]);
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_set_data_name_fn(co_data, "data");
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&co_data_set);
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_data_set_add_fn(co_data_set, co_data);
        CheckStatusAlt(status, nullptr);

        std::unique_ptr<CodeObj> code_obj(new (std::nothrow) CodeObj(buf, co_data, co_data_set));

        return code_obj;
    }

    std::unique_ptr<CodeObj> CodeObj::OpenBuffer(const std::vector<char>& buf, const amd_comgr_data_kind_t& data_kind)
    {
        amd_comgr_data_t     co_data;
        amd_comgr_data_set_t co_data_set;
        amd_comgr_status_t   status = AMD_COMGR_STATUS_ERROR;

        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_fn(data_kind, &co_data);
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_set_data_fn(co_data, buf.size(), &buf[0]);
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_set_data_name_fn(co_data, "data");
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&co_data_set);
        CheckStatusAlt(status, nullptr);
        status = ComgrEntryPoints::Instance()->amd_comgr_data_set_add_fn(co_data_set, co_data);
        CheckStatusAlt(status, nullptr);

        std::unique_ptr<CodeObj> code_obj(new (std::nothrow) CodeObj(buf, co_data, co_data_set));

        return code_obj;
    }

    MDNode CodeObj::GetMD()
    {
        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), 0);
        amd_comgr_metadata_node_t md;
        amd_comgr_status_t        status;
        status = ComgrEntryPoints::Instance()->amd_comgr_get_data_metadata_fn(data_, &md);
        CheckStatus(this, status, 0);

        // The root must be map.
        amd_comgr_metadata_kind_t kind = AMD_COMGR_METADATA_KIND_NULL;
        status                         = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_kind_fn(md, &kind);
        if (status != AMD_COMGR_STATUS_SUCCESS)
        {
            SetError(status);
            ComgrEntryPoints::Instance()->amd_comgr_destroy_metadata_fn(md);
            return 0;
        }
        if (kind != AMD_COMGR_METADATA_KIND_MAP)
        {
            ComgrEntryPoints::Instance()->amd_comgr_destroy_metadata_fn(md);
        }
        return (kind == AMD_COMGR_METADATA_KIND_MAP ? MDNode(md, this) : 0);
    }

    bool CodeObj::ExtractSymbolData(CodeObjSymbolInfo& data)
    {
        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), false);
        // Scratch buffer to hold symbol info within callback over symbols.
        CodeObjSymbolIterState* iter_state = (CodeObjSymbolIterState*)malloc(sizeof(CodeObjSymbolIterState));
        if (iter_state == nullptr)
        {
            return false;
        }
        memset(reinterpret_cast<void*>(iter_state), 0, sizeof(CodeObjSymbolIterState));
        static const size_t s_SCRATCH_BUFER_SIZE = 1024;
        iter_state->scratch_buffer_size_in_bytes = s_SCRATCH_BUFER_SIZE;
        iter_state->scratch_buffer               = (char*)malloc(iter_state->scratch_buffer_size_in_bytes);
        if (iter_state->scratch_buffer == nullptr)
        {
            free(iter_state);
            return false;
        }

        bool               ret_code = false;
        amd_comgr_status_t status   = ComgrEntryPoints::Instance()->amd_comgr_iterate_symbols_fn(data_, CountFuncSymbolCallback, iter_state);
        if (status != AMD_COMGR_STATUS_SUCCESS)
        {
            SetError(status);
            free(iter_state->scratch_buffer);
            free(iter_state);
            return ret_code;
        }
        memset(iter_state->scratch_buffer, 0, iter_state->scratch_buffer_size_in_bytes);

        if (iter_state->symbol_count > 0)
        {
            iter_state->code_object_symbols = (CodeObjSymbol*)malloc(sizeof(CodeObjSymbol) * iter_state->symbol_count);
            if (iter_state->code_object_symbols != nullptr)
            {
                status = ComgrEntryPoints::Instance()->amd_comgr_iterate_symbols_fn(data_, AppendToSymbolVectorCallback, iter_state);
                if (status != AMD_COMGR_STATUS_SUCCESS)
                {
                    SetError(status);
                    free(iter_state->code_object_symbols);
                    free(iter_state->scratch_buffer);
                    free(iter_state);
                    return ret_code;
                }

                if (status == AMD_COMGR_STATUS_SUCCESS)
                {
                    data.num_symbols = iter_state->symbol_count;
                    data.symbols     = iter_state->code_object_symbols;
                    ret_code         = true;
                }
            }
        }

        // Don't free iter_state->symbols.
        // The pointer to the symbols, it will be cleared when you call ClearSymbolData.
        free(iter_state->scratch_buffer);
        free(iter_state);

        return ret_code;
    }

    void CodeObj::ClearSymbolData(CodeObjSymbolInfo& data)
    {
        if ((data.num_symbols > 0) && (data.symbols != nullptr))
        {
            for (uint32_t i = 0; i < data.num_symbols; ++i)
            {
                if (data.symbols[i].type == kComgrUtilsSymbolTypeFunc)
                {
                    if (data.symbols[i].symbol_function.name != nullptr)
                    {
                        free(data.symbols[i].symbol_function.name);
                    }
                }
            }
            free(data.symbols);
            data.num_symbols = 0;
        }
    }

    ComgrUtilsStatus CodeObj::ExtractHsaKernelData(HsaKernelData& data)
    {
        MDNode md = GetMD();

        MDNode version = md[kHsaMdTagKernelVersion];

        if (!version.IsValid())
        {
            md.Destroy();
            return kComgrUtilsStatusError;
        }

        size_t version_entries = version.size();
        if (version_entries < 2)
        {
            version.Destroy();
            md.Destroy();
            return kComgrUtilsStatusError;
        }

        MDNode major_node = version[0];
        MDNode minor_node = version[1];

        data.version.major_version = major_node.value<uint32_t>();
        data.version.minor_version = minor_node.value<uint32_t>();

        major_node.Destroy();
        minor_node.Destroy();
        version.Destroy();

        // Extract HSA kernels.
        MDNode kernels = md[kHsaMdTagKernels];
        if (!kernels.IsValid())
        {
            md.Destroy();
            return kComgrUtilsStatusError;
        }

        size_t num_kernels = kernels.size();
        data.num_kernels   = static_cast<uint32_t>(num_kernels);

        size_t kernels_alloc_size = num_kernels * sizeof(HsaKernel);
        // Check if the above multiplication resulted in an overflow.
        if (num_kernels != kernels_alloc_size / sizeof(HsaKernel))
        {
            kernels.Destroy();
            md.Destroy();
            return kComgrUtilsStatusError;
        }

        data.kernels = (HsaKernel*)malloc(kernels_alloc_size);
        if (nullptr == data.kernels)
        {
            kernels.Destroy();
            md.Destroy();
            return kComgrUtilsStatusError;
        }

        memset(data.kernels, 0, kernels_alloc_size);
        ComgrUtilsStatus ret_code = kComgrUtilsStatusSuccess;
        for (size_t i = 0; i < num_kernels; i++)
        {
            HsaKernel* kernel_data = &data.kernels[i];
            MDNode     kernel_node = kernels[i];

            // Extract name.
            MDNode kernel_name = kernel_node[kHsaMdTagKernalName];
            if (kernel_name.IsValid())
            {
                const std::string& name = kernel_name.value<std::string>();
                kernel_data->name       = (char*)malloc(name.length() + 1);
                if (nullptr == kernel_data->name)
                {
                    return kComgrUtilsStatusError;
                }
                memset(kernel_data->name, '\0', name.length() + 1);
                strncpy_safe(kernel_data->name, name.c_str(), name.length() + 1, name.length() + 1);
            }
            kernel_name.Destroy();

            // Extract SGPR count.
            MDNode sgpr_count = kernel_node[kHsaMdTagSgprCount];
            if (sgpr_count.IsValid())
            {
                kernel_data->sgpr_count = sgpr_count.value<uint32_t>();
            }
            sgpr_count.Destroy();

            // Extract VGPR count.
            MDNode vgpr_count = kernel_node[kHsaMdTagVgprCount];
            if (vgpr_count.IsValid())
            {
                kernel_data->vgpr_count = vgpr_count.value<uint32_t>();
            }
            vgpr_count.Destroy();

            // Extract kernel arguments.
            if (!ExtractHsaMdKernelArgsInfo(*kernel_data, kernel_node))
            {
                ret_code = kComgrUtilsStatusInvalidHsaKernelInfo;
            }
        }

        return ret_code;
    }

    ComgrUtilsStatus CodeObj::ExtractPalPipelineData(PalPipelineData& data)
    {
        MDNode md = GetMD();

        // Extract version.
        MDNode version = md[kPalMdTagPipelineVersion];
        if (!version.IsValid())
        {
            ReportInvalidRequiredPalMDMapItem(version);
            md.Destroy();
            return kComgrUtilsStatusError;
        }

        size_t version_entries = version.size();
        if (version_entries < 2)
        {
            version.Destroy();
            md.Destroy();
            return kComgrUtilsStatusError;
        }

        MDNode major_node = version[0];
        MDNode minor_node = version[1];

        data.version.major_version = major_node.value<uint32_t>();
        data.version.minor_version = minor_node.value<uint32_t>();

        major_node.Destroy();
        minor_node.Destroy();
        version.Destroy();

        // Extract pipelines.
        MDNode pipelines = md[kPalMdTagPipelines];
        if (!pipelines.IsValid())
        {
            ReportInvalidRequiredPalMDMapItem(pipelines);
            md.Destroy();
            return kComgrUtilsStatusInvalidPipelineInfo;
        }

        size_t pipelines_num        = pipelines.size();
        data.num_pipelines          = static_cast<uint32_t>(pipelines_num);
        size_t pipelines_alloc_size = pipelines_num * sizeof(Pipeline);
        // Check if the above multiplication resulted in an overflow.
        if (pipelines_num != pipelines_alloc_size / sizeof(Pipeline))
        {
            md.Destroy();
            return kComgrUtilsStatusError;
        }

        data.pipelines = (Pipeline*)malloc(pipelines_alloc_size);
        if (nullptr == data.pipelines)
        {
            pipelines.Destroy();
            md.Destroy();
            return kComgrUtilsStatusInvalidPipelineInfo;
        }

        memset(data.pipelines, 0, pipelines_alloc_size);
        ComgrUtilsStatus ret_code = kComgrUtilsStatusSuccess;

        for (size_t i = 0; i < pipelines_num; i++)
        {
            Pipeline* pipeline_data = &data.pipelines[i];
            MDNode    ppln          = pipelines[i];

            // Name (optional).
            MDNode ppln_name = ppln[kPalMdTagPipelineName];
            if (ppln_name.IsValid())
            {
                const std::string& name = ppln_name.value<std::string>();
                ppln_name.Destroy();
                pipeline_data->name = (char*)malloc(name.size() + 1);
                if (nullptr == pipeline_data->name)
                {
                    ret_code = kComgrUtilsStatusInvalidPipelineInfo;
                    break;
                }

                memset(pipeline_data->name, '\0', name.size() + 1);
                strncpy_safe(pipeline_data->name, name.c_str(), name.size() + 1, name.length() + 1);
            }

            // Type (optional).
            MDNode ppln_type = ppln[kPalMdTagPipelineType];
            if (ppln_type.IsValid())
            {
                const std::string& type = ppln_type.value<std::string>();
                ppln_type.Destroy();

                if (!SetPipelineTypeFromString(type, pipeline_data->type))
                {
                    ret_code = kComgrUtilsStatusInvalidPipelineInfo;
                    break;
                }
            }

            // Hash (required).
            if (data.version.major_version < COMGR_METADATA_MAJOR_VERSION_HASH_UPDATES)
            {
                MDNode ppln_hash = ppln[kPalMdTagPipelineHash];
                if (!ppln_hash.IsValid())
                {
                    ReportInvalidRequiredPalMDMapItem(ppln_hash);
                    ppln.Destroy();
                    pipelines.Destroy();
                    md.Destroy();
                    return kComgrUtilsStatusInvalidPipelineInfo;
                }
                pipeline_data->internal_pipeline_hash_lo = ppln_hash.value<uint64_t>();
                ppln_hash.Destroy();
            }
            else
            {
                MDNode ppln_hash = ppln[kPalMdTagInternalPipelineHash];
                if (!ppln_hash.IsValid())
                {
                    ReportInvalidRequiredPalMDMapItem(ppln_hash);
                    ppln.Destroy();
                    pipelines.Destroy();
                    md.Destroy();
                    return kComgrUtilsStatusInvalidPipelineInfo;
                }
                else
                {
                    size_t num_hash_entries = ppln_hash.size();
                    if (num_hash_entries >= 2)
                    {
                        MDNode hash_lo = ppln_hash[0];
                        MDNode hash_hi = ppln_hash[1];

                        pipeline_data->internal_pipeline_hash_lo = hash_lo.value<uint64_t>();
                        pipeline_data->internal_pipeline_hash_hi = hash_hi.value<uint64_t>();

                        hash_lo.Destroy();
                        hash_hi.Destroy();
                    }

                    ppln_hash.Destroy();
                }
            }

            ExtractPalMDComputeRegisters(*pipeline_data, ppln);

            // Uses CPS (Continuation Passing Shader).
            MDNode uses_cps = ppln[kPalMdTagUsesCps];
            if (uses_cps.IsValid())
            {
                pipeline_data->uses_cps = uses_cps.value<bool>();
                uses_cps.Destroy();
            }

            // User Data Limit (optional).
            MDNode user_data_limit = ppln[kPalMdTagUserDataLimit];
            if (user_data_limit.IsValid())
            {
                pipeline_data->user_data_limit = user_data_limit.value<uint32_t>();
                user_data_limit.Destroy();
            }

            // Spill threshold (optional).
            MDNode spill_threshold = ppln[kPalMdTagSpillThreshold];
            if (spill_threshold.IsValid())
            {
                pipeline_data->spill_threshold = spill_threshold.value<uint32_t>();
                spill_threshold.Destroy();
            }

            // UsesViewportArrayIndex (optional).
            MDNode viewport_array_index = ppln[kPalMdTagUsesViewportArrayIndex];
            if (viewport_array_index.IsValid())
            {
                pipeline_data->uses_viewport_array_index = viewport_array_index.value<uint32_t>();
                viewport_array_index.Destroy();
            }

            // EsGsLocalDataShareSize (optional).
            MDNode es_gs_loc_data_share_size = ppln[kPalMdTagEsGsLocalDataShareSize];
            if (es_gs_loc_data_share_size.IsValid())
            {
                pipeline_data->es_gs_local_data_share_size = es_gs_loc_data_share_size.value<uint32_t>();
                es_gs_loc_data_share_size.Destroy();
            }

            // NumInterpolants (optional).
            MDNode num_ps_interpolants = ppln[kPalMdTagNumInterpolants];
            if (num_ps_interpolants.IsValid())
            {
                pipeline_data->num_interpolants = num_ps_interpolants.value<uint32_t>();
                num_ps_interpolants.Destroy();
            }

            // API (optional).
            MDNode api = ppln[kPalMdTagApi];
            if (api.IsValid())
            {
                const std::string& api_name = api.value<std::string>();
                api.Destroy();
                pipeline_data->api_name = (char*)malloc(api_name.size() + 1);
                if (nullptr == pipeline_data->api_name)
                {
                    ret_code = kComgrUtilsStatusInvalidPipelineInfo;
                    break;
                }

                memset(pipeline_data->api_name, '\0', api_name.size() + 1);
                strncpy_safe(pipeline_data->api_name, api_name.c_str(), api_name.size() + 1, api_name.length() + 1);
            }

            // ApiCreateInfo (optional).
            MDNode api_create = ppln[kPalMdTagApiCreateInfo];
            if (api_create.IsValid())
            {
                pipeline_data->api_create_info = api_create.value<uint32_t>();
                api_create.Destroy();
            }

            // Extract shaders and shader info.
            bool regular_shader = ExtractPalMDShadersInfo(*pipeline_data, ppln);

            // Extract shader function info.
            bool shader_function = ExtractPalMDShaderFunctionsInfo(*pipeline_data, ppln);

            if (!regular_shader && !shader_function)
            {
                ret_code = kComgrUtilsStatusInvalidShaderInfo;
            }
            else
            {
                // Extract hardware stages.
                if (!ExtractPalMDHardwareStages(*pipeline_data, data.version, ppln, shader_function))
                {
                    ppln.Destroy();
                    pipelines.Destroy();
                    md.Destroy();
                    return kComgrUtilsStatusInvalidHwStageInfo;
                }

                // Extract register info.
                if (!ExtractPalMDRegisterInfo(*pipeline_data, ppln))
                {
                    ppln.Destroy();
                    pipelines.Destroy();
                    md.Destroy();
                    return kComgrUtilsStatusInvalidRegisterInfo;
                }

                ExtractPalMdGraphicsRegisterInfo(*pipeline_data, ppln);
            }

            ppln.Destroy();

        }  // End pipleline loop.

        pipelines.Destroy();

        md.Destroy();

        return ret_code;
    }

    bool CodeObj::ExtractAssemblySizeInBytes(const char* options, uint32_t* out_size_in_bytes)
    {
        if (out_size_in_bytes == nullptr)
        {
            return false;
        }

        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), false);
        amd_comgr_status_t      status;
        amd_comgr_action_info_t action_info;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_action_info_fn(&action_info);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_isa_name_fn(action_info, options);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_option_list_fn(action_info, nullptr, 0);
        CheckStatus(this, status, false);

        amd_comgr_data_set_t data_set_out;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&data_set_out);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(AMD_COMGR_ACTION_DISASSEMBLE_RELOCATABLE_TO_SOURCE, action_info, data_set_, data_set_out);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_destroy_action_info_fn(action_info);
        CheckStatus(this, status, false);

        size_t count;
        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(data_set_out, AMD_COMGR_DATA_KIND_SOURCE, &count);
        CheckStatus(this, status, false);

        if (1 != count)
        {
            std::cerr << "ERROR: Number of data object should be 1." << std::endl;
            return false;
        }

        amd_comgr_data_t data_out;
        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_get_data_fn(data_set_out, AMD_COMGR_DATA_KIND_SOURCE, 0, &data_out);
        CheckStatus(this, status, false);

        // Update size only, then we can update output buffer later.
        status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(data_out, &count, nullptr);
        CheckStatus(this, status, false);

        // Update output buffer.
        out_size_in_bytes[0] = static_cast<uint32_t>(count);
        return true;
    }

    bool CodeObj::ExtractAssemblyRaw(const char* options, const uint32_t ip_buffer_size_in_bytes, char* out_data)
    {
        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), false);
        amd_comgr_status_t status;

        amd_comgr_action_info_t action_info;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_action_info_fn(&action_info);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_isa_name_fn(action_info, options);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_option_list_fn(action_info, nullptr, 0);
        CheckStatus(this, status, false);

        amd_comgr_data_set_t data_set_out;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&data_set_out);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(AMD_COMGR_ACTION_DISASSEMBLE_RELOCATABLE_TO_SOURCE, action_info, data_set_, data_set_out);
        CheckStatus(this, status, false);

        size_t count;
        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(data_set_out, AMD_COMGR_DATA_KIND_SOURCE, &count);
        CheckStatus(this, status, false);

        if (1 != count)
        {
            std::cerr << "ERROR: Number of data object should be 1." << std::endl;
            return false;
        }

        amd_comgr_data_t data_out;
        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_get_data_fn(data_set_out, AMD_COMGR_DATA_KIND_SOURCE, 0, &data_out);
        CheckStatus(this, status, false);

        // Update size only, then we can update output buffer later.
        status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(data_out, &count, nullptr);
        CheckStatus(this, status, false);

        // If we at least have enough memory.
        if (ip_buffer_size_in_bytes >= count)
        {
            // Fill output buffer.
            status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(data_out, &count, out_data);
            CheckStatus(this, status, false);
        }

        return true;
    }

    bool CodeObj::ExtractAssemblyData(std::vector<char>& assembly_buffer, std::string options)
    {
        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), false);
        amd_comgr_status_t status;

        amd_comgr_action_info_t action_info;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_action_info_fn(&action_info);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_isa_name_fn(action_info, options.c_str());
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_option_list_fn(action_info, nullptr, 0);
        CheckStatus(this, status, false);

        amd_comgr_data_set_t data_set_out;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&data_set_out);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(AMD_COMGR_ACTION_DISASSEMBLE_RELOCATABLE_TO_SOURCE, action_info, data_set_, data_set_out);
        CheckStatus(this, status, false);

        size_t count;
        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(data_set_out, AMD_COMGR_DATA_KIND_SOURCE, &count);
        CheckStatus(this, status, false);

        if (1 != count)
        {
            std::cerr << "ERROR: Number of data object should be 1." << std::endl;
            return false;
        }

        amd_comgr_data_t data_out;
        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_get_data_fn(data_set_out, AMD_COMGR_DATA_KIND_SOURCE, 0, &data_out);
        CheckStatus(this, status, false);

        // Update size only, then we can update output buffer later.
        status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(data_out, &count, nullptr);
        CheckStatus(this, status, false);

        // Update output buffer.
        assembly_buffer.resize(count);
        status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(data_out, &count, assembly_buffer.data());
        CheckStatus(this, status, false);

        return true;
    }

    bool CodeObj::ConvertSourceToCodeObject(std::vector<char>& code_object_buffer, const amd_comgr_language_t& language_info, const std::string& isa_name)
    {
        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), false);
        amd_comgr_status_t status;

        amd_comgr_action_info_t action_info;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_action_info_fn(&action_info);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_language_fn(action_info, language_info);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_isa_name_fn(action_info, isa_name.c_str());
        CheckStatus(this, status, false);

        const char* options1[] = {"-mno-code-object-v3"};
        status                 = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_option_list_fn(action_info, options1, 1);
        CheckStatus(this, status, false);

        amd_comgr_data_set_t data_set_pre_compiled_headers;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&data_set_pre_compiled_headers);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(
            AMD_COMGR_ACTION_ADD_PRECOMPILED_HEADERS, action_info, data_set_, data_set_pre_compiled_headers);
        CheckStatus(this, status, false);

        size_t count;
        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(data_set_pre_compiled_headers, AMD_COMGR_DATA_KIND_PRECOMPILED_HEADER, &count);
        CheckStatus(this, status, false);

        if (1 != count)
        {
            std::cerr << "ERROR: Incorrect number of data object (expected 1)." << std::endl;
            return false;
        }

        amd_comgr_data_set_t data_set_bit_code;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&data_set_bit_code);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(
            AMD_COMGR_ACTION_COMPILE_SOURCE_TO_BC, action_info, data_set_pre_compiled_headers, data_set_bit_code);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(data_set_bit_code, AMD_COMGR_DATA_KIND_BC, &count);
        CheckStatus(this, status, false);

        if (1 != count)
        {
            std::cerr << "ERROR: Incorrect number of data object (expected 1)." << std::endl;
            return false;
        }

        amd_comgr_data_set_t data_set_dev_libs;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&data_set_dev_libs);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_option_list_fn(action_info, nullptr, 0);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(
            AMD_COMGR_ACTION_COMPILE_SOURCE_WITH_DEVICE_LIBS_TO_BC, action_info, data_set_bit_code, data_set_dev_libs);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(data_set_dev_libs, AMD_COMGR_DATA_KIND_BC, &count);
        CheckStatus(this, status, false);

        amd_comgr_data_set_t data_set_linked;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&data_set_linked);
        CheckStatus(this, status, false);

        const char* options2[] = {"-mno-code-object-v3"};
        status                 = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_option_list_fn(action_info, options2, 1);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(AMD_COMGR_ACTION_LINK_BC_TO_BC, action_info, data_set_dev_libs, data_set_linked);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(data_set_linked, AMD_COMGR_DATA_KIND_BC, &count);
        CheckStatus(this, status, false);

        if (1 != count)
        {
            std::cerr << "ERROR: Incorrect number of data object (expected 1)." << std::endl;
            return false;
        }

        amd_comgr_data_set_t data_set_relocatable;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&data_set_relocatable);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(
            AMD_COMGR_ACTION_CODEGEN_BC_TO_RELOCATABLE, action_info, data_set_linked, data_set_relocatable);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(data_set_relocatable, AMD_COMGR_DATA_KIND_RELOCATABLE, &count);
        CheckStatus(this, status, false);

        if (1 != count)
        {
            std::cerr << "ERROR: Incorrect number of data object (expected 1)." << std::endl;
            return false;
        }

        amd_comgr_data_set_t data_set_executable;
        status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&data_set_executable);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_option_list_fn(action_info, nullptr, 0);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(
            AMD_COMGR_ACTION_LINK_RELOCATABLE_TO_EXECUTABLE, action_info, data_set_relocatable, data_set_executable);
        CheckStatus(this, status, false);

        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(data_set_executable, AMD_COMGR_DATA_KIND_EXECUTABLE, &count);
        CheckStatus(this, status, false);

        if (1 != count)
        {
            std::cerr << "ERROR: Incorrect number of data object (expected 1)." << std::endl;
            return false;
        }

        amd_comgr_data_t data_out;
        status = ComgrEntryPoints::Instance()->amd_comgr_action_data_get_data_fn(data_set_executable, AMD_COMGR_DATA_KIND_EXECUTABLE, 0, &data_out);
        CheckStatus(this, status, false);

        // Update size only, then we can update output buffer later.
        status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(data_out, &count, nullptr);
        CheckStatus(this, status, false);

        // Update output buffer.
        code_object_buffer.resize(count);
        status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(data_out, &count, code_object_buffer.data());
        CheckStatus(this, status, false);

        return true;
    }

    void CodeObj::ClearHsaKernelData(HsaKernelData& data)
    {
        for (size_t i = 0; i < data.num_kernels; i++)
        {
            HsaKernel* kernel = &data.kernels[i];

            if (kernel != nullptr)
            {
                for (size_t j = 0; j < kernel->num_kernel_args; j++)
                {
                    HsaKernelArgInfo* kernel_arg_info = &kernel->kernel_arg_list[j];

                    if (kernel_arg_info != nullptr)
                    {
                        free(kernel_arg_info->name);
                        free(kernel_arg_info->type_name);
                    }
                }
            }

            free(kernel->kernel_arg_list);
            free(kernel->name);
        }

        free(data.kernels);
    }

    void CodeObj::ClearPalPipelineData(PalPipelineData& data)
    {
        for (size_t pplnN = 0; pplnN < data.num_pipelines; pplnN++)
        {
            Pipeline* ppln = &data.pipelines[pplnN];
            free(ppln->name);
            free(ppln->api_name);

            for (size_t stage_n = 0; stage_n < ppln->num_stages; stage_n++)
            {
                HWStageInfo* stage = &ppln->stage_list[stage_n];
                free(stage->entry_point_symbol_name);
            }

            free(ppln->stage_list);

            free(ppln->register_data_list);

            for (size_t i = 0; i < ppln->num_shader_functions; i++)
            {
                ShaderFunctionInfo* shader_func_info = &ppln->shader_function_list[i];

                if (nullptr != shader_func_info)
                {
                    free(shader_func_info->name);
                    free(shader_func_info->shader_subtype);
                }
            }

            for (size_t i = 0; i < ppln->num_shaders; i++)
            {
                ShaderInfo* shader_info = &ppln->shader_list[i];

                if (shader_info != nullptr)
                {
                    free(shader_info->shader_subtype);
                }
            }

            free(ppln->shader_list);
            free(ppln->shader_function_list);
        }

        free(data.pipelines);

        memset(reinterpret_cast<void*>(&data), 0, sizeof(PalPipelineData));
    }

    amd_comgr_status_t CodeObj::GetStatus() const
    {
        return status_;
    }

    std::pair<amd_comgr_status_t, std::string> CodeObj::GetLastError()
    {
        if (!ComgrEntryPoints::Instance()->EntryPointsValid())
        {
            return {AMD_COMGR_STATUS_ERROR, "Invalid comgr entry points"};
        }

        const char*        err_msg = nullptr;
        std::string        msg     = (ComgrEntryPoints::Instance()->amd_comgr_status_string_fn(status_, &err_msg) == AMD_COMGR_STATUS_SUCCESS ? err_msg : "");
        amd_comgr_status_t status  = status_;
        status_                    = AMD_COMGR_STATUS_SUCCESS;
        err_msg_                   = "";
        return {status, msg};
    }

    bool CodeObj::SetHwStageBitFieldFromString(const std::string& hw_mapping, uint16_t& hw_stage)
    {
        if (hw_mapping == ".es")
        {
            hw_stage |= 1 << HwStageType::kHwStageEs;
        }
        else if (hw_mapping == ".gs")
        {
            hw_stage |= 1 << HwStageType::kHwStageGs;
        }
        else if (hw_mapping == ".vs")
        {
            hw_stage |= 1 << HwStageType::kHwStageVs;
        }
        else if (hw_mapping == ".hs")
        {
            hw_stage |= 1 << HwStageType::kHwStageHs;
        }
        else if (hw_mapping == ".ls")
        {
            hw_stage |= 1 << HwStageType::kHwStageLs;
        }
        else if (hw_mapping == ".ps")
        {
            hw_stage |= 1 << HwStageType::kHwStagePs;
        }
        else if (hw_mapping == ".cs")
        {
            hw_stage |= 1 << HwStageType::kHwStageCs;
        }
        else
        {
            assert(false && "ERROR: Unknown HW Stage Type");
        }

        return true;
    }

    bool CodeObj::SetPipelineTypeFromString(const std::string& type, PipelineType& pipeline_type)
    {
        if (type == "Cs")
        {
            pipeline_type = kComgrUtilsPipelineTypeCs;
        }
        else if (type == "VsPs")
        {
            pipeline_type = kComgrUtilsPipelineTypeVsPs;
        }
        else if (type == "Gs")
        {
            pipeline_type = kComgrUtilsPipelineTypeGs;
        }
        else if (type == "Ngg")
        {
            pipeline_type = kComgrUtilsPipelineTypeNgg;
        }
        else if (type == "Tess")
        {
            pipeline_type = kComgrUtilsPipelineTypeTess;
        }
        else if (type == "GsTess")
        {
            pipeline_type = kComgrUtilsPipelineTypeGsTess;
        }
        else if (type == "NggPrim")
        {
            pipeline_type = kComgrUtilsPipelineTypeNggPrim;
        }
        else if (type == "NggTess")
        {
            pipeline_type = kComgrUtilsPipelineTypeNggTess;
        }
        else if (type == "NggSurfPrim")
        {
            pipeline_type = kComgrUtilsPipelineTypeNggSurfPrim;
        }
        else if (type == "Mesh")
        {
            pipeline_type = kComgrUtilsPipelineTypeMesh;
        }
        else if (type == "TaskMesh")
        {
            pipeline_type = kComgrUtilsPipelineTypeTaskMesh;
        }
        else
        {
            assert(false && "ERROR: Unknown HW Stage Type");
        }

        return true;
    }

    bool CodeObj::ExtractHsaMdKernelArgsInfo(HsaKernel& md_hsa_kernel_data, MDNode& kernel_node)
    {
        MDNode kernel_args = kernel_node[kHsaMdTagKernelArgs];

        if (!kernel_args.IsValid())
        {
            kernel_args.Destroy();
            return false;
        }

        uint32_t kernel_arg_count          = static_cast<uint32_t>(kernel_args.size());
        md_hsa_kernel_data.num_kernel_args = kernel_arg_count;
        size_t kernel_arg_alloc_size       = kernel_arg_count * sizeof(HsaKernelArgInfo);
        // Check if the above multiplication resulted in an overflow.
        if (kernel_arg_count != kernel_arg_alloc_size / sizeof(HsaKernelArgInfo))
        {
            kernel_args.Destroy();
            return false;
        }

        md_hsa_kernel_data.kernel_arg_list = (HsaKernelArgInfo*)malloc(kernel_arg_alloc_size);

        if (md_hsa_kernel_data.kernel_arg_list == nullptr)
        {
            kernel_args.Destroy();
            return false;
        }

        memset(md_hsa_kernel_data.kernel_arg_list, 0, kernel_arg_alloc_size);

        for (size_t arg_idx = 0; arg_idx < kernel_arg_count; arg_idx++)
        {
            MDNode            kernel_arg      = kernel_args[arg_idx];
            HsaKernelArgInfo* kernel_arg_info = &md_hsa_kernel_data.kernel_arg_list[arg_idx];

            // Extract name.
            MDNode kernel_arg_name = kernel_arg[kHsaMdTagKernelArgName];
            if (kernel_arg_name.IsValid())
            {
                const std::string& name = kernel_arg_name.value<std::string>();
                kernel_arg_info->name   = (char*)malloc(name.length() + 1);
                if (nullptr == kernel_arg_info->name)
                {
                    return false;
                }
                memset(kernel_arg_info->name, '\0', name.length() + 1);
                strncpy_safe(kernel_arg_info->name, name.c_str(), name.length() + 1, name.length() + 1);
            }
            kernel_arg_name.Destroy();

            // Extract type name.
            MDNode kernel_arg_type_name = kernel_arg[kHsaMdTagKernelArgTypeName];
            if (kernel_arg_type_name.IsValid())
            {
                const std::string& type_name = kernel_arg_type_name.value<std::string>();
                kernel_arg_info->type_name   = (char*)malloc(type_name.length() + 1);
                if (nullptr == kernel_arg_info->type_name)
                {
                    return false;
                }
                memset(kernel_arg_info->type_name, '\0', type_name.length() + 1);
                strncpy_safe(kernel_arg_info->type_name, type_name.c_str(), type_name.length() + 1, type_name.length() + 1);
            }
            kernel_arg_type_name.Destroy();

            // Extract offset.
            MDNode kernel_arg_offset = kernel_arg[kHsaMdTagKernelArgOffset];
            if (kernel_arg_offset.IsValid())
            {
                kernel_arg_info->offset = kernel_arg_offset.value<uint32_t>();
            }
            kernel_arg_offset.Destroy();

            // Extract size.
            MDNode kernel_arg_size = kernel_arg[kHsaMdTagKernelArgSize];
            if (kernel_arg_size.IsValid())
            {
                kernel_arg_info->size = kernel_arg_size.value<uint32_t>();
            }
            kernel_arg_size.Destroy();
        }

        kernel_args.Destroy();

        return true;
    }

    bool CodeObj::ExtractPalMDShaderFunctionsInfo(Pipeline& md_pipeline_data, MDNode& pp_in)
    {
        MDNode shader_functions = pp_in[kPalMdTagShaderFunctions];

        if (!shader_functions.IsValid())
        {
            ReportInvalidRequiredPalMDMapItem(shader_functions);
            shader_functions.Destroy();
            return false;
        }

        size_t shader_functions_count         = shader_functions.size();
        md_pipeline_data.num_shader_functions = static_cast<uint32_t>(shader_functions_count);
        size_t shader_functions_alloc_size    = shader_functions_count * sizeof(ShaderFunctionInfo);
        // Check if the above multiplication resulted in an overflow.
        if (shader_functions_alloc_size == 0 || shader_functions_count != shader_functions_alloc_size / sizeof(ShaderFunctionInfo))
        {
            shader_functions.Destroy();
            return false;
        }

        md_pipeline_data.shader_function_list = (ShaderFunctionInfo*)malloc(shader_functions_alloc_size);
        if (nullptr == md_pipeline_data.shader_function_list)
        {
            shader_functions.Destroy();
            return false;
        }

        memset(md_pipeline_data.shader_function_list, 0, shader_functions_alloc_size);

        size_t i = 0;

        for (const std::string& shader_name : shader_functions.GetKeys())
        {
            ShaderFunctionInfo* shader_func_info = &md_pipeline_data.shader_function_list[i++];
            shader_func_info->name               = (char*)malloc(shader_name.length() + 1);

            if (nullptr == shader_func_info->name)
            {
                return false;
            }

            memset(shader_func_info->name, '\0', shader_name.length() + 1);
            strncpy_safe(shader_func_info->name, shader_name.c_str(), shader_name.length() + 1, shader_name.length() + 1);

            MDNode shader_info = shader_functions[shader_name];
            if (!(shader_info.IsValid() && shader_info.GetKind() == MDNode::Kind::kNodeKindMap))
            {
                shader_info.Destroy();
                shader_functions.Destroy();
                return false;
            }

            MDNode api_shader_hash = shader_info[kPalMdTagShaderHash];
            if (!api_shader_hash.IsValid())
            {
                // This may be an internal shader function for which the internal_shader_hash does not apply,
                // as it is not compiled via SPIR-V/DXIL.
                shader_func_info->api_shader_hash_hi = 0;
                shader_func_info->api_shader_hash_lo = 0;
                api_shader_hash.Destroy();
            }
            else
            {
                size_t num_hash_entries = api_shader_hash.size();

                if (num_hash_entries == 2)
                {
                    MDNode shader_hash_lo = api_shader_hash[0];
                    MDNode shader_hash_hi = api_shader_hash[1];

                    shader_func_info->api_shader_hash_lo = shader_hash_lo.value<uint64_t>();
                    shader_func_info->api_shader_hash_hi = shader_hash_hi.value<uint64_t>();

                    shader_hash_lo.Destroy();
                    shader_hash_hi.Destroy();
                }

                api_shader_hash.Destroy();
            }

            MDNode lds_size = shader_info[kPalMdTagLocalDataShareSize];
            if (lds_size.IsValid())
            {
                shader_func_info->lds_size = lds_size.value<uint64_t>();
                lds_size.Destroy();
            }

            MDNode scratch_memory_size = shader_info[kPalMdTagScratchMemorySize];
            if (scratch_memory_size.IsValid())
            {
                shader_func_info->scratch_memory_size = scratch_memory_size.value<uint64_t>();
                scratch_memory_size.Destroy();
            }

            MDNode sgpr_count = shader_info[kPalMdTagNumUsedSgprs];
            if (sgpr_count.IsValid())
            {
                shader_func_info->sgpr_count = sgpr_count.value<uint64_t>();
                sgpr_count.Destroy();
            }

            MDNode vgpr_count = shader_info[kPalMdTagNumUsedVgprs];
            if (vgpr_count.IsValid())
            {
                shader_func_info->vgpr_count = vgpr_count.value<uint64_t>();
                vgpr_count.Destroy();
            }

            MDNode stack_frame_size_in_bytes = shader_info[kPalMdTagStackFrameSize];
            if (stack_frame_size_in_bytes.IsValid())
            {
                shader_func_info->stack_frame_size_in_bytes = stack_frame_size_in_bytes.value<uint64_t>();
                stack_frame_size_in_bytes.Destroy();
            }

            MDNode shader_subtype = shader_info[kPalMdTagShaderSubtype];
            if (shader_subtype.IsValid())
            {
                const std::string subtype_name = shader_subtype.value<std::string>();
                shader_subtype.Destroy();

                shader_func_info->shader_subtype = (char*)malloc(subtype_name.length() + 1);
                memset(shader_func_info->shader_subtype, '\0', subtype_name.length() + 1);
                strncpy_safe(shader_func_info->shader_subtype, subtype_name.c_str(), subtype_name.length() + 1, subtype_name.length() + 1);
            }
            shader_info.Destroy();
        }
        shader_functions.Destroy();

        return true;
    }

    bool CodeObj::ExtractPalMDShadersInfo(Pipeline& md_pipeline_data, MDNode& ppln)
    {
        MDNode shaders = ppln[kPalMdTagShaders];
        if (!shaders.IsValid())
        {
            ReportInvalidRequiredPalMDMapItem(shaders);
            shaders.Destroy();
            return false;
        }

        size_t shaders_num           = shaders.size();
        md_pipeline_data.num_shaders = static_cast<uint32_t>(shaders_num);
        size_t stages_alloc_size     = shaders_num * sizeof(ShaderInfo);
        // Check if the above multiplication resulted in an overflow.
        if (stages_alloc_size == 0 || shaders_num != stages_alloc_size / sizeof(ShaderInfo))
        {
            shaders.Destroy();
            return false;
        }
        md_pipeline_data.shader_list = (ShaderInfo*)malloc(stages_alloc_size);
        if (nullptr == md_pipeline_data.shader_list)
        {
            shaders.Destroy();
            return false;
        }

        memset(md_pipeline_data.shader_list, 0, stages_alloc_size);

        int shader_n = 0;

        for (const std::string& shader_type : shaders.GetKeys())
        {
            ShaderInfo* shader_info_data = &md_pipeline_data.shader_list[shader_n++];

            // Shader Info Type.
            if (shader_type == kPalMdTagShaderTypeVertex)
            {
                shader_info_data->shader_type = ShaderInfoType::kVertexShader;
            }
            else if (shader_type == kPalMdTagShaderTypeHull)
            {
                shader_info_data->shader_type = ShaderInfoType::kHullShader;
            }
            else if (shader_type == kPalMdTagShaderTypeDomain)
            {
                shader_info_data->shader_type = ShaderInfoType::kDomainShader;
            }
            else if (shader_type == kPalMdTagShaderTypeGeometry)
            {
                shader_info_data->shader_type = ShaderInfoType::kGeometryShader;
            }
            else if (shader_type == kPalMdTagShaderTypePixel)
            {
                shader_info_data->shader_type = ShaderInfoType::kPixelShader;
            }
            else if (shader_type == kPalMdTagShaderTypeCompute)
            {
                shader_info_data->shader_type = ShaderInfoType::kComputeShader;
            }
            else if (shader_type == kPalMdTagShaderTypeMesh)
            {
                shader_info_data->shader_type = ShaderInfoType::kMeshShader;
            }
            else if (shader_type == kPalMdTagShaderTypeTask)
            {
                shader_info_data->shader_type = ShaderInfoType::kTaskShader;
            }
            else
            {
                assert(false && "ERROR: Unknown Shader Info Type.");
            }

            MDNode shader_info = shaders[shader_type];
            if (!(shader_info.IsValid() && shader_info.GetKind() == MDNode::Kind::kNodeKindMap))
            {
                shader_info.Destroy();
                shaders.Destroy();
                return false;
            }

            // API Shader Hash.
            MDNode api_shader_hash = shader_info[kPalMdTagShaderHash];
            if (!api_shader_hash.IsValid())
            {
                ReportInvalidRequiredPalMDMapItem(api_shader_hash);
                shader_info.Destroy();
                shaders.Destroy();
                return false;
            }
            else
            {
                size_t num_hash_entries = api_shader_hash.size();
                if (num_hash_entries == 2)
                {
                    MDNode shader_hash_lo = api_shader_hash[0];
                    MDNode shader_hash_hi = api_shader_hash[1];

                    shader_info_data->api_shader_hash_lo = shader_hash_lo.value<uint64_t>();
                    shader_info_data->api_shader_hash_hi = shader_hash_hi.value<uint64_t>();

                    shader_hash_lo.Destroy();
                    shader_hash_hi.Destroy();
                }

                api_shader_hash.Destroy();
            }

            // Hardware Mapping.
            MDNode shader_hw_mapping = shader_info[kPalMdTagShaderHardwareMapping];
            if (!shader_hw_mapping.IsValid())
            {
                ReportInvalidRequiredPalMDMapItem(shader_hw_mapping);
                shader_info.Destroy();
                shaders.Destroy();
                return false;
            }
            else
            {
                shader_info_data->num_hardware_mappings      = static_cast<uint32_t>(shader_hw_mapping.size());
                shader_info_data->hardware_mapping_bit_field = 0;

                for (size_t i = 0; i < shader_info_data->num_hardware_mappings; i++)
                {
                    MDNode hw_mapping_node = shader_hw_mapping[i];
                    if (!hw_mapping_node.IsValid())
                    {
                        ReportInvalidRequiredPalMDMapItem(hw_mapping_node);
                        shader_info.Destroy();
                        shaders.Destroy();
                        return false;
                    }
                    else
                    {
                        const std::string& hw_mapping = hw_mapping_node.value<std::string>();
                        hw_mapping_node.Destroy();
                        if (!SetHwStageBitFieldFromString(hw_mapping, shader_info_data->hardware_mapping_bit_field))
                        {
                            shader_info_data->hardware_mapping_bit_field = 0xffff;
                            shader_hw_mapping.Destroy();
                            shader_info.Destroy();
                            shaders.Destroy();
                            return false;
                        }
                    }
                }
            }

            // Shader subtype.
            MDNode shader_subtype = shader_info[kPalMdTagShaderSubtype];
            if (shader_subtype.IsValid())
            {
                const std::string subtype_name = shader_subtype.value<std::string>();
                shader_subtype.Destroy();

                shader_info_data->shader_subtype = (char*)malloc(subtype_name.length() + 1);
                memset(shader_info_data->shader_subtype, '\0', subtype_name.length() + 1);
                strncpy_safe(shader_info_data->shader_subtype, subtype_name.c_str(), subtype_name.length() + 1, subtype_name.length() + 1);
            }

            shader_hw_mapping.Destroy();
            shader_info.Destroy();
        }

        shaders.Destroy();

        return true;
    }

    bool CodeObj::ExtractPalMDHardwareStages(Pipeline& md_pipeline_data, PalPipelineVersion& version, MDNode& ppln, const bool has_shader_function)
    {
        MDNode stages = ppln[kPalMdTagShaderStages];
        if (!stages.IsValid())
        {
            ReportInvalidRequiredPalMDMapItem(stages);
            return false;
        }

        size_t stages_num           = stages.size();
        md_pipeline_data.num_stages = static_cast<uint32_t>(stages_num);
        size_t stages_alloc_size    = stages_num * sizeof(HWStageInfo);
        // Check if the above multiplication resulted in an overflow.
        if (stages_num != stages_alloc_size / sizeof(HWStageInfo))
        {
            return false;
        }
        md_pipeline_data.stage_list = (HWStageInfo*)malloc(stages_alloc_size);

        if (md_pipeline_data.stage_list == nullptr)
        {
            stages.Destroy();
            return false;
        }

        memset(md_pipeline_data.stage_list, 0, stages_alloc_size);

        int stage_n = 0;

        for (const std::string& stage_type : stages.GetKeys())
        {
            HWStageInfo* stage_info_data = &md_pipeline_data.stage_list[stage_n++];

            // Stage Type.
            if (stage_type == kPalMdTagShaderStageLs)
            {
                stage_info_data->stage_type = HwStageType::kHwStageLs;
            }
            else if (stage_type == kPalMdTagShaderStageHs)
            {
                stage_info_data->stage_type = HwStageType::kHwStageHs;
            }
            else if (stage_type == kPalMdTagHardwareStageEs)
            {
                stage_info_data->stage_type = HwStageType::kHwStageEs;
            }
            else if (stage_type == kPalMdTagHardwareStageGs)
            {
                stage_info_data->stage_type = HwStageType::kHwStageGs;
            }
            else if (stage_type == kPalMdTagHardwareStagePs)
            {
                stage_info_data->stage_type = HwStageType::kHwStagePs;
            }
            else if (stage_type == kPalMdTagHardwareStageVs)
            {
                stage_info_data->stage_type = HwStageType::kHwStageVs;
            }
            else if (stage_type == kPalMdTagHardwareStageCs)
            {
                stage_info_data->stage_type = HwStageType::kHwStageCs;
            }
            else
            {
                assert(false && "ERROR: Unknown HW Stage Type");
            }

            MDNode stage_info = stages[stage_type];
            if (!(stage_info.IsValid() && stage_info.GetKind() == MDNode::Kind::kNodeKindMap))
            {
                stage_info.Destroy();
                stages.Destroy();
                return false;
            }

            // Entry Symbol Name.
            // When shader functions present, we do not need to validate the entry point.
            if (!has_shader_function)
            {
                if (((version.major_version == COMGR_METADATA_MAJOR_VERSION_ENTRY_POINT_TAG_UPDATES) &&
                     (version.minor_version >= COMGR_METADATA_MINOR_VERSION_ENTRY_POINT_TAG_UPDATES)) ||
                    (version.major_version > COMGR_METADATA_MAJOR_VERSION_ENTRY_POINT_TAG_UPDATES))
                {
                    MDNode entry_name = stage_info[kPalMdTagEntryPointSymbolName];
                    if (!entry_name.IsValid())
                    {
                        ReportInvalidRequiredPalMDMapItem(entry_name);
                        stage_info.Destroy();
                        stages.Destroy();
                        return false;
                    }

                    const std::string& name = entry_name.value<std::string>();
                    entry_name.Destroy();

                    stage_info_data->entry_point_symbol_name = (char*)malloc(name.size() + 1);
                    memset(stage_info_data->entry_point_symbol_name, '\0', name.size() + 1);

                    strncpy_safe(stage_info_data->entry_point_symbol_name, name.c_str(), name.size() + 1, name.length() + 1);
                }
                else
                {
                    MDNode entry_name = stage_info[kPalMdTagEntryPointName];
                    if (!entry_name.IsValid())
                    {
                        ReportInvalidRequiredPalMDMapItem(entry_name);
                        stage_info.Destroy();
                        stages.Destroy();
                        return false;
                    }

                    const std::string& name = entry_name.value<std::string>();
                    entry_name.Destroy();

                    stage_info_data->entry_point_symbol_name = (char*)malloc(name.size() + 1);
                    memset(stage_info_data->entry_point_symbol_name, '\0', name.size() + 1);

                    strncpy_safe(stage_info_data->entry_point_symbol_name, name.c_str(), name.size() + 1, name.length() + 1);
                }
            }

            // Local Data Share Size.
            MDNode local_share_size = stage_info[kPalMdTagLocalDataShareSize];
            if (local_share_size.IsValid())
            {
                stage_info_data->local_data_share_size = local_share_size.value<uint32_t>();
                local_share_size.Destroy();
            }

            // Perf Data Buffer Size.
            MDNode perf_buf_size = stage_info[kPalMdTagPerfDataBufferSize];
            if (perf_buf_size.IsValid())
            {
                stage_info_data->performance_data_buffer_size = perf_buf_size.value<uint32_t>();
                perf_buf_size.Destroy();
            }

            // Used VGPRs/SGPRs.
            MDNode used_vgprs = stage_info[kPalMdTagNumUsedVgprs];
            if (used_vgprs.IsValid())
            {
                stage_info_data->num_used_vgprs = used_vgprs.value<uint32_t>();
                used_vgprs.Destroy();
            }

            MDNode used_sgprs = stage_info[kPalMdTagNumUsedSgprs];
            if (used_sgprs.IsValid())
            {
                stage_info_data->num_used_sgprs = used_sgprs.value<uint32_t>();
                used_sgprs.Destroy();
            }

            // Wavefront size.
            MDNode wavefront_size = stage_info[kPalMdTagWavefrontSize];
            if (wavefront_size.IsValid())
            {
                stage_info_data->wavefront_size = wavefront_size.value<uint32_t>();
                wavefront_size.Destroy();
            }

            // If the kPalMdTagNumAvailableVgprs or kPalMdTagNumAvailableSgprs
            // tags are not there, then we should be using the device limits.
            //
            // The metadata tags only added if the limits were explicitly overwritten.

            // Available VGPRs/SGPRs.
            MDNode available_vgprs = stage_info[kPalMdTagNumAvailableVgprs];
            if (available_vgprs.IsValid())
            {
                stage_info_data->num_available_vgprs = available_vgprs.value<uint32_t>();
                available_vgprs.Destroy();
            }

            MDNode available_sgprs = stage_info[kPalMdTagNumAvailableSgprs];
            if (available_sgprs.IsValid())
            {
                stage_info_data->num_available_sgprs = available_sgprs.value<uint32_t>();
                available_sgprs.Destroy();
            }

            // Waves Per Group.
            MDNode waves_per_group = stage_info[kPalMdTagWavesPerGroup];
            if (waves_per_group.IsValid())
            {
                stage_info_data->waves_per_group = waves_per_group.value<uint32_t>();
                waves_per_group.Destroy();
            }

            // Uses Uavs.
            MDNode uses_uavs = stage_info[kPalMdTagUsesUavs];
            if (uses_uavs.IsValid())
            {
                stage_info_data->uses_uavs = uses_uavs.value<uint32_t>();
                uses_uavs.Destroy();
            }

            // Uses Rovs.
            MDNode uses_rovs = stage_info[kPalMdTagUsesRovs];
            if (uses_rovs.IsValid())
            {
                stage_info_data->uses_rovs = uses_rovs.value<uint32_t>();
                uses_rovs.Destroy();
            }

            // Writes Uavs.
            MDNode writes_uavs = stage_info[kPalMdTagWritesUavs];
            if (writes_uavs.IsValid())
            {
                stage_info_data->writes_uavs = writes_uavs.value<uint32_t>();
                writes_uavs.Destroy();
            }

            // Writes Depth.
            MDNode writes_depth = stage_info[kPalMdTagWritesDepth];
            if (writes_depth.IsValid())
            {
                stage_info_data->writes_depth = writes_depth.value<uint32_t>();
                writes_depth.Destroy();
            }

            // MaxPrimsPerPsWave (optional).
            MDNode prims_per_ps_wave = stage_info[kPalMdTagMaxPrimsPerPsWave];
            if (prims_per_ps_wave.IsValid())
            {
                stage_info_data->max_prims_per_ps_wave = prims_per_ps_wave.value<uint32_t>();
                prims_per_ps_wave.Destroy();
            }
            else
            {
                stage_info_data->max_prims_per_ps_wave = 0;
            }

            // Scratch Memory Size (optional).
            MDNode scratch_mem_size = stage_info[kPalMdTagScratchMemorySize];
            if (scratch_mem_size.IsValid())
            {
                stage_info_data->scratch_memory_size = scratch_mem_size.value<uint32_t>();
                scratch_mem_size.Destroy();
            }
            else
            {
                stage_info_data->scratch_memory_size = 0;
            }

            MDNode user_data_reg_map = stage_info[kPalMdTagUserDataRegMap];
            if (user_data_reg_map.IsValid())
            {
                size_t num_regs = user_data_reg_map.size();

                for (size_t i = 0; i < num_regs; i++)
                {
                    MDNode reg_data                       = user_data_reg_map[i];
                    stage_info_data->user_data_reg_map[i] = reg_data.value<uint32_t>();
                    reg_data.Destroy();
                }
            }
            user_data_reg_map.Destroy();

            MDNode threadgroup_dimensions = stage_info[kPalMdTagThreadGroupDimensions];
            if (threadgroup_dimensions.IsValid())
            {
                if (threadgroup_dimensions.size() == 3)
                {
                    MDNode dim_x = threadgroup_dimensions[0];
                    MDNode dim_y = threadgroup_dimensions[1];
                    MDNode dim_z = threadgroup_dimensions[2];

                    stage_info_data->threadgroup_dimensions[0] = dim_x.value<uint32_t>();
                    stage_info_data->threadgroup_dimensions[1] = dim_y.value<uint32_t>();
                    stage_info_data->threadgroup_dimensions[2] = dim_z.value<uint32_t>();

                    dim_x.Destroy();
                    dim_y.Destroy();
                    dim_z.Destroy();
                }
            }
            threadgroup_dimensions.Destroy();

            stage_info.Destroy();
        }

        stages.Destroy();

        return true;
    }

    bool CodeObj::ExtractPalMDComputeRegisters(Pipeline &md_pipeline_data, MDNode &ppln)
    {
        MDNode compute_registers = ppln[kPalMdTagComputeRegisters];
        auto &compute_registers_info = md_pipeline_data.compute_registers_info;

        if (!compute_registers.IsValid())
        {
            ReportInvalidRequiredPalMDMapItem(compute_registers);
            return false;
        }

        MDNode dynamic_vgpr_en = compute_registers[kPalMdTagDynamicVgprEn];
        compute_registers_info.dynamic_vgpr_en = dynamic_vgpr_en.value<bool>();
        dynamic_vgpr_en.Destroy();

        MDNode tg_size_en = compute_registers[kPalMdTagTgSizeEn];
        compute_registers_info.tg_size_en = tg_size_en.value<bool>();
        tg_size_en.Destroy();

        MDNode tgid_x_en = compute_registers[kPalMdTagTgidXEn];
        compute_registers_info.tgid_x_en = tgid_x_en.value<bool>();
        tgid_x_en.Destroy();

        MDNode tgid_y_en = compute_registers[kPalMdTagTgidYEn];
        compute_registers_info.tgid_y_en = tgid_y_en.value<bool>();
        tgid_y_en.Destroy();

        MDNode tgid_z_en = compute_registers[kPalMdTagTgidZEn];
        compute_registers_info.tgid_z_en = tgid_z_en.value<bool>();
        tgid_z_en.Destroy();

        MDNode tidig_comp_cnt = compute_registers[kPalMdTagTidigCompCnt];
        compute_registers_info.tidig_comp_cnt = tidig_comp_cnt.value<uint32_t>();
        tidig_comp_cnt.Destroy();

        MDNode x_interleave = compute_registers[kPalMdTagXInterleave];
        compute_registers_info.x_interleave = x_interleave.value<uint32_t>();
        x_interleave.Destroy();

        MDNode y_interleave = compute_registers[kPalMdTagYInterleave];
        compute_registers_info.y_interleave = y_interleave.value<uint32_t>();
        y_interleave.Destroy();

        compute_registers.Destroy();
        return true;
    }

    bool CodeObj::ExtractPalMdGraphicsRegisterInfo(Pipeline& md_pipeline_data, MDNode& pp_in)
    {
        MDNode graphics_registers     = pp_in[kPalMdTagGraphicsRegisters];
        auto&  graphics_register_info = md_pipeline_data.graphics_register_info;

        if (!graphics_registers.IsValid())
        {
            ReportInvalidRequiredPalMDMapItem(graphics_registers);
            graphics_registers.Destroy();
        }

        MDNode cb_shader_mask = graphics_registers[kPalMdTagCbRegisterMask];
        if (cb_shader_mask.IsValid())
        {
            MDNode output0_enable                                 = cb_shader_mask[".output0_enable"];
            graphics_register_info.cb_shader_mask.output_0_enable = output0_enable.value<uint8_t>();
            output0_enable.Destroy();

            MDNode output1_enable                                 = cb_shader_mask[".output1_enable"];
            graphics_register_info.cb_shader_mask.output_1_enable = output1_enable.value<uint8_t>();
            output1_enable.Destroy();

            MDNode output2_enable                                 = cb_shader_mask[".output2_enable"];
            graphics_register_info.cb_shader_mask.output_2_enable = output2_enable.value<uint8_t>();
            output2_enable.Destroy();

            MDNode output3_enable                                 = cb_shader_mask[".output3_enable"];
            graphics_register_info.cb_shader_mask.output_3_enable = output3_enable.value<uint8_t>();
            output3_enable.Destroy();

            MDNode output4_enable                                 = cb_shader_mask[".output4_enable"];
            graphics_register_info.cb_shader_mask.output_4_enable = output4_enable.value<uint8_t>();
            output4_enable.Destroy();

            MDNode output5_enable                                 = cb_shader_mask[".output5_enable"];
            graphics_register_info.cb_shader_mask.output_5_enable = output5_enable.value<uint8_t>();
            output5_enable.Destroy();

            MDNode output6_enable                                 = cb_shader_mask[".output6_enable"];
            graphics_register_info.cb_shader_mask.output_6_enable = output6_enable.value<uint8_t>();
            output6_enable.Destroy();

            MDNode output7_enable                                 = cb_shader_mask[".output7_enable"];
            graphics_register_info.cb_shader_mask.output_7_enable = output7_enable.value<uint8_t>();
            output7_enable.Destroy();

            cb_shader_mask.Destroy();
        }

        MDNode db_shader_control = graphics_registers[kPalMdTagDbShaderControl];
        if (db_shader_control.IsValid())
        {
            MDNode alpha_to_mask_disable                                   = db_shader_control[".alpha_to_mask_disable"];
            graphics_register_info.db_shader_control.alpha_to_mask_disable = static_cast<uint16_t>(alpha_to_mask_disable.value<bool>());
            alpha_to_mask_disable.Destroy();

            MDNode conservative_z_export                                   = db_shader_control[".conservative_z_export"];
            graphics_register_info.db_shader_control.conservative_z_export = conservative_z_export.value<uint8_t>();
            conservative_z_export.Destroy();

            MDNode coverage_to_mask_en                                   = db_shader_control[".coverage_to_mask_en"];
            graphics_register_info.db_shader_control.coverage_to_mask_en = coverage_to_mask_en.value<bool>();
            coverage_to_mask_en.Destroy();

            MDNode exec_on_hier_fail                                   = db_shader_control[".exec_on_hier_fail"];
            graphics_register_info.db_shader_control.exec_on_hier_fail = exec_on_hier_fail.value<bool>();
            exec_on_hier_fail.Destroy();

            MDNode exec_on_noop                                   = db_shader_control[".exec_on_noop"];
            graphics_register_info.db_shader_control.exec_on_noop = exec_on_noop.value<bool>();
            exec_on_noop.Destroy();

            MDNode kill_enable                                   = db_shader_control[".kill_enable"];
            graphics_register_info.db_shader_control.kill_enable = kill_enable.value<bool>();
            kill_enable.Destroy();

            MDNode mask_export_enable                                   = db_shader_control[".mask_export_enable"];
            graphics_register_info.db_shader_control.mask_export_enable = mask_export_enable.value<bool>();
            mask_export_enable.Destroy();

            MDNode pre_shader_depth_coverage_enable                                   = db_shader_control[".pre_shader_depth_coverage_enable"];
            graphics_register_info.db_shader_control.pre_shader_depth_coverage_enable = pre_shader_depth_coverage_enable.value<bool>();
            pre_shader_depth_coverage_enable.Destroy();

            MDNode primitive_ordered_pixel_shader                                   = db_shader_control[".primitive_ordered_pixel_shader"];
            graphics_register_info.db_shader_control.primitive_ordered_pixel_shader = primitive_ordered_pixel_shader.value<bool>();
            primitive_ordered_pixel_shader.Destroy();

            MDNode stencil_op_val_export_enable                                   = db_shader_control[".stencil_op_val_export_enable"];
            graphics_register_info.db_shader_control.stencil_op_val_export_enable = stencil_op_val_export_enable.value<bool>();
            stencil_op_val_export_enable.Destroy();

            MDNode stencil_test_val_export_enable                                   = db_shader_control[".stencil_test_val_export_enable"];
            graphics_register_info.db_shader_control.stencil_test_val_export_enable = stencil_test_val_export_enable.value<bool>();
            stencil_test_val_export_enable.Destroy();

            MDNode z_export_enable                                   = db_shader_control[".z_export_enable"];
            graphics_register_info.db_shader_control.z_export_enable = z_export_enable.value<bool>();
            z_export_enable.Destroy();

            MDNode z_order                                   = db_shader_control[".z_order"];
            graphics_register_info.db_shader_control.z_order = z_order.value<uint8_t>();
            z_order.Destroy();

            db_shader_control.Destroy();
        }

        MDNode es_vgpr_comp_cnt                 = graphics_registers[kPalMdTagEsVgprCompCnt];
        graphics_register_info.es_vgpr_comp_cnt = es_vgpr_comp_cnt.value<uint8_t>();
        es_vgpr_comp_cnt.Destroy();

        MDNode ge_ngg_subgrp_cntl = graphics_registers[kPalMdTagGeNggSubgrpCntl];
        if (ge_ngg_subgrp_cntl.IsValid())
        {
            MDNode prim_amp_factor                                    = ge_ngg_subgrp_cntl[".prim_amp_factor"];
            graphics_register_info.ge_ngg_subgrp_cntl.prim_amp_factor = prim_amp_factor.value<uint16_t>();
            prim_amp_factor.Destroy();

            MDNode threads_per_subgroup                                    = ge_ngg_subgrp_cntl[".threads_per_subgroup"];
            graphics_register_info.ge_ngg_subgrp_cntl.threads_per_subgroup = threads_per_subgroup.value<uint16_t>();
            threads_per_subgroup.Destroy();

            ge_ngg_subgrp_cntl.Destroy();
        }

        MDNode gs_vgpr_comp_cnt                 = graphics_registers[kPalMdTagGsVgprCompCnt];
        graphics_register_info.gs_vgpr_comp_cnt = gs_vgpr_comp_cnt.value<uint8_t>();
        gs_vgpr_comp_cnt.Destroy();

        MDNode ia_multi_vgt_param = graphics_registers[kPalMdTagIaMultiVgtParam];
        if (ia_multi_vgt_param.IsValid())
        {
            MDNode partial_es_wave_on                                    = ia_multi_vgt_param[".partial_es_wave_on"];
            graphics_register_info.ia_multi_vgt_param.partial_es_wave_on = partial_es_wave_on.value<bool>();
            partial_es_wave_on.Destroy();

            MDNode partial_vs_wave_on                                    = ia_multi_vgt_param[".partial_vs_wave_on"];
            graphics_register_info.ia_multi_vgt_param.partial_vs_wave_on = partial_vs_wave_on.value<bool>();
            partial_vs_wave_on.Destroy();

            MDNode primgroup_size                                     = ia_multi_vgt_param[".primgroup_size"];
            graphics_register_info.ia_multi_vgt_param.prim_group_size = primgroup_size.value<uint16_t>();
            primgroup_size.Destroy();

            MDNode switch_on_eoi                                    = ia_multi_vgt_param[".switch_on_eoi"];
            graphics_register_info.ia_multi_vgt_param.switch_on_eoi = switch_on_eoi.value<bool>();
            switch_on_eoi.Destroy();

            MDNode switch_on_eop                                    = ia_multi_vgt_param[".switch_on_eop"];
            graphics_register_info.ia_multi_vgt_param.switch_on_eop = switch_on_eop.value<bool>();
            switch_on_eop.Destroy();

            ia_multi_vgt_param.Destroy();
        }

        MDNode max_verts_per_subgroup                 = graphics_registers[kPalMdTagMaxVertsPerSubgroup];
        graphics_register_info.max_verts_per_subgroup = max_verts_per_subgroup.value<uint16_t>();
        max_verts_per_subgroup.Destroy();

        MDNode pa_cl_clip_cntl = graphics_registers[kPalMdTagPaClClipCntl];
        if (pa_cl_clip_cntl.IsValid())
        {
            MDNode clip_disable                                 = pa_cl_clip_cntl[".clip_disable"];
            graphics_register_info.pa_cl_clip_cntl.clip_disable = clip_disable.value<bool>();
            clip_disable.Destroy();

            MDNode dx_linear_attr_clip_ena                                 = pa_cl_clip_cntl[".dx_linear_attr_clip_ena"];
            graphics_register_info.pa_cl_clip_cntl.dx_linear_attr_clip_ena = dx_linear_attr_clip_ena.value<bool>();
            dx_linear_attr_clip_ena.Destroy();

            MDNode rasterization_kill                                 = pa_cl_clip_cntl[".rasterization_kill"];
            graphics_register_info.pa_cl_clip_cntl.rasterization_kill = rasterization_kill.value<bool>();
            rasterization_kill.Destroy();

            MDNode user_clip_plane0_ena                                  = pa_cl_clip_cntl[".user_clip_plane0_ena"];
            graphics_register_info.pa_cl_clip_cntl.user_clip_plane_0_ena = user_clip_plane0_ena.value<bool>();
            user_clip_plane0_ena.Destroy();

            MDNode user_clip_plane1_ena                                  = pa_cl_clip_cntl[".user_clip_plane1_ena"];
            graphics_register_info.pa_cl_clip_cntl.user_clip_plane_1_ena = user_clip_plane1_ena.value<bool>();
            user_clip_plane1_ena.Destroy();

            MDNode user_clip_plane2_ena                                  = pa_cl_clip_cntl[".user_clip_plane2_ena"];
            graphics_register_info.pa_cl_clip_cntl.user_clip_plane_2_ena = user_clip_plane2_ena.value<bool>();
            user_clip_plane2_ena.Destroy();

            MDNode user_clip_plane3_ena                                  = pa_cl_clip_cntl[".user_clip_plane3_ena"];
            graphics_register_info.pa_cl_clip_cntl.user_clip_plane_3_ena = user_clip_plane3_ena.value<bool>();
            user_clip_plane3_ena.Destroy();

            MDNode user_clip_plane4_ena                                  = pa_cl_clip_cntl[".user_clip_plane4_ena"];
            graphics_register_info.pa_cl_clip_cntl.user_clip_plane_4_ena = user_clip_plane4_ena.value<bool>();
            user_clip_plane4_ena.Destroy();

            MDNode user_clip_plane5_ena                                  = pa_cl_clip_cntl[".user_clip_plane5_ena"];
            graphics_register_info.pa_cl_clip_cntl.user_clip_plane_5_ena = user_clip_plane5_ena.value<bool>();
            user_clip_plane5_ena.Destroy();

            MDNode zclip_far_disable                                  = pa_cl_clip_cntl[".zclip_far_disable"];
            graphics_register_info.pa_cl_clip_cntl.z_clip_far_disable = zclip_far_disable.value<bool>();
            zclip_far_disable.Destroy();

            MDNode zclip_near_disable                                  = pa_cl_clip_cntl[".zclip_near_disable"];
            graphics_register_info.pa_cl_clip_cntl.z_clip_near_disable = zclip_near_disable.value<bool>();
            zclip_near_disable.Destroy();

            pa_cl_clip_cntl.Destroy();
        }

        MDNode pa_cl_vs_out_cntl = graphics_registers[kPalMdTagPaClVsOutCntl];
        if (pa_cl_vs_out_cntl.IsValid())
        {
            MDNode bypass_prim_rate_combiner                                   = pa_cl_vs_out_cntl[".bypass_prim_rate_combiner"];
            graphics_register_info.pa_cl_vs_out_cntl.bypass_prim_rate_combiner = bypass_prim_rate_combiner.value<bool>();
            bypass_prim_rate_combiner.Destroy();

            MDNode bypass_vtx_rate_combiner                                   = pa_cl_vs_out_cntl[".bypass_vtx_rate_combiner"];
            graphics_register_info.pa_cl_vs_out_cntl.bypass_vtx_rate_combiner = bypass_vtx_rate_combiner.value<bool>();
            bypass_vtx_rate_combiner.Destroy();

            MDNode clip_dist_ena_0                                   = pa_cl_vs_out_cntl[".clip_dist_ena_0"];
            graphics_register_info.pa_cl_vs_out_cntl.clip_dist_ena_0 = clip_dist_ena_0.value<bool>();
            clip_dist_ena_0.Destroy();

            MDNode clip_dist_ena_1                                   = pa_cl_vs_out_cntl[".clip_dist_ena_1"];
            graphics_register_info.pa_cl_vs_out_cntl.clip_dist_ena_1 = clip_dist_ena_1.value<bool>();
            clip_dist_ena_1.Destroy();

            MDNode clip_dist_ena_2                                   = pa_cl_vs_out_cntl[".clip_dist_ena_2"];
            graphics_register_info.pa_cl_vs_out_cntl.clip_dist_ena_2 = clip_dist_ena_2.value<bool>();
            clip_dist_ena_2.Destroy();

            MDNode clip_dist_ena_3                                   = pa_cl_vs_out_cntl[".clip_dist_ena_3"];
            graphics_register_info.pa_cl_vs_out_cntl.clip_dist_ena_3 = clip_dist_ena_3.value<bool>();
            clip_dist_ena_3.Destroy();

            MDNode clip_dist_ena_4                                   = pa_cl_vs_out_cntl[".clip_dist_ena_4"];
            graphics_register_info.pa_cl_vs_out_cntl.clip_dist_ena_4 = clip_dist_ena_4.value<bool>();
            clip_dist_ena_4.Destroy();

            MDNode clip_dist_ena_5                                   = pa_cl_vs_out_cntl[".clip_dist_ena_5"];
            graphics_register_info.pa_cl_vs_out_cntl.clip_dist_ena_5 = clip_dist_ena_5.value<bool>();
            clip_dist_ena_5.Destroy();

            MDNode clip_dist_ena_6                                   = pa_cl_vs_out_cntl[".clip_dist_ena_6"];
            graphics_register_info.pa_cl_vs_out_cntl.clip_dist_ena_6 = clip_dist_ena_6.value<bool>();
            clip_dist_ena_6.Destroy();

            MDNode clip_dist_ena_7                                   = pa_cl_vs_out_cntl[".clip_dist_ena_7"];
            graphics_register_info.pa_cl_vs_out_cntl.clip_dist_ena_7 = clip_dist_ena_7.value<bool>();
            clip_dist_ena_7.Destroy();

            MDNode cull_dist_ena_0                                   = pa_cl_vs_out_cntl[".cull_dist_ena_0"];
            graphics_register_info.pa_cl_vs_out_cntl.cull_dist_ena_0 = cull_dist_ena_0.value<bool>();
            cull_dist_ena_0.Destroy();

            MDNode cull_dist_ena_1                                   = pa_cl_vs_out_cntl[".cull_dist_ena_1"];
            graphics_register_info.pa_cl_vs_out_cntl.cull_dist_ena_1 = cull_dist_ena_1.value<bool>();
            cull_dist_ena_1.Destroy();

            MDNode cull_dist_ena_2                                   = pa_cl_vs_out_cntl[".cull_dist_ena_2"];
            graphics_register_info.pa_cl_vs_out_cntl.cull_dist_ena_2 = cull_dist_ena_2.value<bool>();
            cull_dist_ena_2.Destroy();

            MDNode cull_dist_ena_3                                   = pa_cl_vs_out_cntl[".cull_dist_ena_3"];
            graphics_register_info.pa_cl_vs_out_cntl.cull_dist_ena_3 = cull_dist_ena_3.value<bool>();
            cull_dist_ena_3.Destroy();

            MDNode cull_dist_ena_4                                   = pa_cl_vs_out_cntl[".cull_dist_ena_4"];
            graphics_register_info.pa_cl_vs_out_cntl.cull_dist_ena_4 = cull_dist_ena_4.value<bool>();
            cull_dist_ena_4.Destroy();

            MDNode cull_dist_ena_5                                   = pa_cl_vs_out_cntl[".cull_dist_ena_5"];
            graphics_register_info.pa_cl_vs_out_cntl.cull_dist_ena_5 = cull_dist_ena_5.value<bool>();
            cull_dist_ena_5.Destroy();

            MDNode cull_dist_ena_6                                   = pa_cl_vs_out_cntl[".cull_dist_ena_6"];
            graphics_register_info.pa_cl_vs_out_cntl.cull_dist_ena_6 = cull_dist_ena_6.value<bool>();
            cull_dist_ena_6.Destroy();

            MDNode cull_dist_ena_7                                   = pa_cl_vs_out_cntl[".cull_dist_ena_7"];
            graphics_register_info.pa_cl_vs_out_cntl.cull_dist_ena_7 = cull_dist_ena_7.value<bool>();
            cull_dist_ena_7.Destroy();

            MDNode use_vtx_edge_flag                                   = pa_cl_vs_out_cntl[".use_vtx_edge_flag"];
            graphics_register_info.pa_cl_vs_out_cntl.use_vtx_edge_flag = use_vtx_edge_flag.value<bool>();
            use_vtx_edge_flag.Destroy();

            MDNode use_vtx_fsr_select                                   = pa_cl_vs_out_cntl[".use_vtx_fsr_select"];
            graphics_register_info.pa_cl_vs_out_cntl.use_vtx_fsr_select = use_vtx_fsr_select.value<bool>();
            use_vtx_fsr_select.Destroy();

            MDNode use_vtx_line_width                                   = pa_cl_vs_out_cntl[".use_vtx_line_width"];
            graphics_register_info.pa_cl_vs_out_cntl.use_vtx_line_width = use_vtx_line_width.value<bool>();
            use_vtx_line_width.Destroy();

            MDNode use_vtx_point_size                                   = pa_cl_vs_out_cntl[".use_vtx_point_size"];
            graphics_register_info.pa_cl_vs_out_cntl.use_vtx_point_size = use_vtx_point_size.value<bool>();
            use_vtx_point_size.Destroy();

            MDNode use_vtx_render_target_indx                                   = pa_cl_vs_out_cntl[".use_vtx_render_target_indx"];
            graphics_register_info.pa_cl_vs_out_cntl.use_vtx_render_target_indx = use_vtx_render_target_indx.value<bool>();
            use_vtx_render_target_indx.Destroy();

            MDNode use_vtx_viewport_indx                                   = pa_cl_vs_out_cntl[".use_vtx_viewport_indx"];
            graphics_register_info.pa_cl_vs_out_cntl.use_vtx_viewport_indx = use_vtx_viewport_indx.value<bool>();
            use_vtx_viewport_indx.Destroy();

            MDNode use_vtx_vrs_rate                                   = pa_cl_vs_out_cntl[".use_vtx_vrs_rate"];
            graphics_register_info.pa_cl_vs_out_cntl.use_vtx_vrs_rate = use_vtx_vrs_rate.value<bool>();
            use_vtx_vrs_rate.Destroy();

            MDNode vs_out_cc_dist0_vec_ena                                    = pa_cl_vs_out_cntl[".vs_out_cc_dist0_vec_ena"];
            graphics_register_info.pa_cl_vs_out_cntl.vs_out_cc_dist_0_vec_ena = vs_out_cc_dist0_vec_ena.value<bool>();
            vs_out_cc_dist0_vec_ena.Destroy();

            MDNode vs_out_cc_dist1_vec_ena                                    = pa_cl_vs_out_cntl[".vs_out_cc_dist1_vec_ena"];
            graphics_register_info.pa_cl_vs_out_cntl.vs_out_cc_dist_1_vec_ena = vs_out_cc_dist1_vec_ena.value<bool>();
            vs_out_cc_dist1_vec_ena.Destroy();

            MDNode vs_out_misc_side_bus_ena                                   = pa_cl_vs_out_cntl[".vs_out_misc_side_bus_ena"];
            graphics_register_info.pa_cl_vs_out_cntl.vs_out_misc_side_bus_ena = vs_out_misc_side_bus_ena.value<bool>();
            vs_out_misc_side_bus_ena.Destroy();

            MDNode vs_out_misc_vec_ena                                   = pa_cl_vs_out_cntl[".vs_out_misc_vec_ena"];
            graphics_register_info.pa_cl_vs_out_cntl.vs_out_misc_vec_ena = vs_out_misc_vec_ena.value<bool>();
            vs_out_misc_vec_ena.Destroy();

            pa_cl_vs_out_cntl.Destroy();
        }

        MDNode pa_cl_vte_cntl = graphics_registers[kPalMdTagPaClVteCntl];
        if (pa_cl_vte_cntl.IsValid())
        {
            MDNode vtx_w0_fmt                                = pa_cl_vte_cntl[".vtx_w0_fmt"];
            graphics_register_info.pa_cl_vte_cntl.vtx_w0_fmt = vtx_w0_fmt.value<bool>();
            vtx_w0_fmt.Destroy();

            MDNode vtx_xy_fmt                                = pa_cl_vte_cntl[".vtx_xy_fmt"];
            graphics_register_info.pa_cl_vte_cntl.vtx_xy_fmt = vtx_xy_fmt.value<bool>();
            vtx_xy_fmt.Destroy();

            MDNode vtx_z_fmt                                = pa_cl_vte_cntl[".vtx_z_fmt"];
            graphics_register_info.pa_cl_vte_cntl.vtx_z_fmt = vtx_z_fmt.value<bool>();
            vtx_z_fmt.Destroy();

            MDNode x_offset_ena                                = pa_cl_vte_cntl[".x_offset_ena"];
            graphics_register_info.pa_cl_vte_cntl.x_offset_ena = x_offset_ena.value<bool>();
            x_offset_ena.Destroy();

            MDNode x_scale_ena                                = pa_cl_vte_cntl[".x_scale_ena"];
            graphics_register_info.pa_cl_vte_cntl.x_scale_ena = x_scale_ena.value<bool>();
            x_scale_ena.Destroy();

            MDNode y_offset_ena                                = pa_cl_vte_cntl[".y_offset_ena"];
            graphics_register_info.pa_cl_vte_cntl.y_offset_ena = y_offset_ena.value<bool>();
            y_offset_ena.Destroy();

            MDNode y_scale_ena                                = pa_cl_vte_cntl[".y_scale_ena"];
            graphics_register_info.pa_cl_vte_cntl.y_scale_ena = y_scale_ena.value<bool>();
            y_scale_ena.Destroy();

            MDNode z_offset_ena                                = pa_cl_vte_cntl[".z_offset_ena"];
            graphics_register_info.pa_cl_vte_cntl.z_offset_ena = z_offset_ena.value<bool>();
            z_offset_ena.Destroy();

            MDNode z_scale_ena                                = pa_cl_vte_cntl[".z_scale_ena"];
            graphics_register_info.pa_cl_vte_cntl.z_scale_ena = z_scale_ena.value<bool>();
            z_scale_ena.Destroy();

            pa_cl_vte_cntl.Destroy();
        }

        MDNode pa_sc_shader_control = graphics_registers[kPalMdTagPaScShaderControl];
        if (pa_sc_shader_control.IsValid())
        {
            MDNode load_collision_waveid                                       = pa_sc_shader_control[".load_collision_waveid"];
            graphics_register_info.pa_sc_shader_control.load_collision_wave_id = load_collision_waveid.value<bool>();
            load_collision_waveid.Destroy();

            MDNode load_intrawave_collision                                    = pa_sc_shader_control[".load_intrawave_collision"];
            graphics_register_info.pa_sc_shader_control.load_collision_wave_id = load_intrawave_collision.value<bool>();
            load_intrawave_collision.Destroy();

            MDNode wave_break_region_size                                      = pa_sc_shader_control[".wave_break_region_size"];
            graphics_register_info.pa_sc_shader_control.wave_break_region_size = wave_break_region_size.value<uint8_t>();
            wave_break_region_size.Destroy();

            pa_sc_shader_control.Destroy();
        }

        MDNode pa_su_vtx_cntl = graphics_registers[kPalMdTagPaSuVtxCntl];
        if (pa_su_vtx_cntl.IsValid())
        {
            MDNode pix_center                                = pa_su_vtx_cntl[".pix_center"];
            graphics_register_info.pa_su_vtx_cntl.pix_center = pix_center.value<bool>();
            pix_center.Destroy();

            MDNode quant_mode                                = pa_su_vtx_cntl[".quant_mode"];
            graphics_register_info.pa_su_vtx_cntl.quant_mode = quant_mode.value<uint8_t>();
            quant_mode.Destroy();

            MDNode round_mode                                = pa_su_vtx_cntl[".round_mode"];
            graphics_register_info.pa_su_vtx_cntl.round_mode = round_mode.value<uint8_t>();
            round_mode.Destroy();

            pa_su_vtx_cntl.Destroy();
        }

        MDNode ps_extra_lds_size                 = graphics_registers[kPalMdTagPsExtraLdsSize];
        graphics_register_info.ps_extra_lds_size = ps_extra_lds_size.value<uint32_t>();
        ps_extra_lds_size.Destroy();

        MDNode ps_iter_sample                 = graphics_registers[kPalMdTagPsIterSample];
        graphics_register_info.ps_iter_sample = ps_iter_sample.value<bool>();
        ps_iter_sample.Destroy();

        MDNode ps_load_provoking_vtx                 = graphics_registers[kPalMdTagPsLoadProvokingVtx];
        graphics_register_info.ps_load_provoking_vtx = ps_load_provoking_vtx.value<bool>();
        ps_load_provoking_vtx.Destroy();

        MDNode ps_wave_cnt_en                 = graphics_registers[kPalMdTagPsWaveCntEn];
        graphics_register_info.ps_wave_cnt_en = ps_wave_cnt_en.value<bool>();
        ps_wave_cnt_en.Destroy();

        MDNode spi_baryc_cntl = graphics_registers[kPalMdTagSpiBarycCntl];
        if (spi_baryc_cntl.IsValid())
        {
            MDNode front_face_all_bits                                = spi_baryc_cntl[".front_face_all_bits"];
            graphics_register_info.spi_baryc_cntl.front_face_all_bits = front_face_all_bits.value<bool>();
            front_face_all_bits.Destroy();

            MDNode pos_float_location                                = spi_baryc_cntl[".pos_float_location"];
            graphics_register_info.spi_baryc_cntl.pos_float_location = pos_float_location.value<uint8_t>();
            pos_float_location.Destroy();

            spi_baryc_cntl.Destroy();
        }

        MDNode spi_interp_control = graphics_registers[kPalMdTagSpiInterpControl];
        if (spi_interp_control.IsValid())
        {
            MDNode point_sprite_ena                                    = spi_interp_control[".point_sprite_ena"];
            graphics_register_info.spi_interp_control.point_sprite_ena = point_sprite_ena.value<bool>();
            point_sprite_ena.Destroy();

            spi_interp_control.Destroy();
        }

        MDNode spi_ps_in_control = graphics_registers[kPalMdTagSpiPsInControl];
        if (spi_ps_in_control.IsValid())
        {
            MDNode bc_optimize_disable                                   = spi_ps_in_control[".bc_optimize_disable"];
            graphics_register_info.spi_ps_in_control.bc_optimize_disable = bc_optimize_disable.value<uint8_t>();
            bc_optimize_disable.Destroy();

            MDNode late_pc_dealloc                                   = spi_ps_in_control[".late_pc_dealloc"];
            graphics_register_info.spi_ps_in_control.late_pc_dealloc = late_pc_dealloc.value<bool>();
            late_pc_dealloc.Destroy();

            MDNode num_interps                                   = spi_ps_in_control[".num_interps"];
            graphics_register_info.spi_ps_in_control.num_interps = num_interps.value<uint8_t>();
            num_interps.Destroy();

            MDNode num_prim_interp                                   = spi_ps_in_control[".num_prim_interp"];
            graphics_register_info.spi_ps_in_control.num_prim_interp = num_prim_interp.value<uint8_t>();
            num_prim_interp.Destroy();

            MDNode offchip_param_en                                   = spi_ps_in_control[".offchip_param_en"];
            graphics_register_info.spi_ps_in_control.offchip_param_en = offchip_param_en.value<bool>();
            offchip_param_en.Destroy();

            MDNode param_gen                                   = spi_ps_in_control[".param_gen"];
            graphics_register_info.spi_ps_in_control.param_gen = param_gen.value<bool>();
            param_gen.Destroy();

            spi_ps_in_control.Destroy();
        }

        MDNode spi_ps_input = graphics_registers[kPalMdTagSpiPsInputAddr];
        if (spi_ps_input.IsValid())
        {
            MDNode ancillary_ena                              = spi_ps_input[".ancillary_ena"];
            graphics_register_info.spi_ps_input.ancillary_ena = ancillary_ena.value<bool>();
            ancillary_ena.Destroy();

            MDNode front_face_ena                              = spi_ps_input[".front_face_ena"];
            graphics_register_info.spi_ps_input.front_face_ena = front_face_ena.value<bool>();
            front_face_ena.Destroy();

            MDNode line_stipple_tex_ena                              = spi_ps_input[".line_stipple_tex_ena"];
            graphics_register_info.spi_ps_input.line_stipple_tex_ena = line_stipple_tex_ena.value<bool>();
            line_stipple_tex_ena.Destroy();

            MDNode linear_center_ena                              = spi_ps_input[".linear_center_ena"];
            graphics_register_info.spi_ps_input.linear_center_ena = linear_center_ena.value<bool>();
            linear_center_ena.Destroy();

            MDNode linear_centroid_ena                              = spi_ps_input[".linear_centroid_ena"];
            graphics_register_info.spi_ps_input.linear_centroid_ena = linear_centroid_ena.value<bool>();
            linear_centroid_ena.Destroy();

            MDNode linear_sample_ena                              = spi_ps_input[".linear_sample_ena"];
            graphics_register_info.spi_ps_input.linear_sample_ena = linear_sample_ena.value<bool>();
            linear_sample_ena.Destroy();

            MDNode persp_center_ena                              = spi_ps_input[".persp_center_ena"];
            graphics_register_info.spi_ps_input.persp_center_ena = persp_center_ena.value<bool>();
            persp_center_ena.Destroy();

            MDNode persp_centroid_ena                              = spi_ps_input[".persp_centroid_ena"];
            graphics_register_info.spi_ps_input.persp_centroid_ena = persp_centroid_ena.value<bool>();
            persp_centroid_ena.Destroy();

            MDNode persp_pull_model_ena                              = spi_ps_input[".persp_pull_model_ena"];
            graphics_register_info.spi_ps_input.persp_pull_model_ena = persp_pull_model_ena.value<bool>();
            persp_pull_model_ena.Destroy();

            MDNode persp_sample_ena                              = spi_ps_input[".persp_sample_ena"];
            graphics_register_info.spi_ps_input.persp_sample_ena = persp_sample_ena.value<bool>();
            persp_sample_ena.Destroy();

            MDNode pos_fixed_pt_ena                              = spi_ps_input[".pos_fixed_pt_ena"];
            graphics_register_info.spi_ps_input.pos_fixed_pt_ena = pos_fixed_pt_ena.value<bool>();
            pos_fixed_pt_ena.Destroy();

            MDNode pos_w_float_ena                              = spi_ps_input[".pos_w_float_ena"];
            graphics_register_info.spi_ps_input.pos_w_float_ena = pos_w_float_ena.value<bool>();
            pos_w_float_ena.Destroy();

            MDNode pos_x_float_ena                              = spi_ps_input[".pos_x_float_ena"];
            graphics_register_info.spi_ps_input.pos_x_float_ena = pos_x_float_ena.value<bool>();
            pos_x_float_ena.Destroy();

            MDNode pos_y_float_ena                              = spi_ps_input[".pos_y_float_ena"];
            graphics_register_info.spi_ps_input.pos_y_float_ena = pos_y_float_ena.value<bool>();
            pos_y_float_ena.Destroy();

            MDNode pos_z_float_ena                              = spi_ps_input[".pos_z_float_ena"];
            graphics_register_info.spi_ps_input.pos_z_float_ena = pos_z_float_ena.value<bool>();
            pos_z_float_ena.Destroy();

            MDNode sample_coverage_ena                              = spi_ps_input[".sample_coverage_ena"];
            graphics_register_info.spi_ps_input.sample_coverage_ena = sample_coverage_ena.value<bool>();
            sample_coverage_ena.Destroy();

            spi_ps_input.Destroy();
        }

        MDNode spi_ps_input_cntl = graphics_registers[kPalMdTagSpiPsInputCntl];
        if (spi_ps_input_cntl.IsValid())
        {
            size_t num_spi_ps_input_cntl = spi_ps_input_cntl.size();

            for (size_t i = 0; i < num_spi_ps_input_cntl; i++)
            {
                MDNode spi_ps_input_cntl_node = spi_ps_input_cntl[i];

                MDNode attr0_valid                                       = spi_ps_input_cntl_node[".attr0_valid"];
                graphics_register_info.spi_ps_input_cntl[i].attr_0_valid = attr0_valid.value<bool>();
                attr0_valid.Destroy();

                MDNode attr1_valid                                       = spi_ps_input_cntl_node[".attr1_valid"];
                graphics_register_info.spi_ps_input_cntl[i].attr_1_valid = attr1_valid.value<bool>();
                attr1_valid.Destroy();

                MDNode default_val                                      = spi_ps_input_cntl_node[".default_val"];
                graphics_register_info.spi_ps_input_cntl[i].default_val = default_val.value<uint8_t>();
                default_val.Destroy();

                MDNode flat_shade                                      = spi_ps_input_cntl_node[".flat_shade"];
                graphics_register_info.spi_ps_input_cntl[i].flat_shade = flat_shade.value<bool>();
                flat_shade.Destroy();

                MDNode fp16_interp_mode                                      = spi_ps_input_cntl_node[".fp16_interp_mode"];
                graphics_register_info.spi_ps_input_cntl[i].fp16_interp_mode = fp16_interp_mode.value<bool>();
                fp16_interp_mode.Destroy();

                MDNode offset                                      = spi_ps_input_cntl_node[".offset"];
                graphics_register_info.spi_ps_input_cntl[i].offset = offset.value<uint8_t>();
                offset.Destroy();

                MDNode prim_attr                                      = spi_ps_input_cntl_node[".prim_attr"];
                graphics_register_info.spi_ps_input_cntl[i].prim_attr = prim_attr.value<bool>();
                prim_attr.Destroy();

                MDNode pt_sprite_tex                                      = spi_ps_input_cntl_node[".pt_sprite_tex"];
                graphics_register_info.spi_ps_input_cntl[i].pt_sprite_tex = pt_sprite_tex.value<bool>();
                pt_sprite_tex.Destroy();

                MDNode rotate_pc_ptr                                      = spi_ps_input_cntl_node[".rotate_pc_ptr"];
                graphics_register_info.spi_ps_input_cntl[i].rotate_pc_ptr = rotate_pc_ptr.value<bool>();
                rotate_pc_ptr.Destroy();

                spi_ps_input_cntl_node.Destroy();
            }

            spi_ps_input_cntl.Destroy();
        }

        MDNode spi_ps_input_ena = graphics_registers[kPalMdTagSpiPsInputEna];
        if (spi_ps_input_ena.IsValid())
        {
            MDNode ancillary_ena                              = spi_ps_input_ena[".ancillary_ena"];
            graphics_register_info.spi_ps_input.ancillary_ena = ancillary_ena.value<bool>();
            ancillary_ena.Destroy();

            MDNode front_face_ena                              = spi_ps_input_ena[".front_face_ena"];
            graphics_register_info.spi_ps_input.front_face_ena = front_face_ena.value<bool>();
            front_face_ena.Destroy();

            MDNode line_stipple_tex_ena                              = spi_ps_input_ena[".line_stipple_tex_ena"];
            graphics_register_info.spi_ps_input.line_stipple_tex_ena = line_stipple_tex_ena.value<bool>();
            line_stipple_tex_ena.Destroy();

            MDNode linear_center_ena                              = spi_ps_input_ena[".linear_center_ena"];
            graphics_register_info.spi_ps_input.linear_center_ena = linear_center_ena.value<bool>();
            linear_center_ena.Destroy();

            MDNode linear_centroid_ena                              = spi_ps_input_ena[".linear_centroid_ena"];
            graphics_register_info.spi_ps_input.linear_centroid_ena = linear_centroid_ena.value<bool>();
            linear_centroid_ena.Destroy();

            MDNode linear_sample_ena                              = spi_ps_input_ena[".linear_sample_ena"];
            graphics_register_info.spi_ps_input.linear_sample_ena = linear_sample_ena.value<bool>();
            linear_sample_ena.Destroy();

            MDNode persp_center_ena                              = spi_ps_input_ena[".persp_center_ena"];
            graphics_register_info.spi_ps_input.persp_center_ena = persp_center_ena.value<bool>();
            persp_center_ena.Destroy();

            MDNode persp_centroid_ena                              = spi_ps_input_ena[".persp_centroid_ena"];
            graphics_register_info.spi_ps_input.persp_centroid_ena = persp_centroid_ena.value<bool>();
            persp_centroid_ena.Destroy();

            MDNode persp_pull_model_ena                              = spi_ps_input_ena[".persp_pull_model_ena"];
            graphics_register_info.spi_ps_input.persp_pull_model_ena = persp_pull_model_ena.value<bool>();
            persp_pull_model_ena.Destroy();

            MDNode persp_sample_ena                              = spi_ps_input_ena[".persp_sample_ena"];
            graphics_register_info.spi_ps_input.persp_sample_ena = persp_sample_ena.value<bool>();
            persp_sample_ena.Destroy();

            MDNode pos_fixed_pt_ena                              = spi_ps_input_ena[".pos_fixed_pt_ena"];
            graphics_register_info.spi_ps_input.pos_fixed_pt_ena = pos_fixed_pt_ena.value<bool>();
            pos_fixed_pt_ena.Destroy();

            MDNode pos_w_float_ena                              = spi_ps_input_ena[".pos_w_float_ena"];
            graphics_register_info.spi_ps_input.pos_w_float_ena = pos_w_float_ena.value<bool>();
            pos_w_float_ena.Destroy();

            MDNode pos_x_float_ena                              = spi_ps_input_ena[".pos_x_float_ena"];
            graphics_register_info.spi_ps_input.pos_x_float_ena = pos_x_float_ena.value<bool>();
            pos_x_float_ena.Destroy();

            MDNode pos_y_float_ena                              = spi_ps_input_ena[".pos_y_float_ena"];
            graphics_register_info.spi_ps_input.pos_y_float_ena = pos_y_float_ena.value<bool>();
            pos_y_float_ena.Destroy();

            MDNode pos_z_float_ena                              = spi_ps_input_ena[".pos_z_float_ena"];
            graphics_register_info.spi_ps_input.pos_z_float_ena = pos_z_float_ena.value<bool>();
            pos_z_float_ena.Destroy();

            MDNode sample_coverage_ena                              = spi_ps_input_ena[".sample_coverage_ena"];
            graphics_register_info.spi_ps_input.sample_coverage_ena = sample_coverage_ena.value<bool>();
            sample_coverage_ena.Destroy();

            spi_ps_input_ena.Destroy();
        }

        MDNode spi_shader_col_format = graphics_registers[kPalMdTagSpiShaderColFormat];
        if (spi_shader_col_format.IsValid())
        {
            MDNode col_0_export_format                                       = spi_shader_col_format[".col_0_export_format"];
            graphics_register_info.spi_shader_col_format.col_0_export_format = col_0_export_format.value<uint8_t>();
            col_0_export_format.Destroy();

            MDNode col_1_export_format                                       = spi_shader_col_format[".col_1_export_format"];
            graphics_register_info.spi_shader_col_format.col_1_export_format = col_1_export_format.value<uint8_t>();
            col_1_export_format.Destroy();

            MDNode col_2_export_format                                       = spi_shader_col_format[".col_2_export_format"];
            graphics_register_info.spi_shader_col_format.col_2_export_format = col_2_export_format.value<uint8_t>();
            col_2_export_format.Destroy();

            MDNode col_3_export_format                                       = spi_shader_col_format[".col_3_export_format"];
            graphics_register_info.spi_shader_col_format.col_3_export_format = col_3_export_format.value<uint8_t>();
            col_3_export_format.Destroy();

            MDNode col_4_export_format                                       = spi_shader_col_format[".col_4_export_format"];
            graphics_register_info.spi_shader_col_format.col_4_export_format = col_4_export_format.value<uint8_t>();
            col_4_export_format.Destroy();

            MDNode col_5_export_format                                       = spi_shader_col_format[".col_5_export_format"];
            graphics_register_info.spi_shader_col_format.col_5_export_format = col_5_export_format.value<uint8_t>();
            col_5_export_format.Destroy();

            MDNode col_6_export_format                                       = spi_shader_col_format[".col_6_export_format"];
            graphics_register_info.spi_shader_col_format.col_6_export_format = col_6_export_format.value<uint8_t>();
            col_6_export_format.Destroy();

            MDNode col_7_export_format                                       = spi_shader_col_format[".col_7_export_format"];
            graphics_register_info.spi_shader_col_format.col_7_export_format = col_7_export_format.value<uint8_t>();
            col_7_export_format.Destroy();

            spi_shader_col_format.Destroy();
        }

        MDNode spi_shader_gs_meshlet_dim = graphics_registers[kPalMdTagSpiShaderGsMeshletDim];
        if (spi_shader_gs_meshlet_dim.IsValid())
        {
            MDNode num_thread_x                                           = spi_shader_gs_meshlet_dim[".num_thread_x"];
            graphics_register_info.spi_shader_gs_meshlet_dim.num_thread_x = num_thread_x.value<uint16_t>();
            num_thread_x.Destroy();

            MDNode num_thread_y                                           = spi_shader_gs_meshlet_dim[".num_thread_y"];
            graphics_register_info.spi_shader_gs_meshlet_dim.num_thread_y = num_thread_y.value<uint16_t>();
            num_thread_y.Destroy();

            MDNode num_thread_z                                           = spi_shader_gs_meshlet_dim[".num_thread_z"];
            graphics_register_info.spi_shader_gs_meshlet_dim.num_thread_z = num_thread_z.value<uint16_t>();
            num_thread_z.Destroy();

            MDNode threadgroup_size                                           = spi_shader_gs_meshlet_dim[".threadgroup_size"];
            graphics_register_info.spi_shader_gs_meshlet_dim.threadgroup_size = threadgroup_size.value<uint32_t>();
            threadgroup_size.Destroy();

            spi_shader_gs_meshlet_dim.Destroy();
        }

        MDNode spi_shader_gs_meshlet_exp_alloc = graphics_registers[kPalMdTagSpiShaderGsMeshletExpAlloc];
        if (spi_shader_gs_meshlet_exp_alloc.IsValid())
        {
            MDNode max_exp_prims                                                 = spi_shader_gs_meshlet_exp_alloc[".max_exp_prims"];
            graphics_register_info.spi_shader_gs_meshlet_exp_alloc.max_exp_prims = max_exp_prims.value<uint16_t>();
            max_exp_prims.Destroy();

            MDNode max_exp_verts                                                 = spi_shader_gs_meshlet_exp_alloc[".max_exp_verts"];
            graphics_register_info.spi_shader_gs_meshlet_exp_alloc.max_exp_verts = max_exp_verts.value<uint16_t>();
            max_exp_verts.Destroy();

            spi_shader_gs_meshlet_exp_alloc.Destroy();
        }

        MDNode spi_shader_idx_format                 = graphics_registers[kPalMdTagSpiShaderIdxFormat];
        graphics_register_info.spi_shader_idx_format = spi_shader_idx_format.value<uint8_t>();
        spi_shader_idx_format.Destroy();

        MDNode spi_shader_z_format                 = graphics_registers[kPalMdTagSpiShaderZFormat];
        graphics_register_info.spi_shader_z_format = spi_shader_z_format.value<uint8_t>();
        spi_shader_z_format.Destroy();

        MDNode spi_vs_out_config = graphics_registers[kPalMdTagSpiVsOutConfig];
        if (spi_vs_out_config.IsValid())
        {
            MDNode no_pc_export                                   = spi_vs_out_config[".no_pc_export"];
            graphics_register_info.spi_vs_out_config.no_pc_export = no_pc_export.value<bool>();
            no_pc_export.Destroy();

            MDNode prim_export_count                                   = spi_vs_out_config[".prim_export_count"];
            graphics_register_info.spi_vs_out_config.prim_export_count = prim_export_count.value<uint8_t>();
            prim_export_count.Destroy();

            MDNode vs_export_count                                   = spi_vs_out_config[".vs_export_count"];
            graphics_register_info.spi_vs_out_config.vs_export_count = vs_export_count.value<uint8_t>();
            vs_export_count.Destroy();

            spi_vs_out_config.Destroy();
        }

        MDNode vgt_draw_prim_payload_en                 = graphics_registers[kPalMdTagVgtDrawPrimPayloadEn];
        graphics_register_info.vgt_draw_prim_payload_en = vgt_draw_prim_payload_en.value<bool>();
        vgt_draw_prim_payload_en.Destroy();

        MDNode vgt_esgs_ring_itemsize                   = graphics_registers[kPalMdTagVgtEsGsRingItemSize];
        graphics_register_info.vgt_es_gs_ring_item_size = vgt_esgs_ring_itemsize.value<bool>();
        vgt_esgs_ring_itemsize.Destroy();

        MDNode vgt_gs_instance_cnt = graphics_registers[kPalMdTagVgtGsInstanceCnt];
        if (vgt_gs_instance_cnt.IsValid())
        {
            MDNode count                                     = vgt_gs_instance_cnt[".count"];
            graphics_register_info.vgt_gs_instance_cnt.count = count.value<uint8_t>();
            count.Destroy();

            MDNode en_max_vert_out_per_gs_instance                                     = vgt_gs_instance_cnt[".en_max_vert_out_per_gs_instance"];
            graphics_register_info.vgt_gs_instance_cnt.en_max_vert_out_per_gs_instance = en_max_vert_out_per_gs_instance.value<uint8_t>();
            en_max_vert_out_per_gs_instance.Destroy();

            MDNode enable                                     = vgt_gs_instance_cnt[".enable"];
            graphics_register_info.vgt_gs_instance_cnt.enable = enable.value<bool>();
            enable.Destroy();

            vgt_gs_instance_cnt.Destroy();
        }

        MDNode vgt_gs_max_vert_out                 = graphics_registers[kPalMdTagVgtGsMaxVertOut];
        graphics_register_info.vgt_gs_max_vert_out = vgt_gs_max_vert_out.value<uint16_t>();
        vgt_gs_max_vert_out.Destroy();

        MDNode vgt_gs_mode = graphics_registers[kPalMdTagVgtGsMode];
        if (vgt_gs_mode.IsValid())
        {
            MDNode cut_mode                             = vgt_gs_mode[".cut_mode"];
            graphics_register_info.vgt_gs_mode.cut_mode = cut_mode.value<uint8_t>();
            cut_mode.Destroy();

            MDNode es_write_optimize                             = vgt_gs_mode[".es_write_optimize"];
            graphics_register_info.vgt_gs_mode.es_write_optimize = es_write_optimize.value<bool>();
            es_write_optimize.Destroy();

            MDNode gs_write_optimize                             = vgt_gs_mode[".gs_write_optimize"];
            graphics_register_info.vgt_gs_mode.gs_write_optimize = gs_write_optimize.value<bool>();
            gs_write_optimize.Destroy();

            MDNode mode                             = vgt_gs_mode[".mode"];
            graphics_register_info.vgt_gs_mode.mode = mode.value<uint8_t>();
            mode.Destroy();

            MDNode onchip                              = vgt_gs_mode[".onchip"];
            graphics_register_info.vgt_gs_mode.on_chip = onchip.value<uint8_t>();
            onchip.Destroy();

            vgt_gs_mode.Destroy();
        }

        MDNode vgt_gs_onchip_cntl = graphics_registers[kPalMdTagVgtGsOnchipCntl];
        if (vgt_gs_onchip_cntl.IsValid())
        {
            MDNode es_verts_per_subgroup                                     = vgt_gs_onchip_cntl[".es_verts_per_subgroup"];
            graphics_register_info.vgt_gs_on_chip_cntl.es_verts_per_subgroup = es_verts_per_subgroup.value<uint16_t>();
            es_verts_per_subgroup.Destroy();

            MDNode gs_inst_prims_per_subgrp                                     = vgt_gs_onchip_cntl[".gs_inst_prims_per_subgrp"];
            graphics_register_info.vgt_gs_on_chip_cntl.gs_inst_prims_per_subgrp = gs_inst_prims_per_subgrp.value<uint16_t>();
            gs_inst_prims_per_subgrp.Destroy();

            MDNode gs_prims_per_subgroup                                     = vgt_gs_onchip_cntl[".gs_prims_per_subgroup"];
            graphics_register_info.vgt_gs_on_chip_cntl.gs_prims_per_subgroup = gs_prims_per_subgroup.value<uint16_t>();
            gs_prims_per_subgroup.Destroy();

            vgt_gs_onchip_cntl.Destroy();
        }

        MDNode vgt_ls_hs_config = graphics_registers[kPalMdTagVgtLsHsConfig];
        if (vgt_ls_hs_config.IsValid())
        {
            MDNode hs_num_input_cp                                  = vgt_ls_hs_config[".hs_num_input_cp"];
            graphics_register_info.vgt_ls_hs_config.hs_num_input_cp = hs_num_input_cp.value<uint8_t>();
            hs_num_input_cp.Destroy();

            MDNode hs_num_output_cp                                  = vgt_ls_hs_config[".hs_num_output_cp"];
            graphics_register_info.vgt_ls_hs_config.hs_num_output_cp = hs_num_output_cp.value<uint8_t>();
            hs_num_output_cp.Destroy();

            MDNode num_patches                                  = vgt_ls_hs_config[".num_patches"];
            graphics_register_info.vgt_ls_hs_config.num_patches = num_patches.value<uint8_t>();
            num_patches.Destroy();

            vgt_ls_hs_config.Destroy();
        }

        MDNode vgt_primitive_id_en                 = graphics_registers[kPalMdTagVgtPrimitiveIdEn];
        graphics_register_info.vgt_primitive_id_en = vgt_primitive_id_en.value<bool>();
        vgt_primitive_id_en.Destroy();

        MDNode vgt_reuse_off                 = graphics_registers[kPalMdTagVgtReuseOff];
        graphics_register_info.vgt_reuse_off = vgt_reuse_off.value<bool>();
        vgt_reuse_off.Destroy();

        MDNode vgt_shader_stages_en = graphics_registers[kPalMdTagVgtShaderStagesEn];
        if (vgt_shader_stages_en.IsValid())
        {
            MDNode dynamic_hs                                      = vgt_shader_stages_en[".dynamic_hs"];
            graphics_register_info.vgt_shader_stages_en.dynamic_hs = dynamic_hs.value<bool>();
            dynamic_hs.Destroy();

            MDNode es_stage_en                                      = vgt_shader_stages_en[".es_stage_en"];
            graphics_register_info.vgt_shader_stages_en.es_stage_en = es_stage_en.value<uint8_t>();
            es_stage_en.Destroy();

            MDNode gs_fast_launch                                      = vgt_shader_stages_en[".gs_fast_launch"];
            graphics_register_info.vgt_shader_stages_en.gs_fast_launch = gs_fast_launch.value<uint8_t>();
            gs_fast_launch.Destroy();

            MDNode gs_stage_en                                      = vgt_shader_stages_en[".gs_stage_en"];
            graphics_register_info.vgt_shader_stages_en.gs_stage_en = gs_stage_en.value<bool>();
            gs_stage_en.Destroy();

            MDNode hs_stage_en                                      = vgt_shader_stages_en[".hs_stage_en"];
            graphics_register_info.vgt_shader_stages_en.hs_stage_en = hs_stage_en.value<bool>();
            hs_stage_en.Destroy();

            MDNode ls_stage_en                                      = vgt_shader_stages_en[".ls_stage_en"];
            graphics_register_info.vgt_shader_stages_en.ls_stage_en = ls_stage_en.value<bool>();
            ls_stage_en.Destroy();

            MDNode max_primgroup_in_wave                                       = vgt_shader_stages_en[".max_primgroup_in_wave"];
            graphics_register_info.vgt_shader_stages_en.max_prim_group_in_wave = max_primgroup_in_wave.value<uint8_t>();
            max_primgroup_in_wave.Destroy();

            MDNode ngg_wave_id_en                                      = vgt_shader_stages_en[".ngg_wave_id_en"];
            graphics_register_info.vgt_shader_stages_en.ngg_wave_id_en = ngg_wave_id_en.value<bool>();
            ngg_wave_id_en.Destroy();

            MDNode ordered_id_mode                                      = vgt_shader_stages_en[".ordered_id_mode"];
            graphics_register_info.vgt_shader_stages_en.ordered_id_mode = ordered_id_mode.value<bool>();
            ordered_id_mode.Destroy();

            MDNode primgen_en                                       = vgt_shader_stages_en[".primgen_en"];
            graphics_register_info.vgt_shader_stages_en.prim_gen_en = primgen_en.value<bool>();
            primgen_en.Destroy();

            MDNode primgen_passthru_en                                       = vgt_shader_stages_en[".primgen_passthru_en"];
            graphics_register_info.vgt_shader_stages_en.prim_gen_passthru_en = primgen_passthru_en.value<bool>();
            primgen_passthru_en.Destroy();

            MDNode primgen_passthru_no_msg                                       = vgt_shader_stages_en[".primgen_passthru_no_msg"];
            graphics_register_info.vgt_shader_stages_en.prim_gen_passthru_no_msg = primgen_passthru_no_msg.value<bool>();
            primgen_passthru_no_msg.Destroy();

            MDNode vs_stage_en                                      = vgt_shader_stages_en[".vs_stage_en"];
            graphics_register_info.vgt_shader_stages_en.vs_stage_en = vs_stage_en.value<uint8_t>();
            vs_stage_en.Destroy();

            vgt_shader_stages_en.Destroy();
        }

        MDNode vgt_tf_param = graphics_registers[kPalMdTagVgtTfParam];
        if (vgt_tf_param.IsValid())
        {
            MDNode disable_donuts                              = vgt_tf_param[".disable_donuts"];
            graphics_register_info.vgt_tf_param.disable_donuts = disable_donuts.value<bool>();
            disable_donuts.Destroy();

            MDNode distribution_mode                              = vgt_tf_param[".distribution_mode"];
            graphics_register_info.vgt_tf_param.distribution_mode = distribution_mode.value<uint8_t>();
            distribution_mode.Destroy();

            MDNode num_ds_waves_per_simd                              = vgt_tf_param[".num_ds_waves_per_simd"];
            graphics_register_info.vgt_tf_param.num_ds_waves_per_simd = num_ds_waves_per_simd.value<uint8_t>();
            num_ds_waves_per_simd.Destroy();

            MDNode partitioning                              = vgt_tf_param[".partitioning"];
            graphics_register_info.vgt_tf_param.partitioning = partitioning.value<uint8_t>();
            partitioning.Destroy();

            MDNode topology                              = vgt_tf_param[".topology"];
            graphics_register_info.vgt_tf_param.topology = topology.value<uint8_t>();
            topology.Destroy();

            MDNode type                              = vgt_tf_param[".type"];
            graphics_register_info.vgt_tf_param.type = type.value<uint8_t>();
            type.Destroy();

            vgt_tf_param.Destroy();
        }

        graphics_registers.Destroy();

        return true;
    }

    bool CodeObj::ExtractPalMDRegisterInfo(Pipeline& md_pipeline_data, MDNode& ppln)
    {
        bool ret_code = true;
        // Registers.
        MDNode regs = ppln[kPalMdTagRegisters];
        if (!regs.IsValid())
        {
            ReportInvalidRequiredPalMDMapItem(shader_hw_mapping);
        }

        md_pipeline_data.num_register_writes = static_cast<uint32_t>(regs.size());
        if (regs.size() > 0)
        {
            size_t regs_alloc_size = regs.size() * sizeof(RegisterData);
            // Check if the above multiplication resulted in an overflow.
            if (regs.size() != regs_alloc_size / sizeof(RegisterData))
            {
                regs.Destroy();
                return false;
            }

            md_pipeline_data.register_data_list = (RegisterData*)malloc(regs_alloc_size);
            if (nullptr != md_pipeline_data.register_data_list)
            {
                memset(md_pipeline_data.register_data_list, 0, regs_alloc_size);

                size_t register_count = 0;

                for (const std::string& addrKey : regs.GetKeys())
                {
                    RegisterData* reg_data = &md_pipeline_data.register_data_list[register_count++];

                    MDNode temp = regs[addrKey];
                    if (!temp.IsValid())
                    {
                        ReportInvalidRequiredPalMDMapItem(temp);
                        regs.Destroy();
                        return false;
                    }

                    ret_code = convert_string_to_uint32(addrKey.c_str(), reg_data->address);

                    reg_data->data = temp.value<uint32_t>();
                    temp.Destroy();
                }
            }
        }

        regs.Destroy();

        return ret_code;
    }

    void CodeObj::SetError(amd_comgr_status_t err, const std::string& err_msg)
    {
        status_ = err;

        if (!err_msg.empty())
        {
            err_msg_ = err_msg;
        }
    }

    MDNode::MDNode(amd_comgr_metadata_node_t node, CodeObj* current_code_object)
        : handle_(node)
        , current_code_object_(current_code_object)
    {
    }

    MDNode::MDNode(int handle, CodeObj* current_code_object)
    {
        handle_.handle       = handle;
        current_code_object_ = current_code_object;
    }

    MDNode::Kind MDNode::GetKind() const
    {
        CheckValid(Kind::kNodeKindNone);
        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), Kind::kNodeKindNone);

        amd_comgr_metadata_kind_t kind   = AMD_COMGR_METADATA_KIND_NULL;
        amd_comgr_status_t        status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_kind_fn(handle_, &kind);
        CheckStatus(current_code_object_, status, Kind::kNodeKindNone);

        // TODO: Current comgr implementation returns integer values as strings.
        // That will change soon, so this code must be updated too.
        switch (kind)
        {
        case AMD_COMGR_METADATA_KIND_STRING:
            return Kind::kNodeKindString;

        case AMD_COMGR_METADATA_KIND_LIST:
            return Kind::kNodeKindList;

        case AMD_COMGR_METADATA_KIND_MAP:
            return Kind::kNodeKindMap;

        case AMD_COMGR_METADATA_KIND_NULL:

        default:
            return Kind::kNodeKindNone;
        }
    }

    MDNode MDNode::operator[](int idx) const
    {
        amd_comgr_metadata_node_t child = {0};

        if (IsValid())
        {
            if (GetKind() == Kind::kNodeKindList)
            {
                Check(ComgrEntryPoints::Instance()->EntryPointsValid(), child);
                amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_index_list_metadata_fn(handle_, idx, &child);
                CheckStatus(current_code_object_, status, child);
            }
        }

        return MDNode(child, current_code_object_);
    }

    MDNode MDNode::operator[](size_t idx) const
    {
        amd_comgr_metadata_node_t child = {0};

        if (IsValid())
        {
            if (GetKind() == Kind::kNodeKindList)
            {
                Check(ComgrEntryPoints::Instance()->EntryPointsValid(), child);
                amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_index_list_metadata_fn(handle_, idx, &child);
                CheckStatus(current_code_object_, status, child);
            }
        }

        return MDNode(child, current_code_object_);
    }

    MDNode MDNode::operator[](const std::string& key) const
    {
        amd_comgr_metadata_node_t child = {0};

        if (IsValid())
        {
            if (GetKind() == Kind::kNodeKindMap)
            {
                Check(ComgrEntryPoints::Instance()->EntryPointsValid(), child);
                amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_metadata_lookup_fn(handle_, key.c_str(), &child);
                CheckStatus(current_code_object_, status, child);
            }
        }

        return MDNode(child, current_code_object_);
    }

    MDNode MDNode::operator[](const char* key) const
    {
        amd_comgr_metadata_node_t child = {0};

        if (IsValid())
        {
            if (GetKind() == Kind::kNodeKindMap)
            {
                Check(ComgrEntryPoints::Instance()->EntryPointsValid(), child);
                amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_metadata_lookup_fn(handle_, key, &child);
                CheckStatus(current_code_object_, status, child);
            }
        }

        return MDNode(child, current_code_object_);
    }

    bool MDNode::Find(const std::string& key) const
    {
        CheckValid(false);

        if (GetKind() == Kind::kNodeKindMap)
        {
            Check(ComgrEntryPoints::Instance()->EntryPointsValid(), false);
            amd_comgr_metadata_node_t child;
            amd_comgr_status_t        status = ComgrEntryPoints::Instance()->amd_comgr_metadata_lookup_fn(handle_, key.c_str(), &child);
            ComgrEntryPoints::Instance()->amd_comgr_destroy_metadata_fn(child);
            return (status == AMD_COMGR_STATUS_SUCCESS);
        }
        else
        {
            return false;
        }
    }

    template <typename TYPE>
    TYPE MDNode::value() const
    {
        static_assert(
            std::is_arithmetic<TYPE>::value == false,
            "Arithmetic types should not fallback on this slow stringstream path as they have significantly quicker alternatives (stoi, stol, etc.).");
        // The default specialization for arithmetic types.
        CheckValid(0);
        TYPE val = (TYPE)0;

        // TODO: Current comgr implementation returns integer values as strings.
        // That will change soon, so this code must be updated too.
        if (GetKind() == Kind::kNodeKindString)
        {
            std::stringstream stream(value<std::string>());
            stream >> val;

            if (stream.fail())
            {
                current_code_object_->SetError(AMD_COMGR_STATUS_ERROR, "ERROR: Failed to convert to int32");
            }
        }

        return val;
    }

    size_t MDNode::size() const
    {
        CheckValid(0);
        amd_comgr_status_t status = AMD_COMGR_STATUS_ERROR;
        size_t             size;

        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), 0);
        switch (GetKind())
        {
        case Kind::kNodeKindList:
            status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_list_size_fn(handle_, &size);
            break;

        case Kind::kNodeKindMap:
            status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_map_size_fn(handle_, &size);
            break;

        default:
            size = 0;
        }

        CheckStatus(current_code_object_, status, 0);
        return size;
    }

    std::vector<std::string> MDNode::GetKeys() const
    {
        CheckValid({});
        Check(ComgrEntryPoints::Instance()->EntryPointsValid(), {});

        MapIterCallbackUserData user_data;
        user_data.code_object = current_code_object_;

        amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_iterate_map_metadata_fn(handle_, MapIterCallback, static_cast<void*>(&user_data));
        CheckStatus(current_code_object_, status, {});

        return user_data.keys;
    }

    bool MDNode::IsValid() const
    {
        return (handle_.handle != 0);
    }

    void MDNode::Dump()
    {
        assert(false && "ERROR: Not implemented");
    }

    void MDNode::Destroy()
    {
        if (IsValid() && ComgrEntryPoints::Instance()->EntryPointsValid())
        {
            ComgrEntryPoints::Instance()->amd_comgr_destroy_metadata_fn(handle_);
        }
    }

    template <>
    std::string MDNode::value<std::string>() const
    {
        CheckValid("");

        if (GetKind() == Kind::kNodeKindString)
        {
            Check(ComgrEntryPoints::Instance()->EntryPointsValid(), "");
            size_t             buf_size = 0;
            amd_comgr_status_t status   = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_string_fn(handle_, &buf_size, NULL);
            CheckStatus(current_code_object_, status, "");
            char* buf = new (std::nothrow) char[buf_size + 1];

            if (nullptr == buf)
            {
                assert(false && "ERROR: Unable to allocate memory for string value.");
                current_code_object_->SetError(AMD_COMGR_STATUS_ERROR, "ERROR: Unable to allocate memory for string value.");
                return "";
            }

            status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_string_fn(handle_, &buf_size, buf);

            if (status != AMD_COMGR_STATUS_SUCCESS)
            {
                current_code_object_->SetError(status);
                delete[] buf;
                return "";
            }

            std::string ret_buf(buf);
            delete[] buf;
            return ret_buf;
        }
        else
        {
            return "";
        }
    }

    template <>
    bool MDNode::value<bool>() const
    {
        return static_cast<bool>(value<uint32_t>());
    }

    template <>
    uint8_t MDNode::value<uint8_t>() const
    {
        return static_cast<uint8_t>(value<uint32_t>());
    }

    template <>
    uint16_t MDNode::value<uint16_t>() const
    {
        return static_cast<uint16_t>(value<uint32_t>());
    }

    template <>
    uint32_t MDNode::value<uint32_t>() const
    {
        CheckValid(0);

        if (GetKind() == Kind::kNodeKindString)
        {
            Check(ComgrEntryPoints::Instance()->EntryPointsValid(), 0);
            size_t             buf_size = 0;
            char               buf[256] = {};
            amd_comgr_status_t status   = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_string_fn(handle_, &buf_size, NULL);
            CheckStatus(current_code_object_, status, 0);

            if (buf_size >= 256)
            {
                assert(false && "ERROR: Size of string value exceeded.");
                current_code_object_->SetError(AMD_COMGR_STATUS_ERROR, "ERROR: Size of string value exceeded.");
            }

            status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_string_fn(handle_, &buf_size, buf);
            CheckStatus(current_code_object_, status, 0);

            uint32_t val = UINT32_MAX;
            if (!convert_string_to_uint32(buf, val))
            {
                assert(false && "ERROR: Failed to convert to uint32");
                current_code_object_->SetError(AMD_COMGR_STATUS_ERROR, "ERROR: Failed to convert to uint32");
            }
            return val;
        }
        else
        {
            return 0;
        }
    }

    template <>
    uint64_t MDNode::value<uint64_t>() const
    {
        CheckValid(0);

        if (GetKind() == Kind::kNodeKindString)
        {
            Check(ComgrEntryPoints::Instance()->EntryPointsValid(), 0);
            size_t             buf_size = 0;
            char               buf[256] = {};
            amd_comgr_status_t status   = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_string_fn(handle_, &buf_size, NULL);
            CheckStatus(current_code_object_, status, 0);

            if (buf_size >= 256)
            {
                assert(false && "ERROR: Size of string value exceeded.");
                current_code_object_->SetError(AMD_COMGR_STATUS_ERROR, "ERROR: Size of string value exceeded.");
            }

            status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_string_fn(handle_, &buf_size, buf);
            CheckStatus(current_code_object_, status, 0);

            uint64_t val = UINT64_MAX;
            if (!convert_string_to_uint64(buf, val))
            {
                assert(false && "ERROR: Failed to convert to uint64");
                current_code_object_->SetError(AMD_COMGR_STATUS_ERROR, "ERROR: Failed to convert to uint64");
            }
            return val;
        }
        else
        {
            return 0;
        }
    }

    /// @brief Wrapper class to enforce releasing an amd_comgr_data_t instance.
    class ComgrDataWrapper
    {
    public:
        /// @brief Constructor.
        ///
        /// @param [in] data The comgr data handle which should be released when this object is destroyed.
        ComgrDataWrapper(amd_comgr_data_t data)
            : data_(data)
        {
        }

        /// @brief Destructor.
        ///
        /// This destructor ensure the owned amd_comgr_data_t is released.
        ~ComgrDataWrapper()
        {
            ComgrEntryPoints::Instance()->amd_comgr_release_data_fn(data_);
        }

    private:
        ComgrDataWrapper()                                    = delete;
        ComgrDataWrapper(const ComgrDataWrapper&)             = delete;
        ComgrDataWrapper& operator=(const ComgrDataWrapper&)  = delete;
        ComgrDataWrapper(const ComgrDataWrapper&&)            = delete;
        ComgrDataWrapper& operator=(const ComgrDataWrapper&&) = delete;

        amd_comgr_data_t data_;  ///< The owned amd_comgr_data_t instance that should be released on destruction.
    };

    bool ComgrGetDemangledName(const std::string& mangled_name, std::string& demangled_name)
    {
        amd_comgr_data_t comgr_data;
        size_t           name_len = mangled_name.length();

        amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_create_data_fn(AMD_COMGR_DATA_KIND_BYTES, &comgr_data);
        CheckStatusAlt(status, false);
        ComgrDataWrapper comgr_data_wrapper(comgr_data);  // This makes sure the data is released.

        status = ComgrEntryPoints::Instance()->amd_comgr_set_data_fn(comgr_data, name_len, mangled_name.c_str());
        CheckStatusAlt(status, false);

        amd_comgr_data_t demangled_name_data;
        status = ComgrEntryPoints::Instance()->amd_comgr_demangle_symbol_name_fn(comgr_data, &demangled_name_data);
        CheckStatusAlt(status, false);
        ComgrDataWrapper demangled_name_data_wrapper(demangled_name_data);  // This makes sure the data is released.

        status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(demangled_name_data, &name_len, nullptr);
        CheckStatusAlt(status, false);

        char* demangled_name_str = new (std::nothrow) char[name_len + 1];
        Check((demangled_name_str != nullptr), false);
        status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(demangled_name_data, &name_len, demangled_name_str);

        if (status == AMD_COMGR_STATUS_SUCCESS)
        {
            demangled_name_str[name_len] = 0;
            demangled_name               = demangled_name_str;
        }

        delete[] demangled_name_str;
        return status == AMD_COMGR_STATUS_SUCCESS;
    }

}  // namespace amdt
