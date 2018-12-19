//============================================================================================
// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools
/// \file
/// \brief  This is a high level C++ interface of comgr utility functionality for tools.
//============================================================================================
#ifndef COMGR_UTILS_H_
#define COMGR_UTILS_H_

#include <memory>
#include <string>
#include <vector>

#ifdef COMGR_DYNAMIC_LINKING
    #ifdef _WIN32
        #include <windows.h>
        #include <tchar.h>
    #else
        #include <dlfcn.h>
    #endif
#endif

#include "amd_comgr.h"

namespace AMDT
{
class MDNode;
class CodeObj;

// Singleton struct to hold the entry points of the comgr library
struct ComgrEntryPoints
{
    decltype(amd_comgr_status_string)*                          amd_comgr_status_string_fn;                          ///< comgr library entry point
    decltype(amd_comgr_get_version)*                            amd_comgr_get_version_fn;                            ///< comgr library entry point
    decltype(amd_comgr_get_isa_count)*                          amd_comgr_get_isa_count_fn;                          ///< comgr library entry point
    decltype(amd_comgr_get_isa_name)*                           amd_comgr_get_isa_name_fn;                           ///< comgr library entry point
    decltype(amd_comgr_get_isa_metadata)*                       amd_comgr_get_isa_metadata_fn;                       ///< comgr library entry point
    decltype(amd_comgr_create_data)*                            amd_comgr_create_data_fn;                            ///< comgr library entry point
    decltype(amd_comgr_release_data)*                           amd_comgr_release_data_fn;                           ///< comgr library entry point
    decltype(amd_comgr_get_data_kind)*                          amd_comgr_get_data_kind_fn;                          ///< comgr library entry point
    decltype(amd_comgr_set_data)*                               amd_comgr_set_data_fn;                               ///< comgr library entry point
    decltype(amd_comgr_set_data_name)*                          amd_comgr_set_data_name_fn;                          ///< comgr library entry point
    decltype(amd_comgr_get_data)*                               amd_comgr_get_data_fn;                               ///< comgr library entry point
    decltype(amd_comgr_get_data_name)*                          amd_comgr_get_data_name_fn;                          ///< comgr library entry point
    decltype(amd_comgr_get_data_isa_name)*                      amd_comgr_get_data_isa_name_fn;                      ///< comgr library entry point
    decltype(amd_comgr_get_data_metadata)*                      amd_comgr_get_data_metadata_fn;                      ///< comgr library entry point
    decltype(amd_comgr_destroy_metadata)*                       amd_comgr_destroy_metadata_fn;                       ///< comgr library entry point
    decltype(amd_comgr_create_data_set)*                        amd_comgr_create_data_set_fn;                        ///< comgr library entry point
    decltype(amd_comgr_destroy_data_set)*                       amd_comgr_destroy_data_set_fn;                       ///< comgr library entry point
    decltype(amd_comgr_data_set_add)*                           amd_comgr_data_set_add_fn;                           ///< comgr library entry point
    decltype(amd_comgr_data_set_remove)*                        amd_comgr_data_set_remove_fn;                        ///< comgr library entry point
    decltype(amd_comgr_action_data_count)*                      amd_comgr_action_data_count_fn;                      ///< comgr library entry point
    decltype(amd_comgr_action_data_get_data)*                   amd_comgr_action_data_get_data_fn;                   ///< comgr library entry point
    decltype(amd_comgr_create_action_info)*                     amd_comgr_create_action_info_fn;                     ///< comgr library entry point
    decltype(amd_comgr_destroy_action_info)*                    amd_comgr_destroy_action_info_fn;                    ///< comgr library entry point
    decltype(amd_comgr_action_info_set_isa_name)*               amd_comgr_action_info_set_isa_name_fn;               ///< comgr library entry point
    decltype(amd_comgr_action_info_get_isa_name)*               amd_comgr_action_info_get_isa_name_fn;               ///< comgr library entry point
    decltype(amd_comgr_action_info_set_language)*               amd_comgr_action_info_set_language_fn;               ///< comgr library entry point
    decltype(amd_comgr_action_info_get_language)*               amd_comgr_action_info_get_language_fn;               ///< comgr library entry point
    decltype(amd_comgr_action_info_set_options)*                amd_comgr_action_info_set_options_fn;                ///< comgr library entry point
    decltype(amd_comgr_action_info_get_options)*                amd_comgr_action_info_get_options_fn;                ///< comgr library entry point
    decltype(amd_comgr_action_info_set_working_directory_path)* amd_comgr_action_info_set_working_directory_path_fn; ///< comgr library entry point
    decltype(amd_comgr_action_info_get_working_directory_path)* amd_comgr_action_info_get_working_directory_path_fn; ///< comgr library entry point
    decltype(amd_comgr_action_info_set_logging)*                amd_comgr_action_info_set_logging_fn;                ///< comgr library entry point
    decltype(amd_comgr_action_info_get_logging)*                amd_comgr_action_info_get_logging_fn;                ///< comgr library entry point
    decltype(amd_comgr_do_action)*                              amd_comgr_do_action_fn;                              ///< comgr library entry point
    decltype(amd_comgr_get_metadata_kind)*                      amd_comgr_get_metadata_kind_fn;                      ///< comgr library entry point
    decltype(amd_comgr_get_metadata_string)*                    amd_comgr_get_metadata_string_fn;                    ///< comgr library entry point
    decltype(amd_comgr_get_metadata_map_size)*                  amd_comgr_get_metadata_map_size_fn;                  ///< comgr library entry point
    decltype(amd_comgr_iterate_map_metadata)*                   amd_comgr_iterate_map_metadata_fn;                   ///< comgr library entry point
    decltype(amd_comgr_metadata_lookup)*                        amd_comgr_metadata_lookup_fn;                        ///< comgr library entry point
    decltype(amd_comgr_get_metadata_list_size)*                 amd_comgr_get_metadata_list_size_fn;                 ///< comgr library entry point
    decltype(amd_comgr_index_list_metadata)*                    amd_comgr_index_list_metadata_fn;                    ///< comgr library entry point
    decltype(amd_comgr_iterate_symbols)*                        amd_comgr_iterate_symbols_fn;                        ///< comgr library entry point
    decltype(amd_comgr_symbol_lookup)*                          amd_comgr_symbol_lookup_fn;                          ///< comgr library entry point
    decltype(amd_comgr_symbol_get_info)*                        amd_comgr_symbol_get_info_fn;                        ///< comgr library entry point

    /// Gets the static singleton instance
    /// \return the singleton instance
    static ComgrEntryPoints* Instance()
    {
        if (nullptr == m_pInstance)
        {
            m_pInstance = new ComgrEntryPoints;
        }

        return m_pInstance;
    }

    /// Deletes the static singleton instance
    static void DeleteInstance()
    {
        if (nullptr != m_pInstance)
        {
            ComgrEntryPoints* pCopyOfInstance = m_pInstance;
            m_pInstance = nullptr;
            delete pCopyOfInstance;
        }
    }

    /// Indicates if the comgr library entry points are valid
    /// Always true when COMGR_DYNAMIC_LINKING is not defined
    /// \return true if the comgr entry points are valid
    bool EntryPointsValid()
    {
        return m_entryPointsValid;
    }

private:

    bool m_entryPointsValid = true; ///< flag indicating if the comgr library entry points are valid

#ifdef COMGR_DYNAMIC_LINKING

#ifdef _WIN32
    HMODULE m_module; ///< the comgr library module handle
#else
    void* m_module;   ///< the comgr library module handle
#endif

    /// Attempts to initialize the specified comgr library entry point
    /// \param pEntryPointName the name of the entry point to initialize
    /// \return the address of the entry point or nullptr if the entry point could not be initialized
    void* InitEntryPoint(const char* pEntryPointName)
    {
        if (nullptr != m_module)
        {
#ifdef _WIN32
            return GetProcAddress(m_module, pEntryPointName);
#else
            return dlsym(m_module, pEntryPointName);
#endif
        }

        return nullptr;
    }
#endif

    /// Private constructor
    ComgrEntryPoints()
    {
#ifdef COMGR_DYNAMIC_LINKING
    #ifdef _WIN32
        m_module = LoadLibrary(_T("amd_comgr.dll"));
    #else
        m_module = dlopen("libamd_comgr.so", RTLD_LAZY);
    #endif
        #define INIT_COMGR_ENTRY_POINT(func) reinterpret_cast<decltype(func)*>(InitEntryPoint(#func)); m_entryPointsValid &= nullptr != func##_fn
#else
        #define INIT_COMGR_ENTRY_POINT(func) func
#endif
        amd_comgr_status_string_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_status_string);
        amd_comgr_get_version_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_version);
        amd_comgr_get_isa_count_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_isa_count);
        amd_comgr_get_isa_name_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_isa_name);
        amd_comgr_get_isa_metadata_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_isa_metadata);
        amd_comgr_create_data_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_create_data);
        amd_comgr_release_data_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_release_data);
        amd_comgr_get_data_kind_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_data_kind);
        amd_comgr_set_data_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_set_data);
        amd_comgr_set_data_name_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_set_data_name);
        amd_comgr_get_data_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_data);
        amd_comgr_get_data_name_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_data_name);
        amd_comgr_get_data_isa_name_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_data_isa_name);
        amd_comgr_get_data_metadata_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_data_metadata);
        amd_comgr_destroy_metadata_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_destroy_metadata);
        amd_comgr_create_data_set_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_create_data_set);
        amd_comgr_destroy_data_set_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_destroy_data_set);
        amd_comgr_data_set_add_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_data_set_add);
        amd_comgr_data_set_remove_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_data_set_remove);
        amd_comgr_action_data_count_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_data_count);
        amd_comgr_action_data_get_data_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_data_get_data);
        amd_comgr_create_action_info_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_create_action_info);
        amd_comgr_destroy_action_info_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_destroy_action_info);
        amd_comgr_action_info_set_isa_name_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_set_isa_name);
        amd_comgr_action_info_get_isa_name_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_get_isa_name);
        amd_comgr_action_info_set_language_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_set_language);
        amd_comgr_action_info_get_language_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_get_language);
        amd_comgr_action_info_set_options_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_set_options);
        amd_comgr_action_info_get_options_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_get_options);
        amd_comgr_action_info_set_working_directory_path_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_set_working_directory_path);
        amd_comgr_action_info_get_working_directory_path_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_get_working_directory_path);
        amd_comgr_action_info_set_logging_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_set_logging);
        amd_comgr_action_info_get_logging_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_action_info_get_logging);
        amd_comgr_do_action_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_do_action);
        amd_comgr_get_metadata_kind_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_metadata_kind);
        amd_comgr_get_metadata_string_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_metadata_string);
        amd_comgr_get_metadata_map_size_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_metadata_map_size);
        amd_comgr_iterate_map_metadata_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_iterate_map_metadata);
        amd_comgr_metadata_lookup_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_metadata_lookup);
        amd_comgr_get_metadata_list_size_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_get_metadata_list_size);
        amd_comgr_index_list_metadata_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_index_list_metadata);
        amd_comgr_iterate_symbols_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_iterate_symbols);
        amd_comgr_symbol_lookup_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_symbol_lookup);
        amd_comgr_symbol_get_info_fn = INIT_COMGR_ENTRY_POINT(amd_comgr_symbol_get_info);
        #undef INIT_COMGR_ENTRY_POINT
    }

    /// Destructor
    virtual ~ComgrEntryPoints()
    {
#ifdef COMGR_DYNAMIC_LINKING
        if (nullptr != m_module)
        {
#ifdef _WIN32
            FreeLibrary(m_module);
#else
            dlclose(m_module);
#endif
        }
#endif
        DeleteInstance();
    }

private:
    static ComgrEntryPoints* m_pInstance; ///< static singleton instance
};

/// PAL pipeline version struct
struct PalPipelineVersion
{
    uint32_t m_major; ///< version major
    uint32_t m_minor; ///< version minor
    /// Default constructor
    PalPipelineVersion() :m_major(0), m_minor(0) {}
};

/// API level shader types
enum ShaderInfoType
{
    VERTEX_SHADER = 0,   ///< vertex shader
    HULL_SHADER,         ///< hull shader
    DOMAIN_SHADER,       ///< Ddomain shader
    GEOMETRY_SHADER,     ///< geometry shader
    PIXEL_SHADER,        ///< pixel shader
    COMPUTE_SHADER       ///< compute shader
};

/// Shader info struct
struct ShaderInfo
{
    ShaderInfoType m_shaderType;       ///< shader type
    uint8_t        m_hash[16];         ///< 128 bit hash
    uint32_t       m_hardwareMapping;  ///< HW Mapping
};

/// Hardware stage type
enum HwStageType
{
    ES = 0,    ///< ES
    GS,        ///< GS
    VS,        ///< VS
    HS,        ///< HS
    LS,        ///< LS
    SS,        ///< NGG
    PrimS,     ///< NGG
    PS,        ///< PS
    CS         ///< CS
};


/// Hardware stage info
struct HWStageInfo
{
    HwStageType     m_stageType;                  ///< stage type
    uint32_t        m_scratchMemorySize;          ///< scratch mem size
    uint32_t        m_localDataShareSize;         ///< local data share size
    uint32_t        m_performanceDataBufferSize;  ///< performance data buffer size
    uint32_t        m_numUsedVgprs;               ///< number of used VGPRs
    uint32_t        m_numUsedSgprs;               ///< number of used SGPRs
    uint32_t        m_numAvailableVgprs;          ///< number of available VGPRs
    uint32_t        m_numAvailableSgprs;          ///< number of available SGPRs
    uint32_t        m_wavesPerGroup;              ///< waves per group
    uint32_t        m_usesUavs;                   ///< uses UAVs
    uint32_t        m_usesRovs;                   ///< uses ROVs
    uint32_t        m_writesUavs;                 ///< writes UAVs
    uint32_t        m_writesDepth;                ///< writes Depth
    uint32_t        m_maxPrimsPerPsWave;          ///< 0 for PS (added so we dont need a special type for PS)
    uint32_t        m_numInterpolants;            ///< 0 for PS (added so we dont need a special type for PS)
    char*           m_pEntryPointSymbolName;      ///< entry point symbol name
};

/// RegisterData struct
struct RegisterData
{
    uint32_t m_address;   ///< data address
    uint32_t m_data;      ///< register data
};

/// Pipeline struct
struct Pipeline
{
    char*       m_pName;                      ///< null terminated string
    uint32_t    m_type;                       ///< Type of pipeline
    uint64_t    m_hash;                       ///< 64bit hash

    uint32_t    m_numShaders;                 ///< num entries in array below
    uint32_t    m_numStages;                  ///< num entries in array below
    uint32_t    m_numRegisterWrites;          ///< num entries in array below

    uint32_t    m_userDataLimit;              ///< limit of user data
    uint32_t    m_spillThreshold;             ///< threshold of spill
    uint32_t    m_usesViewportArrayIndex;     ///< view port arrary index
    uint32_t    m_esGsLocalDataShareSize;     ///< ES and GS local data share size
    uint32_t    m_scratchMemorySize;          ///< scratch memory size
    uint32_t    m_wavefrontSize;              ///< wavefront size
    uint32_t    m_api;                        ///< api info
    uint32_t    m_apiCreateInfo;              ///< api create info

    ShaderInfo*     m_pShaderList;             ///< shader list
    HWStageInfo*    m_pStageList;              ///< stage list
    RegisterData*   m_pRegisterDataList;       ///< register data list
};

/// PAL pipeline data
struct PalPipelineData
{
    PalPipelineVersion  m_version;         ///< PAL version info
    uint32_t            m_numPipelines;    ///< Number of pipelines
    Pipeline*           m_pPipelines;      ///< the pipelines itself
    /// Default constructor
    PalPipelineData():m_version(), m_numPipelines(0), m_pPipelines(nullptr){}
};

enum CodeObjSymbolType
{
    COMGR_UTILS_SYMBOL_TYPE_NOTYPE = 0, ///< The symbol's type is not specified.
    COMGR_UTILS_SYMBOL_TYPE_OBJECT,     ///< The symbol is associated with a data object, such as a variable, an array,  and so on.
    COMGR_UTILS_SYMBOL_TYPE_FUNC,       ///< The symbol is associated with a function or other executable code.
    COMGR_UTILS_SYMBOL_TYPE_SECTION,    ///< The symbol is associated with a section. Symbol table entries of this type exist primarily for relocation.
    COMGR_UTILS_SYMBOL_TYPE_FILE,       ///< Conventionally, the symbol's name gives the name of the source file associated with the object file.
    COMGR_UTILS_SYMBOL_TYPE_COMMON      ///< The symbol labels an uninitialized common block.
};

/// Symbol of type function
struct CodeObjSymbolFunction
{
    uint64_t    m_symbolSize;   ///< Size of the symbol
    uint64_t    m_nameLen;      ///< String length for the name
    uint64_t    m_symbolValue;  ///< Symbol value
    char*       m_pName;        ///< Name of the symbol, null terminated
    /// Default constructor
    CodeObjSymbolFunction(): m_symbolSize(), m_nameLen(0), m_symbolValue(0), m_pName(nullptr) {}
};

/// Symbol of type section
struct CodeObjSymbolSection
{
    uint64_t    m_data;
    CodeObjSymbolSection(): m_data(0) {}
};

/// Symbols that comgrUtils provides
struct CodeObjSymbol
{
    CodeObjSymbolType  m_type;
    union
    {
        CodeObjSymbolFunction m_symbolFunction;
        CodeObjSymbolSection  m_symbolSection;
    };

    CodeObjSymbol():
        m_type(COMGR_UTILS_SYMBOL_TYPE_NOTYPE),
        m_symbolFunction(){};
};

/// Code object symbols
struct CodeObjSymbolInfo
{
    uint32_t        m_numSymbols;    ///< Number of symbols
    CodeObjSymbol*  m_pSymbols;      ///< the symbols

    // Default constructor
    CodeObjSymbolInfo(): m_numSymbols(0), m_pSymbols(nullptr){}
};

extern const char*  gs_PAL_MD_TAG_PIPELINE_VERSION;
extern const char*  gs_PAL_MD_TAG_PIPELINE_VERSION_MAJOR;
extern const char*  gs_PAL_MD_TAG_PIPELINE_VERSION_MINOR;
extern const char*  gs_PAL_MD_TAG_PIPELINE_NAME;
extern const char*  gs_PAL_MD_TAG_PIPELINE_TYPE;
extern const char*  gs_PAL_MD_TAG_PIPELINE_HASH;
extern const char*  gs_PAL_MD_TAG_PIPELINES;
extern const char*  gs_PAL_MD_TAG_SHADERS;
extern const char*  gs_PAL_MD_TAG_SHADER_TYPE_VERTEX;
extern const char*  gs_PAL_MD_TAG_SHADER_TYPE_GEOMETRY;
extern const char*  gs_PAL_MD_TAG_SHADER_TYPE_PIXEL;
extern const char*  gs_PAL_MD_TAG_SHADER_TYPE_COMPUTE;
extern const char*  gs_PAL_MD_TAG_SHADER_HASH;
extern const char*  gs_PAL_MD_TAG_SHADER_HARDWARE_MAPPING;
extern const char*  gs_PAL_MD_TAG_HARDWARE_STAGES;
extern const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_LS;
extern const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_HS;
extern const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_ES;
extern const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_GS;
extern const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_VS;
extern const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_PS;
extern const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_CS;
extern const char*  gs_PAL_MD_TAG_ENTRY_POINT_SYMBOL_NAME;
extern const char*  gs_PAL_MD_TAG_SCRATCH_MEMORY_SIZE;
extern const char*  gs_PAL_MD_TAG_LOCAL_DATA_SHARE_SIZE;
extern const char*  gs_PAL_MD_TAG_PERF_DATA_BUFFER_SIZE;
extern const char*  gs_PAL_MD_TAG_NUM_USED_VGPRS;
extern const char*  gs_PAL_MD_TAG_NUM_USED_SGPRS;
extern const char*  gs_PAL_MD_TAG_NUM_AVAILABLE_VGPRS;
extern const char*  gs_PAL_MD_TAG_NUM_AVAILABLE_SGPRS;
extern const char*  gs_PAL_MD_TAG_WAVES_PER_GROUP;
extern const char*  gs_PAL_MD_TAG_USES_UAVS;
extern const char*  gs_PAL_MD_TAG_USES_ROVS;
extern const char*  gs_PAL_MD_TAG_WRITES_UAVS;
extern const char*  gs_PAL_MD_TAG_WRITES_DEPTH;
extern const char*  gs_PAL_MD_TAG_MAX_PRIMS_PER_PS_WAVE;
extern const char*  gs_PAL_MD_TAG_NUM_INTERPOLANTS;
extern const char*  gs_PAL_MD_TAG_REGISTERS;
extern const char*  gs_PAL_MD_TAG_USER_DATA_LIMIT;
extern const char*  gs_PAL_MD_TAG_SPILL_SHRESHOLD;
extern const char*  gs_PAL_MD_TAG_USES_VIEWPORT_ARRAY_INDEX;
extern const char*  gs_PAL_MD_TAG_ES_GS_LOCAL_DATA_SHARE_SIZE;
extern const char*  gs_PAL_MD_TAG_WAVEFRONT_SIZE;
extern const char*  gs_PAL_MD_TAG_API;
extern const char*  gs_PAL_MD_TAG_API_CREATE_INFO;

#define CheckStatus(status, retVal) \
        if (status != AMD_COMGR_STATUS_SUCCESS) \
        { \
            CodeObj::SetError(status); \
            return retVal; \
        }

#define CheckValid(retVal) \
        if (!IsValid()) \
        { \
            return retVal; \
        }

#define Check(boolVal, retVal) \
        if (!boolVal) \
        { \
            return retVal; \
        }

#define GetPalMDMapItemRequired(TAG, MAP, ITEM) \
    MDNode ITEM = MAP[TAG]; \
    if (!ITEM.IsValid()) \
    { \
        CodeObj::SetError(AMD_COMGR_STATUS_ERROR, "ERROR: Failed to get required MD value:" #ITEM); \
        return false; \
    }\

#define CheckPalMDMapItem(TAG, MAP) \
    (MAP[TAG].IsValid())

/// Callback function for amd_comgr_iterate_map_metadata.
/// \param key amd_comgr_metadata_node_t type key.
/// \param val amd_comgr_metadata_node_t type value.
/// \param data callback data pointer.
/// \return AMD COMGR status.
extern "C" amd_comgr_status_s MapIterCallback(amd_comgr_metadata_node_t key, amd_comgr_metadata_node_t val, void* data);

/// Code Object class.
class CodeObj
{
    friend class MDNode;
public:
    /// Open Code Object from a file.
    /// \param fileName the file name.
    /// \return the unique_ptr pointing to the Codeobj object.
    static std::unique_ptr<CodeObj> OpenFile(const std::string& fileName);

    /// Open Code Object from a file.
    /// \param fileName the file name.
    /// \param dataKind the data kind.
    /// \return the unique_ptr pointing to the Codeobj object.
    static std::unique_ptr<CodeObj> OpenFile(const std::string& fileName, const amd_comgr_data_kind_t& dataKind);

    /// Open Code Object from a memory buffer.
    /// \param buf the memory buffer.
    /// \return the unique_ptr pointing to the Codeobj object.
    static std::unique_ptr<CodeObj> OpenBuffer(const std::vector<char>& buf);

    /// Open Code Object from a memory buffer.
    /// \param buf the memory buffer.
    /// \param dataKind the data kind.
    /// \return the unique_ptr pointing to the Codeobj object.
    static std::unique_ptr<CodeObj> OpenBuffer(const std::vector<char>& buf, const amd_comgr_data_kind_t& dataKind);

    /// Open Code Object from a char buffer.
    /// \param pBuf the memory buffer.
    /// \return the unique_ptr pointing to the Codeobj object.
    static std::unique_ptr<CodeObj> OpenBufferRaw(const char* pBuf, size_t sizeInBytes);

    /// Extract Metadata (MD).
    /// \return the metadata node.
    MDNode GetMD();

    /// Extract the PAL Pipeline metadata and fill the provided structure.
    /// \param data the PalPipelineData type data.
    /// \return true if successful, false otherwise.
    bool ExtractPalPipelineData(PalPipelineData& data);

    /// Extract the symbol info and fill the provided structure.
    /// \param data the Symbol structure
    /// \return true if successful, false otherwise.
    bool ExtractSymbolData(CodeObjSymbolInfo& data);

    /// Extract the assembly data to a data buffer.
    /// \param assemblyBuffer the memory buffer of assembly data.
    /// \param options the options for extracting assembly buffer.
    /// \return true if successful, false otherwise.
    bool ExtractAssemblyData(std::vector<char>& assemblyBuffer, std::string options);

    /// Extract the assembly size in bytes to a data buffer.
    /// \param options the options for extracting assembly buffer.
    /// \param outSizeInByes pointer to a uint to get size in bytes
    /// \return true if successful, false otherwise.
    bool ExtractAssemblySizeInBytes(const char* options, uint32_t* outSizeInByes);

    /// Extract the assembly size in bytes to a data buffer.
    /// \param options the options for extracting assembly buffer.
    /// \param sizeInbyes the size in bytes of the input buffer, to validate memory is enough
    /// \param outData The buffer that will be filled with ISA
    /// \return true if successful, false otherwise.
    bool ExtractAssemblyRaw(const char* options, const uint32_t sizeInbyes, char* outData);

    /// Convert the source data to a code object.
    /// \param codeObjectBuffer the memory buffer of code object.
    /// \param languageInfo the language info for source data.
    /// \param isaName the ISA name string.
    /// \return true if successful, false otherwise.
    bool ConvertSourceToCodeObject(std::vector<char>& codeObjectBuffer, const amd_comgr_language_t& languageInfo, const std::string& isaName);

    /// Clear the PAL pipeline data.
    /// \param data the PalPipelineData type data.
    static void ClearPalPipelineData(PalPipelineData& data);

    /// Clear the symbol data
    /// \param data the symbol data
    static void ClearSymbolData(CodeObjSymbolInfo& data);

    /// Get the error caused by last unsuccessful operation.
    /// Restores the error value to "success".
    /// \return the pair of AMD COMGR status and error message string.
    static std::pair<amd_comgr_status_t, std::string> GetLastError();

    /// Set error.
    /// \param err the error status.
    /// \param errMsg the error message string.
    /// \return true if successful, false otherwise.
    static void SetError(amd_comgr_status_t err, const std::string& errMsg = "");

    /// Constructor.
    /// \param buf the memory buffer.
    /// \param coData the amd_comgr_data_t type data.
    /// \param coDataSet the amd_comgr_data_set_t data set.
    CodeObj(const std::vector<char>& buf, amd_comgr_data_t coData, amd_comgr_data_set_t coDataSet) : m_buf(buf), m_data(coData), m_dataSet(coDataSet) {}

    // Destructor.
    ~CodeObj()
    {
        ComgrEntryPoints::Instance()->amd_comgr_release_data_fn(m_data);
    }

private:
    /// Helper function for extracting PAL metadata Shaders Info.
    /// \param mdPipelineData the pipeline data.
    /// \param ppln the metadata node.
    /// \return true if successful, false otherwise.
    static bool ExtractPalMDShadersInfo(Pipeline& mdPipelineData, MDNode& ppln);

    /// Helper function for extracting PAL metadata Hardware Stages.
    /// \param mdPipelineData the pipeline data.
    /// \param ppln the metadata node.
    /// \return true if successful, false otherwise.
    static bool ExtractPalMDHardwareStages(Pipeline& mdPipelineData, MDNode& ppln);

    /// Helper function for extracting PAL metadata for register info
    /// \param mdPipelineData the pipeline data.
    /// \param ppln the metadata node.
    /// \return true if successful, false otherwise.
    static bool ExtractPalMDRegisterInfo(Pipeline& mdPipelineData, MDNode& ppln);

    std::vector<char>                   m_buf;      ///< Data buffer.
    amd_comgr_data_t                    m_data;     ///< The amd_comgr_data_t type data.
    amd_comgr_data_set_t                m_dataSet;  ///< The amd_comgr_data_set_t type data set.
    static amd_comgr_status_t           m_status;   ///< The AMD COMGR status.
    static std::string                  m_errMsg;   ///< The error message string.
};

/// Metadata Node.
class MDNode
{
public:

    /// Metadata node kind.
    enum class Kind
    {
        None = 0,       ///< No info for metadata node.
        Int,            ///< Metadata node kind is int.
        String,         ///< Metadata node kind is string.
        List,           ///< Metadata node kind is list.
        Map             ///< Metadata node kind is map.
    };

    /// Constructor.
    /// \param node amd_comgr_metadata_node_t type metadata node.
    MDNode(amd_comgr_metadata_node_t node);

    /// Constructor.
    /// \param handle the node handle.
    MDNode(int handle);

    /// Destructor.
    ~MDNode() = default;

    /// Get the kind of this MD node.
    /// \return metadata node kind.
    Kind GetKind() const;

    /// Get the MD sub-node by its index (only valid for List MD nodes).
    /// \param idx the metadata node index.
    /// \return the metadata node.
    MDNode operator[] (int idx) const;

    /// Get the MD sub-node by its index (only valid for List MD nodes).
    /// \param idx the metadata node index.
    /// \return the metadata node.
    MDNode operator[] (size_t idx) const;

    /// Get the MD sub-node by its string key (only valid for Map MD nodes).
    /// \param key the key string of metadata sub-node.
    /// \return the metadata node.
    MDNode operator[] (const std::string& key) const;

    /// Get the MD sub-node by its char* key (only valid for Map MD nodes).
    /// \param key the key of metadata sub-node.
    /// \return the metadata node.
    MDNode operator[] (const char* key) const;

    /// Checks if sub-node with provided key exists in this node (only valid for Map MD nodes).
    /// \param key the key of metadata sub-node.
    /// \return true if successful, false otherwise.
    bool Find(const std::string& key) const;

    /// Get the value.
    /// \return the value.
    template<typename TYPE>
    TYPE value() const;

    /// Get the number of sub-nodes of this MD node (only valid for List and Map nodes).
    /// \return the size.
    size_t size() const;

    /// Get the list of string keys (only valid for Map MD nodes).
    /// \return the keys vector.
    std::vector<std::string> GetKeys() const;

    /// Indicates whether the handle is valid.
    /// \return true if successful, false otherwise.
    bool IsValid() const;

    /// Dump the error info.
    void Dump();

private:
    amd_comgr_metadata_node_t m_handle;    ///< The metadata node handle.
};

/// Specialization of "value" function for std::string.
/// \return the value string.
template<>
std::string MDNode::value<std::string>() const;
}

#endif
