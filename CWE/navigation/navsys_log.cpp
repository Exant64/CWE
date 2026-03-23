#include "stdafx.h"

#include <stdio.h>
#include <cstdarg>
#include <mutex>

static std::mutex logMutex;
static FILE* logOutput;

void NavSysLogInit(const char* path) {
    static char filePath[256];
    sprintf_s(filePath, "%s\\cwe_navmesh.log", path);

    logOutput = _fsopen(filePath, "a", _SH_DENYWR);

    if(!logOutput) {
        PrintDebug("failed to create navsys log at %s", filePath);
        return;
    }

    time_t now = time(NULL);           // current time
    struct tm *t = localtime(&now);   // convert to local time

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", t);
    fprintf_s(logOutput, "New log session: %s\n", buffer);
}

void NavSysLogExit() {
    if(logOutput) {
        fclose(logOutput);
    }
}

void NavSysLog(const char* format, ...) {
    if(!logOutput) {
        return;
    }

    std::lock_guard<std::mutex> lock(logMutex);

    static char msg[1024];

    va_list argptr;
    va_start(argptr, format);
    vsnprintf_s(msg, _countof(msg), format, argptr);
    va_end(argptr);

    PrintDebug("%s\n", msg);

    fprintf_s(logOutput, "%s\n", msg);
    fflush(logOutput);
}