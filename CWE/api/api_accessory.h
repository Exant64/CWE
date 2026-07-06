#pragma once

#include "cwe_api.h"
#include <optional>

#include <al_marketattr.h>

extern CWE_API_REGISTER_ACCESSORY AL_ModAPI_Accessory;

#define ACCESSORY_ID_LENGTH sizeof(CWE_API_ACCESSORY_DATA::ID)

void AccessorySetupDraw(const size_t index, const Uint32 colors[8], const Uint32 usedColorFlags);

bool IsAccessoryGeneric(int index);
CWE_API_ACCESSORY_DATA& GetAccessoryData(int index);
extern "C" __declspec(dllexport) EAccessoryType GetAccessoryType(int index);

const uint32_t* GetAccessoryDefaultColors(size_t index);
const size_t GetAccessoryColorCount(size_t index);
const std::optional <CWE_API_ACCESSORY_BALD_DATA>& GetAccessoryBaldData(size_t index);
extern "C" __declspec(dllexport) void AccessoryMakeBald(int accessory_id);
extern "C" __declspec(dllexport) void AccessoryDisableJiggle(int accessory_id);
const size_t GetAccessoryCount();
const bool IsAccessoryRFSupported(const int id);

bool AccessoryCheckID(const char* ID);
size_t GetAccessoryID(const char* pID);
size_t AddChaoAccessory(const CWE_API_ACCESSORY_DATA* pAccessoryData);
extern "C" __declspec(dllexport) int RegisterChaoAccessory(EAccessoryType type, NJS_OBJECT* model, NJS_TEXLIST* texlist, BlackMarketItemAttributes* attrib, const char* name, const char* description);

void CWE_ParamSetAccessory(task* tp, uint32_t slot, uint32_t kind);
size_t CWE_ParamGetAccessory(task* tp, uint32_t slot);

// helper RAII to store all accessories in range to belong to a mod index
// bit hacky, but for now accessories are the only API that warrant this
// system, so I will not make anything more generalized for it until 
// necessary. for a more generalized solution, black market item entries
// could be used as a sane count check
class AccessoryModIndexSetter {
private:
    const size_t m_modIndex;
    size_t m_start;
public:
    AccessoryModIndexSetter(size_t modIndex) : m_modIndex(modIndex) {
        m_start = GetAccessoryCount();
    }

    ~AccessoryModIndexSetter() {
        const auto count = GetAccessoryCount() - m_start;

        for(size_t i = m_start; i < m_start + count; i++) {
            BlackMarketAttributes::Get()->SetModIndex(ChaoItemCategory_Accessory, i, m_modIndex);
	    }
    }
};