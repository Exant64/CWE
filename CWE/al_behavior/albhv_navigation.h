#pragma once

#include "stdafx.h"

#include <Chao.h>
#include "move.h"

enum class NAVIGATION_TYPE {
    AIM,
    LOCKON
};

template <NAVIGATION_TYPE type>
int ALBHV_SetNaviTarget(task* tp) {
    auto work = GET_CHAOWK(tp);
    MOVE_WORK* move = (MOVE_WORK*)tp->EntityData2;

    switch (type) {
        case NAVIGATION_TYPE::AIM:
            work->NaviTargetPos = move->AimPos;
            break;
        case NAVIGATION_TYPE::LOCKON:
            work->NaviTargetPos = ALW_GetLockOnTask(tp)->Data1.Entity->Position;
            break;
    }

    return BHV_RET_FINISH;
}

int ALBHV_CheckNavigate(task* tp);
int ALBHV_Navigation(task* tp);
void CreatePathAtPos(size_t chaoID, NJS_POINT3& endPos);