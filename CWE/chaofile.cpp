#include "chaofile.h"

CHAO_SAVE_INFO LoadChaoFile(const char* path) {
    CHAO_SAVE_INFO chaoInfo;
    FILE* chaoFile;

    fopen_s(&chaoFile, path, "rb");
    fseek(chaoFile, 64, SEEK_SET);
    fread(&chaoInfo, 1, sizeof(chaoInfo), chaoFile);
    fclose(chaoFile);

    return chaoInfo;
}