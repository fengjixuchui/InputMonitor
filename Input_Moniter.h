#pragma once
extern "C" BOOL _declspec(dllexport)InstallHook();
extern "C" void _declspec(dllexport)UnHook();
#define InputMonitorMsg WM_USER+1
