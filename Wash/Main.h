#pragma once
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <io.h>
#include "vmp.h"
#include "Wash.h"
#include "resource.h"

extern BOOL bOnOff;
extern CWash * Wash;
extern char infoPath[260];

void FormThread(HINSTANCE hinst);