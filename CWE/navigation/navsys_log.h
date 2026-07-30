#pragma once

void NavSysLogInit(const char* path);
void NavSysLogExit();
void NavSysLog(const char* format, ...);