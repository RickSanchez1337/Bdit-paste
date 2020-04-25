#pragma once

#include <Windows.h>
#include <Process.h>

#include <iostream>
#include <sstream>

#include <thread>
#include <chrono>

#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//ImGui
#include "imgui/imgui.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"

#define M_PI 3.14159265358979323846264338327950288419716939937510

//Resources
extern ID3DXLine* pLine;

extern ID3DXFont* pFontVisualsLarge;
extern ID3DXFont* pFontVisualsSmall;

extern ID3DXFont* pFontUILarge;
extern ID3DXFont* pFontUISmall;

//DX Stuff
void D3DRender();
BOOL D3DInitialize(HWND hWnd);

//Drawing Functions
void DrawString(const char* string, float x, float y, int r, int g, int b, int a, ID3DXFont* pFont);
void DrawStringOutline(const char* string, float x, float y, int r, int g, int b, int a, ID3DXFont* pFont);
void DrawRect(RECT drawRect, int r, int g, int b, int a);
void DrawRectOutlined(RECT drawRect, int r, int g, int b, int a);
void DrawLine(float x, float y, float x2, float y2, int r, int g, int b, int a, float w);
void DrawCrosshair(float r, float g, float b, float size);
void FillRect(RECT drawRect, int r, int g, int b, int a);
void FillRectXY(float x, float y, float x2, float y2, int r, int g, int b, int a);
void DrawCircle(int X, int Y, int radius, int numSides, int r, int g, int b, int a);

//Math
void iGetFPS(int* fps);
