//============================================================================================
// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools
/// \file
/// \brief  This is a high level C++ interface of comgr utility functionality for tools.
//============================================================================================
#include "ComgrUtils.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

namespace AMDT
{
ComgrEntryPoints* ComgrEntryPoints::m_pInstance = nullptr;

/// Helper macro to avoid warnings about unused arguments for callbacks.
#define COMGRUTILS_UNUSED(x)  ((void)(x))

static void strncpy_safe(char* dest, const char* src, size_t copySize, size_t sourceBufferSize)
{
#ifdef _WIN32
    strncpy_s(dest, copySize, src, sourceBufferSize);
#else

    COMGRUTILS_UNUSED(sourceBufferSize);
    strncpy(dest, src, copySize);

#endif
}

// Metadata tags
const char*  gs_PAL_MD_TAG_PIPELINE_VERSION            = "amdpal.version";
const char*  gs_PAL_MD_TAG_PIPELINE_NAME               = ".name";
const char*  gs_PAL_MD_TAG_PIPELINE_TYPE               = ".type";
const char*  gs_PAL_MD_TAG_PIPELINE_HASH               = ".pipeline_compiler_hash";
const char*  gs_PAL_MD_TAG_PIPELINES                   = "amdpal.pipelines";
const char*  gs_PAL_MD_TAG_SHADERS                     = ".shaders";
const char*  gs_PAL_MD_TAG_SHADER_TYPE_VERTEX          = ".vertex";
const char*  gs_PAL_MD_TAG_SHADER_TYPE_HULL            = ".hull";
const char*  gs_PAL_MD_TAG_SHADER_TYPE_DOMAIN          = ".domain";
const char*  gs_PAL_MD_TAG_SHADER_TYPE_GEOMETRY        = ".geometry";
const char*  gs_PAL_MD_TAG_SHADER_TYPE_PIXEL           = ".pixel";
const char*  gs_PAL_MD_TAG_SHADER_TYPE_COMPUTE         = ".compute";
const char*  gs_PAL_MD_TAG_SHADER_HASH                 = ".api_shader_hash";
const char*  gs_PAL_MD_TAG_SHADER_HARDWARE_MAPPING     = ".hardware_mapping";
const char*  gs_PAL_MD_TAG_HARDWARE_STAGES             = ".hardware_stages";
const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_LS           = ".ls";
const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_HS           = ".hs";
const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_ES           = ".es";
const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_GS           = ".gs";
const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_VS           = ".vs";
const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_PS           = ".ps";
const char*  gs_PAL_MD_TAG_HARDWARE_STAGE_CS           = ".cs";
const char*  gs_PAL_MD_TAG_ENTRY_POINT_SYMBOL_NAME     = ".entry_point";
const char*  gs_PAL_MD_TAG_SCRATCH_MEMORY_SIZE         = ".scratch_memory_size";
const char*  gs_PAL_MD_TAG_LOCAL_DATA_SHARE_SIZE       = ".lds_size";
const char*  gs_PAL_MD_TAG_PERF_DATA_BUFFER_SIZE       = "PerformanceDataBufferSize";
const char*  gs_PAL_MD_TAG_NUM_USED_VGPRS              = ".vgpr_count";
const char*  gs_PAL_MD_TAG_NUM_USED_SGPRS              = ".sgpr_count";
const char*  gs_PAL_MD_TAG_NUM_AVAILABLE_VGPRS         = ".vgpr_limit";
const char*  gs_PAL_MD_TAG_NUM_AVAILABLE_SGPRS         = ".sgpr_limit";
const char*  gs_PAL_MD_TAG_WAVES_PER_GROUP             = ".waves_per_group";
const char*  gs_PAL_MD_TAG_USES_UAVS                   = ".uses_uavs";
const char*  gs_PAL_MD_TAG_USES_ROVS                   = ".uses_rovs";
const char*  gs_PAL_MD_TAG_WRITES_UAVS                 = ".writes_uavs";
const char*  gs_PAL_MD_TAG_WRITES_DEPTH                = ".writes_depth";
const char*  gs_PAL_MD_TAG_MAX_PRIMS_PER_PS_WAVE       = ".max_prims_per_ps_wave";
const char*  gs_PAL_MD_TAG_NUM_INTERPOLANTS            = ".num_interpolants";
const char*  gs_PAL_MD_TAG_REGISTERS                   = ".registers";
const char*  gs_PAL_MD_TAG_USER_DATA_LIMIT             = ".user_data_limit";
const char*  gs_PAL_MD_TAG_SPILL_SHRESHOLD             = ".spill_threshold";
const char*  gs_PAL_MD_TAG_USES_VIEWPORT_ARRAY_INDEX   = ".uses_viewport_array_index";
const char*  gs_PAL_MD_TAG_ES_GS_LOCAL_DATA_SHARE_SIZE = ".es_gs_lds_size";
const char*  gs_PAL_MD_TAG_WAVEFRONT_SIZE              = ".wavefront_size";
const char*  gs_PAL_MD_TAG_API                         = ".api";
const char*  gs_PAL_MD_TAG_API_CREATE_INFO             = ".api_create_info";


// Iteration state for symbols
struct CodeObjSymbolIterState
{
    void*           m_pScratchBuffer;           // Scratch buffer used in iteration
    size_t          m_scratchBuffersizeInBytes; // Scratch buffer size
    uint32_t        m_symbolCount;              // Num symbols
    uint32_t        m_currentPosition;          // Current position
    CodeObjSymbol*  m_pCodeObjectSymbols;       // Array of symbols that is filled
    CodeObjSymbolIterState():m_pScratchBuffer(nullptr), m_scratchBuffersizeInBytes(0), m_symbolCount(0), m_currentPosition(0), m_pCodeObjectSymbols(nullptr) {}
};

amd_comgr_status_t CodeObj::m_status;
std::string        CodeObj::m_errMsg;

static std::vector<std::string> gMapKeys; ///< Global keys map for MapIterCallback.


extern "C" amd_comgr_status_s
MapIterCallback(amd_comgr_metadata_node_t key, amd_comgr_metadata_node_t val, void* data)
{
    COMGRUTILS_UNUSED(val);
    COMGRUTILS_UNUSED(data);
    const std::string& keyVal = MDNode(key).value<std::string>();
    gMapKeys.push_back(keyVal);
    return (CodeObj::GetLastError().first == AMD_COMGR_STATUS_SUCCESS ?
            AMD_COMGR_STATUS_SUCCESS : AMD_COMGR_STATUS_ERROR);
};


amd_comgr_status_t countFuncSymbolCallback(amd_comgr_symbol_t symbol, void* pUserData)
{
    amd_comgr_status_t status = AMD_COMGR_STATUS_ERROR;
    if (pUserData != nullptr)
    {
        CodeObjSymbolIterState* pState = (CodeObjSymbolIterState*)pUserData;
        void* buffer = pState->m_pScratchBuffer;
        status = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_TYPE, buffer);

        if (*((amd_comgr_symbol_type_t*)buffer) == AMD_COMGR_SYMBOL_TYPE_FUNC)
        {
            // if its a function print some more info
            pState->m_symbolCount += 1;
        }
    }

    return status;
}

amd_comgr_status_t appendToSymbolVectorCallback(amd_comgr_symbol_t symbol, void* pUserData)
{
    amd_comgr_status_t status = AMD_COMGR_STATUS_ERROR;

    CodeObjSymbolIterState* pState = (CodeObjSymbolIterState*)pUserData;
    if (pState == nullptr)
    {
        return status;
    }

    // Abbreviated name to use in this func
    void* buffer = pState->m_pScratchBuffer;
    size_t buffersize = pState->m_scratchBuffersizeInBytes;
    memset(buffer, '\0', buffersize);

    status = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_TYPE, buffer);

    if (*((amd_comgr_symbol_type_t*)buffer) == AMD_COMGR_SYMBOL_TYPE_FUNC)
    {
        CodeObjSymbol* functionSymbol = &(pState->m_pCodeObjectSymbols[pState->m_currentPosition]);

        functionSymbol->m_type = COMGR_UTILS_SYMBOL_TYPE_FUNC;

        memset(buffer, '\0', buffersize);
        status = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_NAME_LENGTH, buffer);
        functionSymbol->m_symbolFunction.m_nameLen = *(size_t*)(buffer);

        functionSymbol->m_symbolFunction.m_pName = (char*)malloc(functionSymbol->m_symbolFunction.m_nameLen + 1);
        if (functionSymbol->m_symbolFunction.m_pName != nullptr)
        {
            memset(functionSymbol->m_symbolFunction.m_pName, '\0', functionSymbol->m_symbolFunction.m_nameLen + 1);
            status = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_NAME, functionSymbol->m_symbolFunction.m_pName);
        }

        memset(buffer, 0, buffersize);
        status = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_SIZE, buffer);
        functionSymbol->m_symbolFunction.m_symbolSize =  *(uint64_t*)buffer;

        memset(buffer, 0, buffersize);
        status = ComgrEntryPoints::Instance()->amd_comgr_symbol_get_info_fn(symbol, AMD_COMGR_SYMBOL_INFO_VALUE, buffer);
        functionSymbol->m_symbolFunction.m_symbolValue = *(uint64_t*)buffer;

        // next position
        pState->m_currentPosition += 1;
    }

    return status;
}

std::unique_ptr<CodeObj>
CodeObj::OpenFile(const std::string& fileName)
{
    std::ifstream file(fileName, std::ios::in | std::ios::binary);
    std::vector<char> buf;
    file.seekg(0, std::ios::end);
    int size = static_cast<int>(file.tellg());
    file.seekg(0, std::ios::beg);
    buf.resize(size);
    file.read(&buf[0], size);
    return OpenBuffer(buf);
}

std::unique_ptr<CodeObj>
CodeObj::OpenFile(const std::string& fileName, const amd_comgr_data_kind_t& dataKind)
{
    std::ifstream file(fileName, std::ios::in | std::ios::binary);
    std::vector<char> buf;
    file.seekg(0, std::ios::end);
    int size = static_cast<int>(file.tellg());
    file.seekg(0, std::ios::beg);
    buf.resize(size);
    file.read(&buf[0], size);
    return OpenBuffer(buf, dataKind);
}

std::unique_ptr<CodeObj>
CodeObj::OpenBufferRaw(const char* pBuf, const size_t sizeInBytes)
{
    std::unique_ptr<CodeObj> retHandle = nullptr;

    if (pBuf != nullptr && sizeInBytes > 0)
    {
        std::vector<char> buffer;
        buffer.assign(pBuf, pBuf + sizeInBytes);
        retHandle = OpenBuffer(buffer);
    }

    return retHandle;
}

std::unique_ptr<CodeObj>
CodeObj::OpenBuffer(const std::vector<char>& buf)
{
    amd_comgr_data_t coData;
    amd_comgr_data_set_t coDataSet;
    amd_comgr_status_t status = AMD_COMGR_STATUS_ERROR;

    status = ComgrEntryPoints::Instance()->amd_comgr_create_data_fn(AMD_COMGR_DATA_KIND_RELOCATABLE, &coData);
    CheckStatus(status, nullptr);
    status = ComgrEntryPoints::Instance()->amd_comgr_set_data_fn(coData, buf.size(), &buf[0]);
    CheckStatus(status, nullptr);
    status = ComgrEntryPoints::Instance()->amd_comgr_set_data_name_fn(coData, "data");
    CheckStatus(status, nullptr);
    status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&coDataSet);
    CheckStatus(status, nullptr);
    status = ComgrEntryPoints::Instance()->amd_comgr_data_set_add_fn(coDataSet, coData);
    CheckStatus(status, nullptr);

    std::unique_ptr<CodeObj> pCodeObj(new (std::nothrow) CodeObj(buf, coData, coDataSet));

    return pCodeObj;
}

std::unique_ptr<CodeObj>
CodeObj::OpenBuffer(const std::vector<char>& buf, const amd_comgr_data_kind_t& dataKind)
{
    amd_comgr_data_t coData;
    amd_comgr_data_set_t coDataSet;
    amd_comgr_status_t status = AMD_COMGR_STATUS_ERROR;

    status = ComgrEntryPoints::Instance()->amd_comgr_create_data_fn(dataKind, &coData);
    CheckStatus(status, nullptr);
    status = ComgrEntryPoints::Instance()->amd_comgr_set_data_fn(coData, buf.size(), &buf[0]);
    CheckStatus(status, nullptr);
    status = ComgrEntryPoints::Instance()->amd_comgr_set_data_name_fn(coData, "data");
    CheckStatus(status, nullptr);
    status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&coDataSet);
    CheckStatus(status, nullptr);
    status = ComgrEntryPoints::Instance()->amd_comgr_data_set_add_fn(coDataSet, coData);
    CheckStatus(status, nullptr);

    std::unique_ptr<CodeObj> pCodeObj(new (std::nothrow) CodeObj(buf, coData, coDataSet));

    return pCodeObj;
}

MDNode CodeObj::GetMD()
{
    amd_comgr_metadata_node_t md;
    amd_comgr_status_t status;
    status = ComgrEntryPoints::Instance()->amd_comgr_get_data_metadata_fn(m_data, &md);
    CheckStatus(status, 0);
    // the root must be map
    amd_comgr_metadata_kind_t kind = AMD_COMGR_METADATA_KIND_NULL;
    status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_kind_fn(md, &kind);
    CheckStatus(status, 0);
    return (kind == AMD_COMGR_METADATA_KIND_MAP ? MDNode(md) : 0);
}


bool CodeObj::ExtractSymbolData(CodeObjSymbolInfo& data)
{

    // scratch buffer to hold symbol info within callback over symbols
    CodeObjSymbolIterState* iterState = (CodeObjSymbolIterState*)malloc(sizeof(CodeObjSymbolIterState));
    if (iterState == nullptr)
    {
        return false;
    }
    memset(iterState, 0, sizeof(CodeObjSymbolIterState));
    static const size_t s_SCRATCH_BUFER_SIZE = 1024;
    iterState->m_scratchBuffersizeInBytes = s_SCRATCH_BUFER_SIZE;
    iterState->m_pScratchBuffer = (char*)malloc(iterState->m_scratchBuffersizeInBytes);
    if (iterState->m_pScratchBuffer == nullptr)
    {
        return false;
    }

    bool retCode = false;
    amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_iterate_symbols_fn(
        m_data,
        countFuncSymbolCallback,
        iterState);
    CheckStatus(status, retCode);

    memset(iterState->m_pScratchBuffer, 0, iterState->m_scratchBuffersizeInBytes);

    if (iterState->m_symbolCount > 0)
    {
        iterState->m_pCodeObjectSymbols = (CodeObjSymbol*)malloc(sizeof(CodeObjSymbol)*iterState->m_symbolCount);
        if (iterState->m_pCodeObjectSymbols != nullptr)
        {
            status = ComgrEntryPoints::Instance()->amd_comgr_iterate_symbols_fn(
                m_data,
                appendToSymbolVectorCallback,
                iterState);
            CheckStatus(status, retCode);

            if (status == AMD_COMGR_STATUS_SUCCESS)
            {
                data.m_numSymbols = iterState->m_symbolCount;
                data.m_pSymbols = iterState->m_pCodeObjectSymbols;
                retCode = true;
            }
        }
    }

    // Dont free iterState->m_pSymbols.
    // The pointer to the symbols, it will be cleared when you call Clear symbol data
    free(iterState->m_pScratchBuffer);
    free(iterState);

    return retCode;
}

void CodeObj::ClearSymbolData(CodeObjSymbolInfo& data)
{
    if ((data.m_numSymbols > 0) && (data.m_pSymbols != nullptr))
    {
        for (uint32_t i = 0; i < data.m_numSymbols; ++i)
        {
            if (data.m_pSymbols[i].m_type == COMGR_UTILS_SYMBOL_TYPE_FUNC)
            {
                if (data.m_pSymbols[i].m_symbolFunction.m_pName != nullptr)
                {
                    free(data.m_pSymbols[i].m_symbolFunction.m_pName);
                }
            }
        }
        free(data.m_pSymbols);
        data.m_numSymbols = 0;
    }
}

bool CodeObj::ExtractPalPipelineData(PalPipelineData& data)
{
    MDNode md = GetMD();

    // Extract version.
    GetPalMDMapItemRequired(gs_PAL_MD_TAG_PIPELINE_VERSION, md, version);
    size_t versionEntries = version.size();

    if (versionEntries < 2)
    {
        return false;
    }

    data.m_version.m_major = version[0].value<uint32_t>();
    data.m_version.m_minor = version[1].value<uint32_t>();

    // Extract Pipelines.
    GetPalMDMapItemRequired(gs_PAL_MD_TAG_PIPELINES, md, pipelines);
    size_t pipelinesNum = pipelines.size();
    data.m_numPipelines = static_cast<uint32_t>(pipelinesNum);

    data.m_pPipelines = (Pipeline*) malloc(pipelinesNum * sizeof(Pipeline));
    if (nullptr == data.m_pPipelines)
    {
        return false;
    }

    memset(data.m_pPipelines, 0, pipelinesNum * sizeof(Pipeline));
    bool retCode = true;

    for (size_t i = 0; i < pipelinesNum; i++)
    {
        Pipeline* pPipelineData = &data.m_pPipelines[i];
        MDNode ppln = pipelines[i];

        // Name.
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_PIPELINE_NAME, ppln))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_PIPELINE_NAME, ppln, pplnName);
            const std::string& name = pplnName.value<std::string>();
            pPipelineData->m_pName = (char*)malloc(name.size() + 1);
            if (nullptr == pPipelineData->m_pName)
            {
                retCode = false;
                break;
            }

            memset(pPipelineData->m_pName, '\0', name.size() + 1);
            strncpy_safe(pPipelineData->m_pName, name.c_str(), name.size() + 1, name.length() + 1);
        }


        // Type. (Not supported yet)
        //GetPalMDMapItemRequired(gs_PAL_MD_TAG_PIPELINE_TYPE, ppln, pplnType);
        //pPipelineData->m_type = pplnType.int32Val();
        // Hash.
        GetPalMDMapItemRequired(gs_PAL_MD_TAG_PIPELINE_HASH, ppln, pplnHash);
        pPipelineData->m_hash = pplnHash.value<uint64_t>();
        // User Data Limit.
        GetPalMDMapItemRequired(gs_PAL_MD_TAG_USER_DATA_LIMIT, ppln, userDataLimit);
        pPipelineData->m_userDataLimit = userDataLimit.value<uint32_t>();
        // Spill threshold.
        GetPalMDMapItemRequired(gs_PAL_MD_TAG_SPILL_SHRESHOLD, ppln, spillThreashold);
        pPipelineData->m_spillThreshold = spillThreashold.value<uint32_t>();

        // UsesViewportArrayIndex
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_USES_VIEWPORT_ARRAY_INDEX, ppln))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_USES_VIEWPORT_ARRAY_INDEX, ppln, viewportArrIndex);
            pPipelineData->m_usesViewportArrayIndex = viewportArrIndex.value<uint32_t>();
        }

        // EsGsLocalDataShareSize
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_ES_GS_LOCAL_DATA_SHARE_SIZE, ppln))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_ES_GS_LOCAL_DATA_SHARE_SIZE, ppln, esgsLocDataShareSize);
            pPipelineData->m_esGsLocalDataShareSize = esgsLocDataShareSize.value<uint32_t>();
        }

        // Scratch Memory Size
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_SCRATCH_MEMORY_SIZE, ppln))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_SCRATCH_MEMORY_SIZE, ppln, scratchSize);
            pPipelineData->m_scratchMemorySize = scratchSize.value<uint32_t>();
        }

        // Wavefront Size
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_WAVEFRONT_SIZE, ppln))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_WAVEFRONT_SIZE, ppln, wavefrontSize);
            pPipelineData->m_wavefrontSize = wavefrontSize.value<uint32_t>();
        }

        // API
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_API, ppln))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_API, ppln, api);
            pPipelineData->m_api = api.value<uint32_t>();
        }

        // ApiCreateInfo
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_API_CREATE_INFO, ppln))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_API_CREATE_INFO, ppln, apiCreate);
            pPipelineData->m_apiCreateInfo = apiCreate.value<uint32_t>();
        }

        // Extract shaders.
        GetPalMDMapItemRequired(gs_PAL_MD_TAG_SHADERS, ppln, shaders);
        size_t shadersNum = shaders.size();
        pPipelineData->m_numShaders = static_cast<uint32_t>(shadersNum);
        pPipelineData->m_pShaderList = (ShaderInfo*) malloc(shadersNum * sizeof(ShaderInfo));

        if (nullptr == pPipelineData->m_pShaderList)
        {
            retCode = false;
            break;
        }

        memset(pPipelineData->m_pShaderList, 0, shadersNum * sizeof(ShaderInfo));
        // Extract Shaders Info.
        if (!ExtractPalMDShadersInfo(*pPipelineData, ppln))
        {
            retCode = false;
        }

        // Extract hardware stages.
        if (!ExtractPalMDHardwareStages(*pPipelineData, ppln))
        {
            retCode = false;
        }

        // Extract register info.
        if (!ExtractPalMDRegisterInfo(*pPipelineData, ppln))
        {
            retCode = false;
        }


    } // end pipleline loop

    return retCode;
}

// Get diassembly size
bool CodeObj::ExtractAssemblySizeInBytes(const char* options, uint32_t* pOutSizeInByes)
{
    bool retCode = false;
    if (pOutSizeInByes != nullptr)
    {
        std::vector<char> assemblyBuffer;
        std::string ipOptions(options);

        ExtractAssemblyData(assemblyBuffer, ipOptions);

        pOutSizeInByes[0] = uint32_t(assemblyBuffer.size());
        retCode = true;
    }
    return retCode;
}

// Get diassembly in a char*
bool CodeObj::ExtractAssemblyRaw(const char* options, const uint32_t sizeInBytes, char* pOutData)
{
    bool retCode = false;
    if (pOutData != nullptr)
    {
        std::vector<char> assemblyBuffer;
        std::string ipOptions(options);

        ExtractAssemblyData(assemblyBuffer, options);
        if (sizeInBytes == assemblyBuffer.size())
        {
            memcpy(pOutData, assemblyBuffer.data(), assemblyBuffer.size());
            retCode = true;
        }
    }
    return retCode;
}


bool CodeObj::ExtractAssemblyData(std::vector<char>& assemblyBuffer, std::string options)
{
    amd_comgr_status_t status;

    amd_comgr_action_info_t actionInfo;
    status = ComgrEntryPoints::Instance()->amd_comgr_create_action_info_fn(&actionInfo);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_isa_name_fn(actionInfo, options.c_str());
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_options_fn(actionInfo, "");
    CheckStatus(status, false);

    amd_comgr_data_set_t dataSetOut;
    status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&dataSetOut);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(AMD_COMGR_ACTION_DISASSEMBLE_RELOCATABLE_TO_SOURCE,
                                                                  actionInfo,
                                                                  m_dataSet,
                                                                  dataSetOut);
    CheckStatus(status, false);

    size_t count;
    status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(dataSetOut, AMD_COMGR_DATA_KIND_SOURCE, &count);
    CheckStatus(status, false);

    if (1 != count)
    {
        std::cerr << "ERROR: Number of data object should be 1." << std::endl;
        return false;
    }

    amd_comgr_data_t dataOut;
    status = ComgrEntryPoints::Instance()->amd_comgr_action_data_get_data_fn(dataSetOut, AMD_COMGR_DATA_KIND_SOURCE, 0, &dataOut);
    CheckStatus(status, false);

    // Update size only, then we can update output buffer later
    status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(dataOut, &count, nullptr);
    CheckStatus(status, false);

    // Update output buffer
    assemblyBuffer.resize(count);
    status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(dataOut, &count, assemblyBuffer.data());
    CheckStatus(status, false);

    return true;
}

bool CodeObj::ConvertSourceToCodeObject(std::vector<char>& codeObjectBuffer, const amd_comgr_language_t& languageInfo, const std::string& isaName)
{
    amd_comgr_status_t status;

    amd_comgr_action_info_t actionInfo;
    status = ComgrEntryPoints::Instance()->amd_comgr_create_action_info_fn(&actionInfo);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_language_fn(actionInfo, languageInfo);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_isa_name_fn(actionInfo, isaName.c_str());
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_options_fn(actionInfo, "-mno-code-object-v3");
    CheckStatus(status, false);

    amd_comgr_data_set_t dataSetPreCompiledHeaders;
    status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&dataSetPreCompiledHeaders);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(AMD_COMGR_ACTION_ADD_PRECOMPILED_HEADERS,
                                                                  actionInfo,
                                                                  m_dataSet,
                                                                  dataSetPreCompiledHeaders);
    CheckStatus(status, false);

    size_t count;
    status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(dataSetPreCompiledHeaders, AMD_COMGR_DATA_KIND_PRECOMPILED_HEADER, &count);
    CheckStatus(status, false);

    if (1 != count)
    {
        std::cerr << "ERROR: Incorrect number of data object (expected 1)." << std::endl;
        return false;
    }

    amd_comgr_data_set_t dataSetBitCode;
    status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&dataSetBitCode);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(AMD_COMGR_ACTION_COMPILE_SOURCE_TO_BC,
                                                                  actionInfo,
                                                                  dataSetPreCompiledHeaders,
                                                                  dataSetBitCode);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(dataSetBitCode, AMD_COMGR_DATA_KIND_BC, &count);
    CheckStatus(status, false);

    if (1 != count)
    {
        std::cerr << "ERROR: Incorrect number of data object (expected 1)." << std::endl;
        return false;
    }

    amd_comgr_data_set_t dataSetDevLibs;
    status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&dataSetDevLibs);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_options_fn(actionInfo, "");
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(AMD_COMGR_ACTION_ADD_DEVICE_LIBRARIES,
                                                                  actionInfo,
                                                                  dataSetBitCode,
                                                                  dataSetDevLibs);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(dataSetDevLibs, AMD_COMGR_DATA_KIND_BC, &count);
    CheckStatus(status, false);

    amd_comgr_data_set_t dataSetLinked;
    status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&dataSetLinked);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_options_fn(actionInfo, "-mno-code-object-v3");
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(AMD_COMGR_ACTION_LINK_BC_TO_BC,
                                                                  actionInfo,
                                                                  dataSetDevLibs,
                                                                  dataSetLinked);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(dataSetLinked, AMD_COMGR_DATA_KIND_BC, &count);
    CheckStatus(status, false);

    if (1 != count)
    {
        std::cerr << "ERROR: Incorrect number of data object (expected 1)." << std::endl;
        return false;
    }

    amd_comgr_data_set_t dataSetRelocatable;
    status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&dataSetRelocatable);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(AMD_COMGR_ACTION_CODEGEN_BC_TO_RELOCATABLE,
                                                                  actionInfo,
                                                                  dataSetLinked,
                                                                  dataSetRelocatable);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(dataSetRelocatable, AMD_COMGR_DATA_KIND_RELOCATABLE, &count);
    CheckStatus(status, false);

    if (1 != count)
    {
        std::cerr << "ERROR: Incorrect number of data object (expected 1)." << std::endl;
        return false;
    }

    amd_comgr_data_set_t dataSetExecutable;
    status = ComgrEntryPoints::Instance()->amd_comgr_create_data_set_fn(&dataSetExecutable);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_info_set_options_fn(actionInfo, "");
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_do_action_fn(AMD_COMGR_ACTION_LINK_RELOCATABLE_TO_EXECUTABLE,
                                                                  actionInfo,
                                                                  dataSetRelocatable,
                                                                  dataSetExecutable);
    CheckStatus(status, false);

    status = ComgrEntryPoints::Instance()->amd_comgr_action_data_count_fn(dataSetExecutable, AMD_COMGR_DATA_KIND_EXECUTABLE, &count);
    CheckStatus(status, false);

    if (1 != count)
    {
        std::cerr << "ERROR: Incorrect number of data object (expected 1)." << std::endl;
        return false;
    }

    amd_comgr_data_t dataOut;
    status = ComgrEntryPoints::Instance()->amd_comgr_action_data_get_data_fn(dataSetExecutable, AMD_COMGR_DATA_KIND_EXECUTABLE, 0, &dataOut);
    CheckStatus(status, false);

    // Update size only, then we can update output buffer later
    status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(dataOut, &count, nullptr);
    CheckStatus(status, false);

    // Update output buffer
    codeObjectBuffer.resize(count);
    status = ComgrEntryPoints::Instance()->amd_comgr_get_data_fn(dataOut, &count, codeObjectBuffer.data());
    CheckStatus(status, false);

    return true;
}

void CodeObj::ClearPalPipelineData(PalPipelineData& data)
{
    for (size_t pplnN = 0; pplnN < data.m_numPipelines; pplnN++)
    {
        Pipeline* ppln = &data.m_pPipelines[pplnN];
        free(ppln->m_pName);
        free(ppln->m_pShaderList);

        for (size_t stageN = 0; stageN < ppln->m_numStages; stageN++)
        {
            HWStageInfo* pStage = &ppln->m_pStageList[stageN];
            free(pStage->m_pEntryPointSymbolName);
        }

        free(ppln->m_pRegisterDataList);
    }

    memset(&data, 0, sizeof(PalPipelineData));
}

std::pair<amd_comgr_status_t, std::string> CodeObj::GetLastError()
{
    const char* pErrMsg = nullptr;
    std::string msg = (ComgrEntryPoints::Instance()->amd_comgr_status_string_fn(m_status, &pErrMsg) == AMD_COMGR_STATUS_SUCCESS ? pErrMsg : "");
    amd_comgr_status_t status = m_status;
    m_status = AMD_COMGR_STATUS_SUCCESS;
    m_errMsg = "";
    return {status, msg};
}

bool CodeObj::ExtractPalMDShadersInfo(Pipeline& mdPipelineData, MDNode& ppln)
{
    GetPalMDMapItemRequired(gs_PAL_MD_TAG_SHADERS, ppln, shaders);
    size_t shadersNum = shaders.size();
    mdPipelineData.m_numShaders = static_cast<uint32_t>(shadersNum);

    mdPipelineData.m_pShaderList = (ShaderInfo*)malloc(shadersNum * sizeof(ShaderInfo));
    if (nullptr == mdPipelineData.m_pShaderList)
    {
        return false;
    }

    memset(mdPipelineData.m_pShaderList, 0, shadersNum * sizeof(ShaderInfo));

    int shaderN = 0;

    for (const std::string& shaderType : shaders.GetKeys())
    {
        ShaderInfo* pShaderInfoData = &mdPipelineData.m_pShaderList[shaderN++];

        // Shader Info Type.
        if (shaderType == gs_PAL_MD_TAG_SHADER_TYPE_VERTEX)
        {
            pShaderInfoData->m_shaderType = ShaderInfoType::VERTEX_SHADER;
        }
        else if (shaderType == gs_PAL_MD_TAG_SHADER_TYPE_HULL)
        {
            pShaderInfoData->m_shaderType = ShaderInfoType::HULL_SHADER;
        }
        else if (shaderType == gs_PAL_MD_TAG_SHADER_TYPE_DOMAIN)
        {
            pShaderInfoData->m_shaderType = ShaderInfoType::DOMAIN_SHADER;
        }
        else if (shaderType == gs_PAL_MD_TAG_SHADER_TYPE_GEOMETRY)
        {
            pShaderInfoData->m_shaderType = ShaderInfoType::GEOMETRY_SHADER;
        }
        else if (shaderType == gs_PAL_MD_TAG_SHADER_TYPE_PIXEL)
        {
            pShaderInfoData->m_shaderType = ShaderInfoType::PIXEL_SHADER;
        }
        else if (shaderType == gs_PAL_MD_TAG_SHADER_TYPE_COMPUTE)
        {
            pShaderInfoData->m_shaderType = ShaderInfoType::COMPUTE_SHADER;
        }
        else
        {
            assert(false && "ERROR: Unknown Shader Info Type.");
        }

        MDNode shaderInfo = shaders[shaderType];
        Check(shaderInfo.IsValid() && shaderInfo.GetKind() == MDNode::Kind::Map, false);
        // Shader Info Hash. (Not supported yet)
        //GetPalMDMapItemRequired(gs_PAL_MD_TAG_SHADER_HASH, shaderInfo, shaderHash);
        //pShaderInfoData->m_hash = ...
        // Hardware Mapping.
        GetPalMDMapItemRequired(gs_PAL_MD_TAG_SHADER_HARDWARE_MAPPING, shaderInfo, shaderHwMapping);
        pShaderInfoData->m_hardwareMapping = shaderHwMapping.value<uint32_t>();
    }

    return true;
}

bool CodeObj::ExtractPalMDHardwareStages(Pipeline& mdPipelineData, MDNode& ppln)
{
    GetPalMDMapItemRequired(gs_PAL_MD_TAG_HARDWARE_STAGES, ppln, stages);
    size_t stagesNum = stages.size();
    mdPipelineData.m_numStages = static_cast<uint32_t>(stagesNum);
    mdPipelineData.m_pStageList = (HWStageInfo*) malloc(stagesNum * sizeof(HWStageInfo));

    if (mdPipelineData.m_pStageList == nullptr)
    {
        return false;
    }

    memset(mdPipelineData.m_pStageList, 0, stagesNum * sizeof(HWStageInfo));

    int stageN = 0;

    for (const std::string& stageType : stages.GetKeys())
    {
        HWStageInfo* stageInfoData = &mdPipelineData.m_pStageList[stageN++];

        // Stage Type.
        if (stageType == gs_PAL_MD_TAG_HARDWARE_STAGE_LS)
        {
            stageInfoData->m_stageType = HwStageType::LS;
        }
        else if (stageType == gs_PAL_MD_TAG_HARDWARE_STAGE_HS)
        {
            stageInfoData->m_stageType = HwStageType::HS;
        }
        else if (stageType == gs_PAL_MD_TAG_HARDWARE_STAGE_ES)
        {
            stageInfoData->m_stageType = HwStageType::ES;
        }
        else if (stageType == gs_PAL_MD_TAG_HARDWARE_STAGE_GS)
        {
            stageInfoData->m_stageType = HwStageType::GS;
        }
        else if (stageType == gs_PAL_MD_TAG_HARDWARE_STAGE_PS)
        {
            stageInfoData->m_stageType = HwStageType::PS;
        }
        else if (stageType == gs_PAL_MD_TAG_HARDWARE_STAGE_VS)
        {
            stageInfoData->m_stageType = HwStageType::VS;
        }
        else if (stageType == gs_PAL_MD_TAG_HARDWARE_STAGE_CS)
        {
            stageInfoData->m_stageType = HwStageType::CS;
        }
        else
        {
            assert(false && "ERROR: Unknown HW Stage Type");
        }

        MDNode stageInfo = stages[stageType];
        Check(stageInfo.IsValid() && stageInfo.GetKind() == MDNode::Kind::Map, false);

        // Entry Symbol Name.
        GetPalMDMapItemRequired(gs_PAL_MD_TAG_ENTRY_POINT_SYMBOL_NAME, stageInfo, entryName);
        const std::string& name = entryName.value<std::string>();

        stageInfoData->m_pEntryPointSymbolName = (char*)malloc(name.size() + 1);
        memset(stageInfoData->m_pEntryPointSymbolName, '\0', name.size() + 1);

        strncpy_safe(stageInfoData->m_pEntryPointSymbolName, name.c_str(), name.size() + 1, name.length() + 1);

        // Scratch Memory Size.
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_SCRATCH_MEMORY_SIZE, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_SCRATCH_MEMORY_SIZE, stageInfo, scratchMemSize);
            stageInfoData->m_scratchMemorySize = scratchMemSize.value<uint32_t>();
        }

        // Local Data Share Size.
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_LOCAL_DATA_SHARE_SIZE, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_LOCAL_DATA_SHARE_SIZE, stageInfo, localshareSize);
            stageInfoData->m_localDataShareSize = localshareSize.value<uint32_t>();
        }

        // Perf Data Buffer Size.
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_PERF_DATA_BUFFER_SIZE, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_PERF_DATA_BUFFER_SIZE, stageInfo, perfBufSize);
            stageInfoData->m_performanceDataBufferSize = perfBufSize.value<uint32_t>();
        }

        // Used VGPRs/SGPRs
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_NUM_USED_VGPRS, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_NUM_USED_VGPRS, stageInfo, usedVGPRs);
            stageInfoData->m_numUsedVgprs = usedVGPRs.value<uint32_t>();
        }

        if (CheckPalMDMapItem(gs_PAL_MD_TAG_NUM_USED_SGPRS, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_NUM_USED_SGPRS, stageInfo, usedSGPRs);
            stageInfoData->m_numUsedSgprs = usedSGPRs.value<uint32_t>();
        }

        // If the gs_PAL_MD_TAG_NUM_AVAILABLE_VGPRS or gs_PAL_MD_TAG_NUM_AVAILABLE_SGPRS
        // tags are not there, then we should be using the device limits.
        //
        // The metadata tags only added if the limits were explicitly overwritten.

        // Available VGPRs/SGPRs
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_NUM_AVAILABLE_VGPRS, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_NUM_AVAILABLE_VGPRS, stageInfo, availableVGPRs);
            stageInfoData->m_numAvailableVgprs = availableVGPRs.value<uint32_t>();
        }

        if (CheckPalMDMapItem(gs_PAL_MD_TAG_NUM_AVAILABLE_SGPRS, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_NUM_AVAILABLE_SGPRS, stageInfo, availableSGPRs);
            stageInfoData->m_numAvailableSgprs = availableSGPRs.value<uint32_t>();
        }

        // Waves Per Group
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_WAVES_PER_GROUP, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_WAVES_PER_GROUP, stageInfo, wavesPerGroup);
            stageInfoData->m_wavesPerGroup = wavesPerGroup.value<uint32_t>();
        }

        // Uses Uavs.
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_USES_UAVS, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_USES_UAVS, stageInfo, usesUavs);
            stageInfoData->m_usesUavs = usesUavs.value<uint32_t>();
        }

        // Uses Rovs.
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_USES_ROVS, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_USES_ROVS, stageInfo, usesRovs);
            stageInfoData->m_usesRovs = usesRovs.value<uint32_t>();
        }

        // Writes Uavs.
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_WRITES_UAVS, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_WRITES_UAVS, stageInfo, writesUavs);
            stageInfoData->m_writesUavs = writesUavs.value<uint32_t>();
        }

        // Writes Depth.
        if (CheckPalMDMapItem(gs_PAL_MD_TAG_WRITES_DEPTH, stageInfo))
        {
            GetPalMDMapItemRequired(gs_PAL_MD_TAG_WRITES_DEPTH, stageInfo, writesDepth);
            stageInfoData->m_writesDepth = writesDepth.value<uint32_t>();
        }

        // MaxPrimsPerPsWave (optional).
        stageInfoData->m_maxPrimsPerPsWave = (stageInfo.Find(gs_PAL_MD_TAG_MAX_PRIMS_PER_PS_WAVE) ?
                                              stageInfo[gs_PAL_MD_TAG_MAX_PRIMS_PER_PS_WAVE].value<uint32_t>() : 0);

        // NumInterpolants (optional).
        stageInfoData->m_numInterpolants = (stageInfo.Find(gs_PAL_MD_TAG_NUM_INTERPOLANTS) ?
                                            stageInfo[gs_PAL_MD_TAG_NUM_INTERPOLANTS].value<uint32_t>() : 0);

    }

    return true;
}

bool CodeObj::ExtractPalMDRegisterInfo(Pipeline& mdPipelineData, MDNode& ppln)
{
    bool retCode = true;
    // Registers.
    GetPalMDMapItemRequired(gs_PAL_MD_TAG_REGISTERS, ppln, regs);
    mdPipelineData.m_numRegisterWrites = static_cast<uint32_t>(regs.size());
    if (regs.size() > 0)
    {
        mdPipelineData.m_pRegisterDataList = (RegisterData*)malloc(regs.size() * sizeof(RegisterData));
        if (nullptr != mdPipelineData.m_pRegisterDataList)
        {

            memset(mdPipelineData.m_pRegisterDataList, 0, regs.size() * sizeof(RegisterData));

            for (const std::string& addrKey : regs.GetKeys())
            {
                RegisterData* regData = &mdPipelineData.m_pRegisterDataList[0];
                GetPalMDMapItemRequired(addrKey, regs, temp);
                std::stringstream stream(addrKey);
                stream >> regData->m_address;
                assert(!stream.fail());

                if (stream.fail())
                {
                    retCode = false;
                }

                regData->m_data = temp.value<uint32_t>();
            }
        }
    } //if regs.size() > 0

    return retCode;
}

void CodeObj::SetError(amd_comgr_status_t err, const std::string& errMsg)
{
    m_status = err;

    if (!errMsg.empty())
    {
        m_errMsg = errMsg;
    }
}

MDNode::MDNode(amd_comgr_metadata_node_t node) : m_handle(node)
{
}

MDNode::MDNode(int handle)
{
    m_handle.handle = handle;
}

MDNode::Kind MDNode::GetKind() const
{
    CheckValid(Kind::None);
    amd_comgr_metadata_kind_t kind = AMD_COMGR_METADATA_KIND_NULL;
    amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_kind_fn(m_handle, &kind);
    CheckStatus(status, Kind::None);

    // TODO: Current comgr implementation returns integer values as strings.
    // That will change soon, so this code must be updated too.
    switch (kind)
    {
        case AMD_COMGR_METADATA_KIND_STRING:
            return Kind::String;

        case AMD_COMGR_METADATA_KIND_LIST:
            return Kind::List;

        case AMD_COMGR_METADATA_KIND_MAP:
            return Kind::Map;

        case AMD_COMGR_METADATA_KIND_NULL:

        default:
            return Kind::None;
    }
}

MDNode MDNode::operator[](int idx) const
{
    amd_comgr_metadata_node_t child = {0};

    if (IsValid())
    {
        if (GetKind() == Kind::List)
        {
            amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_index_list_metadata_fn(m_handle, idx, &child);
            CheckStatus(status, child);
        }
    }

    return MDNode(child);
}

MDNode MDNode::operator[](size_t idx) const
{
    amd_comgr_metadata_node_t child = { 0 };

    if (IsValid())
    {
        if (GetKind() == Kind::List)
        {
            amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_index_list_metadata_fn(m_handle, idx, &child);
            CheckStatus(status, child);
        }
    }

    return MDNode(child);
}
MDNode MDNode::operator[](const std::string& key) const
{
    amd_comgr_metadata_node_t child = {0};

    if (IsValid())
    {
        if (GetKind() == Kind::Map)
        {
            amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_metadata_lookup_fn(m_handle, key.c_str(), &child);
            CheckStatus(status, child);
        }
    }

    return MDNode(child);
}

MDNode MDNode::operator[](const char* key) const
{
    amd_comgr_metadata_node_t child = {0};

    if (IsValid())
    {
        if (GetKind() == Kind::Map)
        {
            amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_metadata_lookup_fn(m_handle, key, &child);
            CheckStatus(status, child);
        }
    }

    return MDNode(child);
}

bool MDNode::Find(const std::string& key) const
{
    CheckValid(false);

    if (GetKind() == Kind::Map)
    {
        amd_comgr_metadata_node_t child;
        amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_metadata_lookup_fn(m_handle, key.c_str(), &child);
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
    // The default specialization for arithmetic types.
    CheckValid(0);
    TYPE val = (TYPE)0;

    // TODO: Current comgr implementation returns integer values as strings.
    // That will change soon, so this code must be updated too.
    if (GetKind() == Kind::String)
    {
        std::stringstream stream(value<std::string>());
        stream >> val;

        if (stream.fail())
        {
            CodeObj::SetError(AMD_COMGR_STATUS_ERROR, "ERROR: Failed to convert to int32");
        }
    }

    return val;
}

size_t MDNode::size() const
{
    CheckValid(0);
    amd_comgr_status_t status = AMD_COMGR_STATUS_ERROR;
    size_t size;

    switch (GetKind())
    {
        case Kind::List:
            status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_list_size_fn(m_handle, &size);
            break;

        case Kind::Map:
            status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_map_size_fn(m_handle, &size);
            break;

        default:
            size = 0;
    }

    CheckStatus(status, 0);
    return size;
}

std::vector<std::string> MDNode::GetKeys() const
{
    CheckValid({});
    amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_iterate_map_metadata_fn(m_handle, MapIterCallback, nullptr);
    CheckStatus(status, {});
    std::vector<std::string> keys = gMapKeys;
    gMapKeys.clear();
    return keys;
}

bool MDNode::IsValid() const
{
    return (m_handle.handle != 0);
}

void Dump()
{
    assert(false && "ERROR: Not implemented");
}

template<>
std::string MDNode::value<std::string>() const
{
    CheckValid("");

    if (GetKind() == Kind::String)
    {
        size_t bufSize;
        char   buf[256];
        amd_comgr_status_t status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_string_fn(m_handle, &bufSize, NULL);
        CheckStatus(status, "");

        if (bufSize >= 256)
        {
            CodeObj::SetError(AMD_COMGR_STATUS_ERROR, "ERROR: Size of string value exceeded.");
        }

        status = ComgrEntryPoints::Instance()->amd_comgr_get_metadata_string_fn(m_handle, &bufSize, buf);
        CheckStatus(status, "");
        return buf;
    }
    else
    {
        return "";
    }
}
}
