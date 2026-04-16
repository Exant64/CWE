#pragma once

#include <code_system/cwe_code.h>

class BetterCameraControl : public Code {
    private:
        enum {
            NONE = 0,
            KEY_NO_NUMPAD = 1,
            KEY_NUMPAD = 2,
            CONTROLLER = 3
        };

        bool UpPressed() const;
        bool DownPressed() const;
        bool LeftPressed() const;
        bool RightPressed() const;
        bool RightStickUpPressed() const;
        bool RightStickDownPressed() const;
        bool ResetPressed() const;
	public:
		using Code::Code;
		void OnFrame() override;
};

DataPointer(task*, pCamTask, 0x01AED2E8);

struct AL_NormalCameraExecutor_Data1 {
    EntityData1 EntityData1;
    unsigned __int16 type;
    NJS_POINT3 velo;
    NJS_POINT3 AimPos;
    Rotation AngSpd;
    NJS_POINT3 target;
    NJS_POINT3 AimTarget;
    float dist;
    int ViewAngle;
    float TargetNormalOffsetY;
    float TargetTreeOffsetY;
    float PosOffsetY;
    float PosItpRatio;
    int StopFlag;
};

typedef struct 
{
    uint32_t capabilities;  /* capability flags                                     */
    uint32_t button;        /* button flags                                         */
    int32_t  x1;            /* left stick, X axis                      (-127<->127) */
    int32_t  y1;            /* left stick, Y axis                      (-127<->127) */
    int32_t  x2;            /* right stick, X axis                     (-127<->127) */
    int32_t  y2;            /* right stick, Y axis                     (-127<->127) */
    int32_t  l;             /* left trigger                               (0<->255) */
    int32_t  r;             /* right trigger                              (0<->255) */
}
SOC_PERIPHERAL;

// thanks shad
DataArray(SOC_PERIPHERAL, SocPeripheral, 0x1A529E8, 4);

enum {
    InternalButtonIDs_Left  = 1,
    InternalButtonIDs_Right  = 2,
    InternalButtonIDs_Down  = 4,
    InternalButtonIDs_Up  = 8,
    InternalButtonIDs_Z  = 0x10,
    InternalButtonIDs_R  = 0x20,
    InternalButtonIDs_L  = 0x40,
    InternalButtonIDs_A  = 0x100,
    InternalButtonIDs_B  = 0x200,
    InternalButtonIDs_X  = 0x400,
    InternalButtonIDs_Y  = 0x800,
    InternalButtonIDs_Start  = 0x1000,
};

bool BetterCameraControl::UpPressed() const {
    if(m_param == KEY_NO_NUMPAD) {
        // VKEY = ascii for digits and letters
        return GetAsyncKeyState('R');
    }

    if(m_param == KEY_NUMPAD) {
        return GetAsyncKeyState(VK_PRIOR);
    }

    return (SocPeripheral[0].button & InternalButtonIDs_Up) != 0;
}

bool BetterCameraControl::DownPressed() const {
    if(m_param == KEY_NO_NUMPAD) {
        // VKEY = ascii for digits and letters
        return GetAsyncKeyState('F');
    }

    if(m_param == KEY_NUMPAD) {
        return GetAsyncKeyState(VK_NEXT);
    }

    return (SocPeripheral[0].button & InternalButtonIDs_Down) != 0;
}

bool BetterCameraControl::LeftPressed() const {
    if(m_param == KEY_NO_NUMPAD) {
        return GetAsyncKeyState(VK_OEM_PLUS) || GetAsyncKeyState(VK_OEM_5);
    }

    if(m_param == KEY_NUMPAD) {
        return GetAsyncKeyState(VK_MULTIPLY);
    }

    return (SocPeripheral[0].button & InternalButtonIDs_Left) != 0;
}

bool BetterCameraControl::RightPressed() const {
    if(m_param == KEY_NO_NUMPAD) {
        return GetAsyncKeyState(VK_OEM_MINUS) || GetAsyncKeyState('6');
    }

    if(m_param == KEY_NUMPAD) {
        return GetAsyncKeyState(VK_SUBTRACT);
    }

    return (SocPeripheral[0].button & InternalButtonIDs_Right) != 0;
}

bool BetterCameraControl::RightStickUpPressed() const {
    if(m_param == KEY_NO_NUMPAD) {
        return GetAsyncKeyState('T');
    }

    if(m_param == KEY_NUMPAD) {
        return GetAsyncKeyState(VK_HOME);
    }

    return SocPeripheral[0].y2 >= 1 && SocPeripheral[0].y2 <= 130;
}

bool BetterCameraControl::RightStickDownPressed() const {
    if(m_param == KEY_NO_NUMPAD) {
        return GetAsyncKeyState('G');
    }

    if(m_param == KEY_NUMPAD) {
        return GetAsyncKeyState(VK_END);
    }

    return SocPeripheral[0].y2 <= -2 && SocPeripheral[0].y2 >= -128;
}

bool BetterCameraControl::ResetPressed() const {
    if(m_param == KEY_NO_NUMPAD) {
        return GetAsyncKeyState(VK_BACK);
    }

    if(m_param == KEY_NUMPAD) {
        return GetAsyncKeyState(VK_DIVIDE);
    }

    return (SocPeripheral[0].button & InternalButtonIDs_L) && (SocPeripheral[0].button & InternalButtonIDs_R);
}

void BetterCameraControl::OnFrame() {
    if(!pCamTask) return;
    if(GameState != GameStates_Ingame) return;

    AL_NormalCameraExecutor_Data1* work = (AL_NormalCameraExecutor_Data1*)pCamTask->Data1.Undefined;

    // shad has the struct for these but i just cannot care to look it up atm
    DataPointer(int, CameraModeThing, 0x01DD0194);

    if(NextChaoArea <= 3 || NextChaoArea == 7) {
        if(UpPressed()) {
            CameraModeThing = 1;

            if(NextChaoArea != 7) {
                work->PosOffsetY += 1;
            }
        }

        if(DownPressed()) {
            CameraModeThing = 1;

            if(NextChaoArea != 7) {
                work->PosOffsetY -= 1;
            }
        }

        if(RightStickUpPressed()) {
            work->TargetNormalOffsetY += 0.5f;
        }

        if(RightStickDownPressed()) {
            work->TargetNormalOffsetY -= 0.5f;
        }

        if(LeftPressed()) {
            CameraModeThing = 2;
        }

        if(RightPressed()) {
            CameraModeThing = 0;
        }
    }

    if(ResetPressed()) {
        CameraModeThing = 1;

        work->TargetNormalOffsetY = 4.5f;
        work->PosOffsetY = 5.f;
    }

    if(work->PosOffsetY < 0) {
        work->PosOffsetY = 0;
    }

    if(work->PosOffsetY >= 100) {
        work->PosOffsetY = 100;
    }

    if(work->TargetNormalOffsetY < -4.5f) {
        work->TargetNormalOffsetY = -4.5f;
    }

    if(work->TargetNormalOffsetY > 24.5f) {
        work->TargetNormalOffsetY = 24.5f;
    }
}