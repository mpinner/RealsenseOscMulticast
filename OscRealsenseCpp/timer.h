/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#pragma once
#include <windows.h>

class FPSTimer {
public:

	FPSTimer(void);
	void Tick();

protected:

	LARGE_INTEGER freq, last;
	int fps;
};

