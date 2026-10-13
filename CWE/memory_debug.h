#pragma once

#include "stdafx.h"
#include <memory.h>

#include <set>

// hacky lil thrown together thing to track memleaks and total usage/max usage

struct MemoryProfiler {
    std::set<void*> m_allocTracker;
    uint32_t max = 0;

    void clear() {
        if(!m_allocTracker.empty()) {
            PrintDebug("==memleak==");
            for(void* p : m_allocTracker) {
                PrintDebug("leaked entry: %x size %d", p, *(uint32_t*)p);
            }
        }

        m_allocTracker.clear();
    }

    uint32_t calcTotal() const {
        uint32_t total = 0;
        for(void* p : m_allocTracker) {
            total += *(uint32_t*)p;
        }

        return total;
    }

    void calcAndPrintTotal() {
        uint32_t total = calcTotal();

        if(total > max) {
            max = total;
        }

        PrintDebug("[mem] total: %f MB max: %f MB", total / float(1024*1024), max / float(1024*1024));
    }

    void* alloc(size_t size) {
        uint8_t* sizeAndData = (uint8_t*)malloc(4 + size);
        uint32_t* pSize = (uint32_t*)sizeAndData;
        void* ptr = (void*)(sizeAndData + 4);

        *pSize = size;

        m_allocTracker.insert(sizeAndData);
        calcAndPrintTotal();

        return ptr;
    }

    void free(void* ptr) {
        if (ptr) {
            uint8_t* sizePtr = (uint8_t*)ptr - 4;

            m_allocTracker.erase(sizePtr);
            calcAndPrintTotal();

            ::free(sizePtr);
        }
    }
};