#include "stdafx.h"
#include <AL_ModAPI.h>
#include <ChaoMain.h>
#include <chao/al_motion/alm_all.h>

void ALAPI_RegisterMotion(CWE_REGAPI* cwe_api) {
	//animation fix for ALS_SassyBye
	NJS_MDATA2* mdata = (NJS_MDATA2*)cwe_api->GetChaoAnimation(450)->NJS_MOTION->mdata;
	mdata[1].p[1] = 0;
	mdata[1].nb[1] = 0;

	cwe_api->RegisterChaoAnimation("alm_sunaasubi", &alm_sunaasubi);
	cwe_api->RegisterChaoAnimation("alm_triangle", &alm_triangle);
	cwe_api->RegisterChaoAnimation("alm_accordion", &alm_accordion);
	cwe_api->RegisterChaoAnimation("alm_tailsfly", &alm_tailsfly);
	cwe_api->RegisterChaoAnimation("alm_tailsfly_tired", &alm_tailsfly_tired);
	cwe_api->RegisterChaoAnimation("alm_dance_n", &alm_dance_n);
	cwe_api->RegisterChaoAnimation("alm_dance_h", &alm_dance_h);
	cwe_api->RegisterChaoAnimation("alm_dance_d", &alm_dance_d);
	cwe_api->RegisterChaoAnimation("alm_guitar", &alm_guitar);
	cwe_api->RegisterChaoAnimation("alm_float", &alm_float);
	cwe_api->RegisterChaoAnimation("alm_duck", &alm_duck);

	cwe_api->RegisterChaoAnimation("alm_wrench", &alm_wrench);
	cwe_api->RegisterChaoAnimation("alm_wrench_start", &alm_wrench_start);
	cwe_api->RegisterChaoAnimation("alm_wrench_start2", &alm_wrench_start2);
	cwe_api->RegisterChaoAnimTransition("alm_wrench_start", "alm_wrench_start2");
	cwe_api->RegisterChaoAnimTransition("alm_wrench_start2", "alm_wrench");

	cwe_api->RegisterChaoAnimation("alm_egg_crawl", &alm_egg_crawl);
	cwe_api->RegisterChaoAnimation("alm_egg_crawl_start", &alm_egg_crawl_start);
	cwe_api->RegisterChaoAnimTransition("alm_egg_crawl_start", "alm_egg_crawl");
}