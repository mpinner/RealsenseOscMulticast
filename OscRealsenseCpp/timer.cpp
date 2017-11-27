/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "timer.h"


FPSTimer::FPSTimer(void) {
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&last);
	fps = 0;
}

void FPSTimer::Tick() {
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	fps++;
	if (now.QuadPart-last.QuadPart>freq.QuadPart) { // update every second
		last = now;

		printf("FPS=%d\n", fps);

		fps=0;
	}
}

