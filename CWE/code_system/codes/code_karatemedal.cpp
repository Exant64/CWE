#include "stdafx.h"

#include "include/race/code_karatemedal.h"

#include <alg_karate_main.h>
#include <FunctionHook.h>

FunctionHook<void, task*> sub_5428C0_t (0x5428C0);

static void sub_5428C0_r(task* tp) {
    if (KarateMainExec_Ptr->Round == 5) {
        const auto pParam = &KarateMainExec_Ptr->field_18->data;
        switch(KarateMainExec_Ptr->byte6) {
            case 0:
                pParam->Medal = 12;
                break;
            case 1:
                pParam->Medal = 14;
                break;
            case 2:
                pParam->Medal = 15;
                break;
            case 3:
                pParam->Medal = 13;
                break;
        }
    }
    
    sub_5428C0_t.Original(tp);
}

void KarateGiveMedal::Init() {
    sub_5428C0_t.Hook(sub_5428C0_r);
}