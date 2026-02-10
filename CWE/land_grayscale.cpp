#include "stdafx.h"
#include "al_stage.h"

// thank you shad for this code!

typedef struct ECW_HSV {
    float h,s,v;
} ECW_HSV;

void
ECW_ColorRGBtoHSV(NJS_ARGB* rgb, ECW_HSV* hsv)
{
    const float max = NJM_MAX(NJM_MAX(rgb->r, rgb->g), rgb->b);
    const float min = NJM_MIN(NJM_MIN(rgb->r, rgb->g), rgb->b);
    const float delta = max - min;

    if (delta > 0) {
        if (max == rgb->r) {
            hsv->h = 60 * (fmod(((rgb->g - rgb->b) / delta), 6));
        }
        else if (max == rgb->g) {
            hsv->h = 60 * (((rgb->b - rgb->r) / delta) + 2);
        }
        else if (max == rgb->b) {
            hsv->h = 60 * (((rgb->r - rgb->g) / delta) + 4);
        }

        if (max > 0) {
            hsv->s = delta / max;
        }
        else {
            hsv->s = 0;
        }

        hsv->v = max;
    }
    else {
        hsv->h = 0;
        hsv->s = 0;
        hsv->v = max;
    }

    if (hsv->h < 0) {
        hsv->h = 360 + hsv->h;
    }
}

void
ECW_ColorHSVtoRGB(ECW_HSV* hsv, NJS_ARGB* rgb)
{
    const float chroma = hsv->v * hsv->s;
    const float prime = fmod(hsv->h / 60.0, 6);
    const float x = chroma * (1 - fabs(fmod(prime, 2) - 1));
    const float m = hsv->v - chroma;

    switch ((int)prime) {
    case 0:
        rgb->r = chroma;
        rgb->g = x;
        rgb->b = 0;
        break;

    case 1:
        rgb->r = x;
        rgb->g = chroma;
        rgb->b = 0;
        break;

    case 2:
        rgb->r = 0;
        rgb->g = chroma;
        rgb->b = x;
        break;

    case 3:
        rgb->r = 0;
        rgb->g = x;
        rgb->b = chroma;
        break;

    case 4:
        rgb->r = x;
        rgb->g = 0;
        rgb->b = chroma;
        break;

    case 5:
        rgb->r = chroma;
        rgb->g = 0;
        rgb->b = x;
        break;

    default: // 6
        rgb->r = 0;
        rgb->g = 0;
        rgb->b = 0;
        break;
    }

    rgb->r += m;
    rgb->g += m;
    rgb->b += m;
}

static void ECW_ColorSaturate255(NJS_BGRA* col, float saturation) {
    NJS_ARGB argb = {
        .r = col->a / 255.f,
        .g = col->r / 255.f,
        .b = col->g / 255.f
    };

    ECW_HSV hsv;
    ECW_ColorRGBtoHSV(&argb, &hsv);
    hsv.s = saturation;
    ECW_ColorHSVtoRGB(&hsv, &argb);

    col->a = Uint8(argb.r * 255.f);
    col->r = Uint8(argb.g * 255.f);
    col->g = Uint8(argb.b * 255.f);
}

static void GreyscaleLandtable(LandTable* pLandTable) {
    for (size_t i = 0; i < pLandTable->ChunkModelCount; i++) {
		const auto pCOL = &pLandTable->COLList[i];

        auto pVertex = pCOL->Model->getsa2bmodel()->Vertices;

        while(pVertex->DataType != -1) {
            if(pVertex->DataType == 3) {
                NJS_BGRA* pBgra = (NJS_BGRA*)pVertex->Data;

                for(size_t j = 0; j < pVertex->ElementCount; ++j) {
                    ECW_ColorSaturate255(&pBgra[j], 0);
                }
            }

            pVertex++;
        }
	}
}

void GrayscalifyCurrentLandtable() {
    static uint32_t GreyscaledAlreadyFlag = 0;

    const uint32_t flag = (1 << AL_GetStageNumber());
    if(!(GreyscaledAlreadyFlag & flag)) {
        GreyscaledAlreadyFlag |= flag;

        GreyscaleLandtable(CurrentLandTable);
    }
}