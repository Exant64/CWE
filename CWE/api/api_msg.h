#pragma once

size_t RegisterAlItemString(const char* str);
void OverwriteAlItemString(size_t id, const char* str);
size_t RegisterFoName(char* str);

void AL_ModAPI_Msg_Init();