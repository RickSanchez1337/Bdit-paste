#include "commons.h"

#include "driver_ctrl.h"
#include "utils/mmap/mmap.h"
#include "security.h"

#include "renderer/menu/menu.h"
#include "renderer/D3D/Direct3D.hpp"

#include "sdk/classes.h"
#include "settings.h"

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"wininet.lib")

std::string folderpath = "C:\\Aspect Rust";
std::string dllpath = "C:\\Aspect Rust\\Fix.dll";
std::string dllpath2 = "C:\\Aspect Rust\\Aspect.dll";
std::string driverpath = "C:\\Aspect Rust\\Aspect.sys";
std::string dllurl = "https://aspectnetwork.net/aspect/rust/download/Fix.dll";
std::string dllurl2 = "https://aspectnetwork.net/aspect/rust/download/Aspect.dll";
std::string driverurl = "https://aspectnetwork.net/aspect/rust/download/Aspect.sys";

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
void WINAPI UpdateSurface(HWND hWnd);
HWND WINAPI InitializeWin(HINSTANCE hInst);
void UpdateWinPosition();

typedef struct _MyUncStr
{
	char stub[0x10];
	int len;
	wchar_t str[1];
} *pMyUncStr;

std::vector<std::uint32_t> GetProcessIds(const std::wstring& processName);
Security sec;
Menu menu;
HWND GameWindow = NULL;
HWND ThisWindow = NULL;
char WINNAME[19] = " ";
HINSTANCE asdd;
DWORD GetThreadId(DWORD dwProcessId);
uintptr_t GABaseAddress;
uintptr_t UnityBaseAddress;
uintptr_t GOM;
uintptr_t BaseNetworkable;
uintptr_t TOD_Sky;
BasePlayer LocalPlayer;
int ScreenWidth;
int ScreenHeight;
int ticks = 0;
int beforeclock = 0;
int FPS = 0;
bool overlaycreated = false;
bool InjectTheDll = true;
bool MenuOpen = true;
bool PrintedAddresses = false;

wchar_t LicenseHash;


MARGINS MARGIN = { 0, 0, ScreenWidth, ScreenHeight };

IDirect3DDevice9Ex* pDevice;
IDirect3D9Ex* pInstance;
D3DPRESENT_PARAMETERS pParams;

ID3DXFont* pFontVisualsLarge;
ID3DXFont* pFontVisualsSmall;

ID3DXFont* pFontUILarge;
ID3DXFont* pFontUISmall;

ID3DXLine* pLine;

char buf[128];

//Overlay FPS
float flCurTick, flOldTick;
int iTempFPS, iRenderFPS;


BOOL D3DInitialize(HWND hWnd) {

	//Direct3D
	Direct3DCreate9Ex(D3D_SDK_VERSION, &pInstance);

	pParams.Windowed = TRUE;
	pParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	pParams.BackBufferHeight = ScreenHeight;
	pParams.BackBufferWidth = ScreenWidth;
	pParams.MultiSampleQuality = DEFAULT_QUALITY;
	pParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pParams.EnableAutoDepthStencil = TRUE;
	pParams.AutoDepthStencilFormat = D3DFMT_D16;
	pParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	pInstance->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pParams, 0, &pDevice);

	if (pDevice == NULL) { return FALSE; }

	D3DXCreateFont(pDevice, 14, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma", &pFontVisualsLarge);
	D3DXCreateFont(pDevice, 12, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma", &pFontVisualsSmall);

	D3DXCreateFont(pDevice, 14, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma", &pFontUILarge);
	D3DXCreateFont(pDevice, 12, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma", &pFontUISmall);

	D3DXCreateLine(pDevice, &pLine);
	return TRUE;
}

void D3DRender() { //wtf this skid added half imgui xdddddd 
	pParams.BackBufferHeight = ScreenHeight;
	pParams.BackBufferWidth = ScreenWidth;
	pDevice->Reset(&pParams);
	pDevice->Clear(NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0, 0), 1.0f, NULL);//->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	pDevice->BeginScene();

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.62f, 0.93f, 0.06f, 1.00f);

	ImGui_ImplDX9_NewFrame();
	ImGui::SetNextWindowSize(ImVec2(460, 410));
	ImGui::Begin("Aspect", 0,
		ImGuiWindowFlags_NoCollapse | ImGuiConfigFlags_NoMouseCursorChange);
	int id = 1;
	if (ImGui::BeginTabBar("Aspect_tab_bar"))
	{
		if (ImGui::BeginTabItem("Player ESP"))
		{
			//ImGui::Checkbox("- ESP Enabled", &settings::player::enabled);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
	ImGui::Render();


	pDevice->EndScene();
	pDevice->PresentEx(0, 0, 0, 0, 0);
}

void DrawCrosshair(float r, float g, float b, float size) {
	//DrawLine(((swidth / 2) - 10.0f * size) - 1.0f, (sheight / 2) + 0.006f, ((swidth / 2) + 10.5f * size) + 1.0f, (sheight / 2) + 0.006f, 1, 1, 1, 255, 3.0f);
	//DrawLine((swidth / 2) - 0.006f, ((sheight / 2) - 10.0f * size) - 1.0f, (swidth / 2) - 0.006f, ((sheight / 2) + 11.0f * size) + 0.006f, 1, 1, 1, 255, 3.0f);
	//DrawLine(((swidth / 2) - 10 * size), sheight / 2, ((swidth / 2) + 10.5f * size), sheight / 2, r, g, b, 255, 1.0f);
	//DrawLine(swidth / 2, ((sheight / 2) - 10.0f * size), swidth / 2, ((sheight / 2) + 11.0f * size), r, g, b, 255, 1.0f);
}

void DrawString(const char* string, float x, float y, int r, int g, int b, int a, ID3DXFont* pFont) {
	RECT rPosition;
	rPosition.left = x;
	rPosition.top = y;
	pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(r, g, b, a));
}

void DrawWString(const char* string, float x, float y, int r, int g, int b, int a) {
	RECT rPosition;
	rPosition.left = x;
	rPosition.top = y;
	//pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(r, g, b, a));
}

void DrawStringOutline(const char* string, float x, float y, int r, int g, int b, int a, ID3DXFont* pFont) {
	RECT rPosition;

	rPosition.left = x + 1.0f;
	rPosition.top = y;

	pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(1, 1, 1, a));

	rPosition.left = x - 1.0f;
	rPosition.top = y;

	pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(1, 1, 1, a));

	rPosition.left = x;
	rPosition.top = y + 1.0f;

	pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(1, 1, 1, a));

	rPosition.left = x;
	rPosition.top = y - 1.0f;

	pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(1, 1, 1, a));

	rPosition.left = x;
	rPosition.top = y;

	pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(r, g, b, a));
}

void DrawRect(RECT drawRect, int r, int g, int b, int a) {
	D3DXVECTOR2 points[5];
	pLine->SetWidth(5);

	float x = drawRect.left;
	float y = drawRect.top;
	float x2 = drawRect.right;
	float y2 = drawRect.bottom;

	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x2, y);
	points[2] = D3DXVECTOR2(x2, y2);
	points[3] = D3DXVECTOR2(x, y2);
	points[4] = D3DXVECTOR2(x, y);

	pLine->Begin();
	pLine->Draw(points, 5, D3DCOLOR_RGBA(r, g, b, a));
	pLine->End();
}

void DrawLine(float x, float y, float x2, float y2, int r, int g, int b, int a, float w) {
	D3DXVECTOR2 points[2];
	pLine->SetWidth(w);

	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x2, y2);

	pLine->Begin();
	pLine->Draw(points, 2, D3DCOLOR_RGBA(r, g, b, a));
	pLine->End();
}

void DrawRectOutlined(RECT drawRect, int r, int g, int b, int a) {
	D3DXVECTOR2 points[5];

	float x = drawRect.left;
	float y = drawRect.top;
	float x2 = drawRect.right;
	float y2 = drawRect.bottom;

	points[0] = D3DXVECTOR2(x + 0.25f, y + 0.25f);
	points[1] = D3DXVECTOR2(x2 + 0.25f, y + 0.25f);
	points[2] = D3DXVECTOR2(x2 + 0.25f, y2 + 0.25f);
	points[3] = D3DXVECTOR2(x + 0.25f, y2 + 0.25f);
	points[4] = D3DXVECTOR2((x + 0.25f), y + 0.25f);

	pLine->SetWidth(3.5f);
	pLine->Draw(points, 5, D3DCOLOR_RGBA(1, 1, 1, a));

	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x2, y);
	points[2] = D3DXVECTOR2(x2, y2);
	points[3] = D3DXVECTOR2(x, y2);
	points[4] = D3DXVECTOR2(x, y);

	pLine->SetWidth(1);
	pLine->Begin();
	pLine->Draw(points, 5, D3DCOLOR_RGBA(r, g, b, a));
	pLine->End();
}

void FillRect(RECT drawRect, int r, int g, int b, int a) {
	D3DRECT RectDraw = { drawRect.left, drawRect.top, drawRect.right, drawRect.bottom };

	pDevice->Clear(1, &RectDraw, D3DCLEAR_TARGET | D3DCLEAR_TARGET, D3DCOLOR_RGBA(r, g, b, a), 0, 0);
}

void FillRectXY(float x, float y, float x2, float y2, int r, int g, int b, int a) {
	D3DXVECTOR2 points[2];

	int h = x2 - x;
	int w = y2 - y;

	pLine->SetWidth(h);

	points[0].x = x; points[0].y = y + (h >> 1);
	points[1].x = x + w; points[1].y = y + (h >> 1);

	pLine->Begin();
	pLine->Draw(points, 2, D3DCOLOR_RGBA(r, g, b, a));
	pLine->End();
}

void DrawCircle(int X, int Y, int radius, int numSides, int r, int g, int b, int a)
{
	D3DXVECTOR2 Line[128];
	float Step = M_PI * 2.0 / numSides;
	int Count = 0;
	for (float a = 0; a < M_PI * 2.0; a += Step)
	{
		float X1 = radius * cos(a) + X;
		float Y1 = radius * sin(a) + Y;
		float X2 = radius * cos(a + Step) + X;
		float Y2 = radius * sin(a + Step) + Y;
		Line[Count].x = X1;
		Line[Count].y = Y1;
		Line[Count + 1].x = X2;
		Line[Count + 1].y = Y2;
		Count += 2;
	}
	pLine->Begin();
	pLine->Draw(Line, Count, D3DCOLOR_RGBA(r, g, b, a));
	pLine->End();
}

void DrawStringOutlinexxx(char* string, float x, float y, int r, int g, int b, int a, ID3DXFont* pFont) {
	RECT rPosition;

	rPosition.left = x + 1.0f;
	rPosition.top = y;

	pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(1, 1, 1, a));

	rPosition.left = x - 1.0f;
	rPosition.top = y;

	pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(1, 1, 1, a));

	rPosition.left = x;
	rPosition.top = y + 1.0f;

	pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(1, 1, 1, a));

	rPosition.left = x;
	rPosition.top = y - 1.0f;

	pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(1, 1, 1, a));

	rPosition.left = x;
	rPosition.top = y;

	pFont->DrawTextA(0, string, strlen(string), &rPosition, DT_NOCLIP, D3DCOLOR_RGBA(r, g, b, a));
}

void iGetFPS(int* fps) {

	flCurTick = clock() * 0.001f;
	iTempFPS++;
	if ((flCurTick - flOldTick) > 1.0f) {
		iRenderFPS = iTempFPS;

		iTempFPS = 0;
		flOldTick = flCurTick;
	}

	*(int*)fps = iRenderFPS;
}

float FOV = 300, curFOV;

BasePlayer closestPlayer;

class PlayerClass
{
public:
	uintptr_t Player;
	uintptr_t ObjectClass;
	std::string ClassName;
	std::string Name;
	std::wstring WName;
	Vector3 Position;
	bool IsLocalPlayer;
	int Health;
	int MaxHealth;

public:
	bool operator==(PlayerClass ent)
	{
		if (ent.Player == this->Player)
			return true;
		else
			return false;
	}
};

class OreClass
{
public:
	uintptr_t Ore;
};

class CollectableClass
{
public:
	uintptr_t Object;
};

class VehicleClass
{
public:
	uintptr_t Object;
	std::string Name;
};

class CrateClass
{
public:
	uintptr_t Object;
};

class StashClass
{
public:
	uintptr_t Object;
};

class ItemClass
{
public:
	uintptr_t Object;
	uintptr_t ObjectClass;
	std::wstring Name;
};

Vector3 localPos;

std::vector<BasePlayer> PlayerList;
std::vector<OreClass> OreList;
std::vector<CollectableClass> CollectibleList;
std::vector<VehicleClass> VehicleList;
std::vector<CrateClass> CrateList;
std::vector<StashClass> StashList;
std::vector<Item> DroppedItemList;

HANDLE hProcess = INVALID_HANDLE_VALUE;

Matrix4x4 pViewMatrix;

uint64_t scan_for_klass(const char* name)
{
	auto base = mex.GetModuleBase("GameAssembly.dll"); //mem::get_module_base(L"GameAssembly.dll");
	auto dos_header = mex.Read<IMAGE_DOS_HEADER>(base);
	auto data_header = mex.Read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (3 * 40));
	auto next_section = mex.Read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (4 * 40));
	auto data_size = next_section.VirtualAddress - data_header.VirtualAddress;

	if (strcmp((char*)data_header.Name, ".data")) {
		printf("[!] Section order changed\n");
	}

	for (uint64_t offset = data_size; offset > 0; offset -= 8) {
		char klass_name[0x100] = { 0 };
		auto klass = mex.Read<uint64_t>(base + data_header.VirtualAddress + offset);
		if (klass == 0) { continue; }
		auto name_pointer = mex.Read<uint64_t>(klass + 0x10);
		if (name_pointer == 0) { continue; }
		mex.ReadString(name_pointer, klass_name, sizeof(klass_name));
		if (!strcmp(klass_name, name)) {
			printf("[*] 0x%x -> %s\n", offset, name);
			return klass;
		}
	}

	printf("[!] Unable to find %s in scan\n", name);
	//exit(0);
}

std::uintptr_t get_base_player(std::uintptr_t entity)
{
	const auto unk1 = mex.Read<uintptr_t>(entity + 0x18);

	if (!unk1)
		return 0;
	return mex.Read<uintptr_t>(unk1 + 0x28);
}

std::uint32_t get_player_health(std::uint64_t entity)
{
	const auto base_player = get_base_player(entity);

	if (!base_player)
		return 0;

	const auto player_health = mex.Read<float>(base_player + 0x1F4);

	if (player_health <= 0.8f)
		return 0;

	return std::lround(player_health);
}

Vector3 GetVelocity(uintptr_t Entity)
{
	uintptr_t player_model = mex.Read<uintptr_t>(Entity + 0x118);
	return mex.Read<Vector3>(player_model + 0x1D4);
}

enum BoneList : int
{
	l_hip = 1,
	l_knee,
	l_foot,
	l_toe,
	l_ankle_scale,
	pelvis,
	penis,
	GenitalCensor,
	GenitalCensor_LOD0,
	Inner_LOD0,
	GenitalCensor_LOD1,
	GenitalCensor_LOD2,
	r_hip,
	r_knee,
	r_foot,
	r_toe,
	r_ankle_scale,
	spine1,
	spine1_scale,
	spine2,
	spine3,
	spine4,
	l_clavicle,
	l_upperarm,
	l_forearm,
	l_hand,
	l_index1,
	l_index2,
	l_index3,
	l_little1,
	l_little2,
	l_little3,
	l_middle1,
	l_middle2,
	l_middle3,
	l_prop,
	l_ring1,
	l_ring2,
	l_ring3,
	l_thumb1,
	l_thumb2,
	l_thumb3,
	IKtarget_righthand_min,
	IKtarget_righthand_max,
	l_ulna,
	neck,
	head,
	jaw,
	eyeTranform,
	l_eye,
	l_Eyelid,
	r_eye,
	r_Eyelid,
	r_clavicle,
	r_upperarm,
	r_forearm,
	r_hand,
	r_index1,
	r_index2,
	r_index3,
	r_little1,
	r_little2,
	r_little3,
	r_middle1,
	r_middle2,
	r_middle3,
	r_prop,
	r_ring1,
	r_ring2,
	r_ring3,
	r_thumb1,
	r_thumb2,
	r_thumb3,
	IKtarget_lefthand_min,
	IKtarget_lefthand_max,
	r_ulna,
	l_breast,
	r_breast,
	BoobCensor,
	BreastCensor_LOD0,
	BreastCensor_LOD1,
	BreastCensor_LOD2,
	collision,
	displacement
};

Vector3 GetPosition(uintptr_t transform)
{
	if (!transform) return Vector3{ 0.f, 0.f, 0.f };

	struct Matrix34 { BYTE vec0[16]; BYTE vec1[16]; BYTE vec2[16]; };
	const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

	int Index = mex.Read<int>(transform + 0x40);// *(PINT)(transform + 0x40);
	uintptr_t pTransformData = mex.Read<uintptr_t>(transform + 0x38);
	uintptr_t transformData[2];
	mex.ReadString((pTransformData + 0x18), &transformData, 16);
	//mex.Read(&transformData, (PVOID)(pTransformData + 0x18), 16);
	//safe_memcpy(&transformData, (PVOID)(pTransformData + 0x18), 16);

	size_t sizeMatriciesBuf = 48 * Index + 48;
	size_t sizeIndicesBuf = 4 * Index + 4;

	PVOID pMatriciesBuf = malloc(sizeMatriciesBuf);
	PVOID pIndicesBuf = malloc(sizeIndicesBuf);

	if (pMatriciesBuf && pIndicesBuf)
	{
		// Read Matricies array into the buffer
		mex.ReadString(transformData[0], pMatriciesBuf, sizeMatriciesBuf);
		//impl::memory->read(transformData[0], pMatriciesBuf, sizeMatriciesBuf);
		// Read Indices array into the buffer
		mex.ReadString(transformData[1], pIndicesBuf, sizeIndicesBuf);

		__m128 result = *(__m128*)((ULONGLONG)pMatriciesBuf + 0x30 * Index);
		int transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * Index);

		while (transformIndex >= 0)
		{
			Matrix34 matrix34 = *(Matrix34*)((ULONGLONG)pMatriciesBuf + 0x30 * transformIndex);
			__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));
			__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));
			__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));
			__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));
			__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));
			__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));
			__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

			result = _mm_add_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), *(__m128*)(&matrix34.vec0));
			try {
				transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * transformIndex);
			}
			catch (...)
			{
				// Do nothing
			}
		}

		return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
	}
}

bool WorldToScreen(const Vector3& EntityPos, Vector2& ScreenPos)
{
	Vector3 TransVec = Vector3(pViewMatrix._14, pViewMatrix._24, pViewMatrix._34);
	Vector3 RightVec = Vector3(pViewMatrix._11, pViewMatrix._21, pViewMatrix._31);
	Vector3 UpVec = Vector3(pViewMatrix._12, pViewMatrix._22, pViewMatrix._32);
	float w = Math::Dot(TransVec, EntityPos) + pViewMatrix._44;
	if (w < 0.098f) return false;
	float y = Math::Dot(UpVec, EntityPos) + pViewMatrix._42;
	float x = Math::Dot(RightVec, EntityPos) + pViewMatrix._41;
	ScreenPos = Vector2((ScreenWidth / 2) * (1.f + x / w), (ScreenHeight / 2) * (1.f - y / w));
	return true;
}

Vector3 GetBonePosition(uintptr_t Entity, int bone)
{
	uintptr_t player_model = mex.Read<uintptr_t>(Entity + 0x118);
	uintptr_t BoneTransforms = mex.Read<uintptr_t>(player_model + 0x48);
	uintptr_t entity_bone = mex.Read<uintptr_t>(BoneTransforms + (0x20 + (bone * 0x8)));
	return GetPosition(mex.Read<uintptr_t>(entity_bone + 0x10));
}

float GetFov(uintptr_t Entity, int Bone) {
	Vector2 ScreenPos;
	if (!WorldToScreen(GetBonePosition(Entity, Bone), ScreenPos))
		return 1000.f;
	return Math::Calc2D_Dist(Vector2(ScreenWidth / 2, ScreenHeight / 2), ScreenPos);
}

std::string get_class_name(std::uint64_t class_object)
{
	const auto object_unk = mex.Read<uintptr_t>(class_object);

	if (!object_unk)
		return {};

	return read_ascii(mex.Read<uintptr_t>(object_unk + 0x10), 64);
}

Vector3 get_obj_pos(std::uint64_t entity)
{
	const auto player_visual = mex.Read<uintptr_t>(entity + 0x8);

	if (!player_visual)
		return {};

	const auto visual_state = mex.Read<uintptr_t>(player_visual + 0x38);

	if (!visual_state)
		return {};

	return mex.Read<Vector3>(visual_state + 0x90);
}

Vector3 GetCurrentObjectPosition(std::uintptr_t entity)
{
	const auto unk1 = mex.Read<uintptr_t>(entity + 0x10);

	if (!unk1)
		return Vector3{ NULL, NULL, NULL };

	const auto unk2 = mex.Read<uintptr_t>(unk1 + 0x30);

	if (!unk2)
		return Vector3{ NULL, NULL, NULL };



	const auto unk3 = mex.Read<uintptr_t>(unk2 + 0x30);

	if (!unk3)
		return Vector3{ NULL, NULL, NULL };



	/* shouldn't be needed, but in case */
	if (!entity)
		return Vector3{ NULL, NULL, NULL };

	Vector2 ScreenPos;
	return get_obj_pos(unk3);
}


void SetAdminFlag(uintptr_t LocalPlayer)
{
	int flags = mex.Read<int>(LocalPlayer + 0x5B8);

	flags |= 4;

	mex.Write<uintptr_t>(LocalPlayer + 0x5B8, flags);
}

void SetAimingFlag(uintptr_t LocalPlayer)
{
	int flags = mex.Read<int>(LocalPlayer + 0x5B0);

	flags |= 16384;

	mex.Write<uintptr_t>(LocalPlayer + 0x5B0, flags);
}

void SetRunningFlag(uintptr_t LocalPlayer)
{
	int flags = mex.Read<int>(LocalPlayer + 0x5B0);

	flags |~ 8192;

	mex.Write<uintptr_t>(LocalPlayer + 0x5B0, flags);
}

void SetGroundAngles(std::uintptr_t LocalPlayer)
{
	auto BaseMovement = mex.Read<uintptr_t>(LocalPlayer + 0x5E8);
	if (!BaseMovement)
		return;

	mex.Write<float>(BaseMovement + 0xAC, 0.f); // private float groundAngle; // 0xAC
	mex.Write<float>(BaseMovement + 0xB0, 0.f); // private float groundAngleNew; // 0xB0
}

inline float distance_cursor(Vector2 vec)
{
	POINT p;
	if (GetCursorPos(&p))
	{
		float ydist = (vec.y - p.y);
		float xdist = (vec.x - p.x);
		float ret = sqrt(pow(ydist, 2) + pow(xdist, 2));
		return ret;
	}
}

Vector2 smooth(Vector2 pos)
{
	Vector2 center{ (float)(ScreenWidth / 2), (float)(ScreenHeight / 2) };
	Vector2 target{ 0, 0 };
	if (pos.x != 0) {
		if (pos.x > center.x) {
			target.x = -(center.x - pos.x);
			target.x /= 1;
			if (target.x + center.x > center.x * 2)
				target.x = 0;
		}

		if (pos.x < center.x) {
			target.x = pos.x - center.x;
			target.x /= 1;
			if (target.x + center.x < 0)
				target.x = 0;
		}
	}

	if (pos.y != 0) {
		if (pos.y > center.y) {
			target.y = -(center.y - pos.y);
			target.y /= 1;
			if (target.y + center.y > center.y * 2)
				target.y = 0;
		}

		if (pos.y < center.y) {
			target.y = pos.y - center.y;
			target.y /= 1;
			if (target.y + center.y < 0)
				target.y = 0;
		}
	}

	target.x /= settings::aimbot::smooth;
	target.y /= settings::aimbot::smooth;

	if (abs(target.x) < 1) {
		if (target.x > 0) {
			target.x = 1;
		}
		if (target.x < 0) {
			target.x = -1;
		}
	}
	if (abs(target.y) < 1) {
		if (target.y > 0) {
			target.y = 1;
		}
		if (target.y < 0) {
			target.y = -1;
		}
	}

	return target;
}

void SetThickBullet(uintptr_t Weapon)
{
	if (!Weapon)
		return;

	auto Held = mex.Read<uintptr_t>(Weapon + 0x98);

	if (!Held)
	{
		std::cout << "!Held" << std::endl;
		return;
	}
	auto projectile_list = mex.Read<uintptr_t>(Held + 0x338);

	if (!projectile_list)
	{
		std::cout << "!projectile_list" << std::endl;
		return;
	}

	auto projectile_array = mex.Read<uintptr_t>(projectile_list + 0x18);

	if (!projectile_array)
	{
		std::cout << "!projectile_array" << std::endl;
		return;
	}

	auto projectile_list_size = mex.Read<int>(projectile_list + 0x10);

	try
	{
		for (auto i = 0; i < projectile_list_size; i++)
		{

			auto current_projectile = mex.Read<uintptr_t>(projectile_array + (0x20 + (i * 8)));

			if (!current_projectile)
			{
				std::cout << "!current_projectile" << std::endl;
				continue;
			}

			if (true) {
				auto old_thickness = mex.Read<float>(current_projectile + 0x2C);


				if (old_thickness <= 0.f || old_thickness >= 1.f)
				{
					std::cout << "old_thickness" << std::endl;
					continue;
				}

				mex.Write<float>(current_projectile + 0x2C, 1.f);
				auto new_thickness = mex.Read<float>(current_projectile + 0x2C);
				printf("[Bullet Thickness] Changed %f.f to %f.f \n", old_thickness, new_thickness);

			}

			std::this_thread::sleep_for(std::chrono::microseconds(3));
		}
	}
	catch (...) {}
}

void Normalize(float& Yaw, float& Pitch) {
	if (Pitch < -89) Pitch = -89;
	else if (Pitch > 89) Pitch = 89;
	if (Yaw < -360) Yaw += 360;
	else if (Yaw > 360) Yaw -= 360;
}

void SetAlwaysDay()
{
	//auto TOD_Sky = mex.Read<uintptr_t>();
	if (TOD_Sky)
	{
		//std::cout << "TOD_SKY: " << std::hex << std::uppercase << TOD_Sky << std::endl;
		auto unk1 = mex.Read<uintptr_t>(TOD_Sky + 0xB8);
		auto instance_list = mex.Read<uintptr_t>(unk1 + 0x0);

		auto instances = mex.Read<uintptr_t>(instance_list + 0x10);
		//std::cout << "instances: " << std::hex << std::uppercase << instances << std::endl;
		auto tSky = mex.Read<uintptr_t>(instances + 0x20);
		//std::cout << "tSky: " << std::hex << std::uppercase << tSky << std::endl;
		auto TOD_CycleParameters = mex.Read<uintptr_t>(tSky + 0x38);
		//std::cout << "TOD_CycleParameters: " << std::hex << std::uppercase << TOD_CycleParameters << std::endl;
		mex.Write<float>(TOD_CycleParameters + 0x10, 12.f);
		//std::cout << "TOD_SKY Written!" << std::endl;
	}
}

void SetFatBullet(Item item, BasePlayer enemy)
{
	if (!item.IsItemGun())
		return;

	auto Held = mex.Read<uintptr_t>(item.Item + 0x98); //BaseProjectile

	auto CreatedProjectiles = mex.Read<uintptr_t>(Held + 0x338);

	//std::cout << "CreatedProjectiles: " << std::hex << std::uppercase << CreatedProjectiles << std::endl;

	auto CreatedProjectilesArray = mex.Read<uintptr_t>(CreatedProjectiles + 0x10);

	const auto size = mex.Read<int>(CreatedProjectiles + 0x18);

	for (int i = 0u; i < size; i++)
	{
		uintptr_t Projectile = mex.Read<uintptr_t>(CreatedProjectilesArray + (0x20 + (i * 0x8)));

		mex.Write<float>(Projectile + 0x2C, 1.5f);
		std::cout << "Thicc: " << mex.Read<float>(Projectile + 0x2C) << std::endl;
		//std::cout << "Projectile: " << std::hex << std::uppercase << Projectile << std::endl;
	}
}

void SetSilentAim(Item item, BasePlayer enemy)
{
	auto Held = mex.Read<uintptr_t>(item.Item + 0x98); //BaseProjectile

	auto CreatedProjectiles = mex.Read<uintptr_t>(Held + 0x338);

	//std::cout << "CreatedProjectiles: " << std::hex << std::uppercase << CreatedProjectiles << std::endl;

	auto CreatedProjectilesArray = mex.Read<uintptr_t>(CreatedProjectiles + 0x10);

	const auto size = mex.Read<int>(CreatedProjectiles + 0x18);

	for (int i = 0u; i < size; i++)
	{
		uintptr_t Projectile = mex.Read<uintptr_t>(CreatedProjectilesArray + (0x20 + (i * 0x8)));

		auto currentpos = mex.Read<Vector3>(Projectile + 0x124);
		std::cout << "Projectile Pos: " << std::hex << std::uppercase << currentpos.x << " | " << currentpos.y << " | " << currentpos.z << std::endl;
		auto enemypos = enemy.GetBonePosition(head);
		//mex.Write<float>(Projectile + 0x124, enemypos);
		//mex.Write<Vector3>(Projectile + 0x138, enemypos);
		//mex.Write<Vector3>(Projectile + 0x144, enemypos);
		auto newpos = mex.Read<Vector3>(Projectile + 0x124);
		std::cout << "New Pos: " << std::hex << std::uppercase << newpos.x << " | " << newpos.y << " | " << newpos.z << std::endl;
		//mex.Write<float>(Projectile + 0x2C, 5.f);
		//std::cout << "Projectile: " << std::hex << std::uppercase << Projectile << std::endl;
	}
}

void PrintProjectileSpeed(Item item, BasePlayer enemy)
{
	auto Held = mex.Read<uintptr_t>(item.Item + 0x98); //BaseProjectile

	auto CreatedProjectiles = mex.Read<uintptr_t>(Held + 0x338);

	//std::cout << "CreatedProjectiles: " << std::hex << std::uppercase << CreatedProjectiles << std::endl;

	auto CreatedProjectilesArray = mex.Read<uintptr_t>(CreatedProjectiles + 0x10);

	const auto size = mex.Read<int>(CreatedProjectiles + 0x18);

	for (int i = 0u; i < size; i++)
	{
		uintptr_t Projectile = mex.Read<uintptr_t>(CreatedProjectilesArray + (0x20 + (i * 0x8)));
		auto ItemModProjectile = mex.Read<uintptr_t>(Projectile + 0xE8);
		auto ProjectileSpeed = mex.Read<float>(ItemModProjectile + 0x34);
		std::cout << "Projectile Speed: " << ProjectileSpeed << std::endl;
	}
}

Vector3 Prediction(const Vector3& LP_Pos, BasePlayer Player, BoneList Bone)
{
	Vector3 BonePos = Player.GetBonePosition(Bone);
	float Dist = Math::Calc3D_Dist(LP_Pos, BonePos);

	if (Dist > 0.001f) {
		float BulletTime = Dist / LocalPlayer.GetHeldItem().GetBulletSpeed();
		Vector3 vel = Player.GetVelocity();
		Vector3 PredictVel = vel * BulletTime * 0.75f;
		BonePos += PredictVel;
		BonePos.y += (4.905f * BulletTime * BulletTime);
	} return BonePos;
}

void FatBulletThread()
{
	while (true)
	{
		try
		{
			if (settings::misc::fat_bullet)
			{
				SetFatBullet(LocalPlayer.GetHeldItem(), closestPlayer);
			}
		}
		catch (...)
		{
			std::cout << "FatBulletThread Error!" << std::endl;
		}
	}
}

void DoGameHax()
{
	while (true)
	{
		try
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			if (!LocalPlayer.IsSleeping())
			{
				if (settings::misc::debug_cam)
					LocalPlayer.SetAdminFlag();
				if (settings::misc::spider)
					LocalPlayer.DoSpider();
				if (settings::misc::automatic)
					LocalPlayer.GetHeldItem().SetAutomatic();
				if (settings::misc::no_recoil)
					LocalPlayer.GetHeldItem().SetNoRecoil();
				if (settings::misc::always_day)
					SetAlwaysDay();

				//std::cout << LocalPlayer.GetHeldItem().GetItemClassName().c_str() << std::endl;
			}
		}
		catch (...)
		{
			std::cout << "DGH Error." << std::endl;
		}
	}
}

void PlayerThreadFunc()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		UnityBaseAddress = mex.GetModuleBase("UnityPlayer.dll");
		//std::cout << "Unity Assembly: " << std::hex << std::uppercase << UnityBaseAddress << std::endl;
		if (!UnityBaseAddress)
		{
			//std::cout << "!UnityBaseAddress" << std::endl;
			continue;
		}

		GOM = mex.Read<uintptr_t>(UnityBaseAddress + 0x17A6AD8);
		if (!GOM)
		{
			//std::cout << "!GABaseAddress" << std::endl;
			continue;
		}

		GABaseAddress = mex.GetModuleBase("GameAssembly.dll");
		if (!GABaseAddress)
		{
			//std::cout << "!GABaseAddress" << std::endl;
			continue;
		}

		//std::cout << "Game Assembly: " << std::hex << std::uppercase << GABaseAddress << std::endl;

		BaseNetworkable = scan_for_klass("BaseNetworkable");
		if (!BaseNetworkable)
		{
			//std::cout << "!BaseNetworkable" << std::endl;
			continue;
		}

		TOD_Sky = scan_for_klass("TOD_Sky");
		if (!TOD_Sky)
		{
			//std::cout << "!TOD_Sky" << std::endl;
			continue;
		}

		const auto unk1 = mex.Read<uintptr_t>(BaseNetworkable + 0xB8);
		if (!unk1)
		{
			std::cout << "!unk1" << std::endl;
			return;
		}

		const auto client_entities = mex.Read<uintptr_t>(unk1);
		if (!client_entities)
		{
			std::cout << "!client_entities" << std::endl;
			return;
		}

		const auto entity_realm = mex.Read<uintptr_t>(client_entities + 0x10);
		if (!entity_realm)
		{
			std::cout << "!entity_realm" << std::endl;
			return;
		}

		const auto buffer_list = mex.Read<uintptr_t>(entity_realm + 0x28);
		if (!buffer_list)
		{
			std::cout << "!buffer_list" << std::endl;
			return;
		}

		const auto object_list = mex.Read<uintptr_t>(buffer_list + 0x18);
		if (!object_list)
		{
			std::cout << "!object_list" << std::endl;
			return;
		}

		const auto object_list_size = mex.Read<std::uint32_t>(buffer_list + 0x10);

		try
		{
			//std::ofstream classfile;
			//classfile.open("C:\\Aspect Rust\\classes.txt", std::ios_base::app);
			for (auto i = 0; i < object_list_size; i++)
			{
				const auto current_object = mex.Read<uintptr_t>(object_list + (0x20 + (i * 8)));

				if (!current_object)
				{
					//std::cout << "!current_object" << std::endl;
					continue;
				}
				//std::cout << "current_object: " << std::hex << std::uppercase << current_object << std::endl;
				const auto baseObject = mex.Read<uintptr_t>(current_object + 0x10);

				if (!baseObject)
					continue;

				const auto object = mex.Read<uintptr_t>(baseObject + 0x30);

				if (!object)
					continue;

				WORD tag = mex.Read<WORD>(object + 0x54);

				DWORD64 localElement = mex.Read<DWORD64>(object_list + 0x20);
				DWORD64 localBO = mex.Read<DWORD64>(localElement + 0x10);
				DWORD64 localPlayer = mex.Read<DWORD64>(localBO + 0x30);
				DWORD64 localOC = mex.Read<DWORD64>(localPlayer + 0x30);
				DWORD64 localT = mex.Read<DWORD64>(localOC + 0x8);
				DWORD64 localVS = mex.Read<DWORD64>(localT + 0x38);
				localPos = mex.Read<Vector3>(localVS + 0x90);

				std::string class_name = get_class_name(current_object);

				if (tag == 6)
				{
					char className[64];
					auto name_pointer = mex.Read<uint64_t>(object + 0x60);
					mex.ReadString(name_pointer, &className, sizeof(className));
					DWORD64 objectClass = mex.Read<DWORD64>(object + 0x30);
					DWORD64 entity = mex.Read<DWORD64>(objectClass + 0x18);
					uintptr_t player = mex.Read<uintptr_t>(entity + 0x28);

					//Get Player Position
					DWORD64 transform = mex.Read<DWORD64>(objectClass + 0x8);
					DWORD64 visualState = mex.Read<DWORD64>(transform + 0x38);
					Vector2 ScreenPos;
					Vector3 Pos = mex.Read<Vector3>(visualState + 0x90);
					//Get Player Name.
					auto Distance = Math::Calc3D_Dist(localPos, Pos);

					BasePlayer bp;
					bp.Player = player;
					bp.ObjectClass = objectClass;
					mex.ReadString(player, &bp.buffer, sizeof(bp.buffer));

					//Get Player Health
					auto player_health = mex.Read<float>(player + 0x1F4);
					float healthf = nearbyint(player_health);
					int health = (int)(healthf);
					//ent.TeamId = teamid;
					if (strcmp(className, "LocalPlayer") != 0)
					{
						bp.IsLocalPlayer = false;
						//if (teamid == LocalTeamID)
					}
					else
					{
						bp.IsLocalPlayer = true;
						LocalPlayer = bp;
						//LocalTeamID = teamid
						//printf("Object: %X\n", object);
						//std::cout << "object: " << std::hex << std::uppercase << object << std::endl;

					}

					const auto player_list_iter = std::find(PlayerList.begin(), PlayerList.end(), bp);

					if (player_list_iter != PlayerList.end() && health == 0)
					{
						PlayerList.erase(player_list_iter);
						continue;
					}
					else if (player_list_iter != PlayerList.end() && health > 0)
					{
						continue;
					}

					if (PlayerList.size() > 500)
						PlayerList.clear();
					PlayerList.push_back(bp);
				}

				else if (tag == 20009) //Player Corpse
				{
					continue;
				}

				else if (class_name.find("Stash") != std::string::npos)
				{
					continue;
				}

				else if (class_name.find("OreRe") != std::string::npos)
				{
					OreClass ore;
					ore.Ore = current_object;
					const auto unk1 = mex.Read<uintptr_t>(ore.Ore + 0x10);

					if (!unk1)
						continue;

					const auto unk2 = mex.Read<uintptr_t>(unk1 + 0x30);

					if (!unk2)
						continue;



					const auto unk3 = mex.Read<uintptr_t>(unk2 + 0x30);

					if (!unk3)
						continue;



					/* shouldn't be needed, but in case */
					if (!ore.Ore)
						continue;

					Vector2 ScreenPos;
					Vector3 OrePos = get_obj_pos(unk3);

					auto distance = Math::Calc3D_Dist(localPos, OrePos);
					if (distance < settings::ore::max_distance)
					{
						if (OreList.size() > 100)
							OreList.clear();
						OreList.push_back(ore);
					}

				}

				else if (class_name.find("LootCo") != std::string::npos)
				{
					CrateClass crate;
					crate.Object = current_object;
					auto pos = GetCurrentObjectPosition(current_object);
					auto distance = Math::Calc3D_Dist(localPos, pos);
					if (distance < 100)
					{
						if (CrateList.size() > 100)
							CrateList.clear();
						CrateList.push_back(crate);
					}
				}

				else if (class_name.find("Collectible") != std::string::npos)
				{
					CollectableClass object;
					object.Object = current_object;
					const auto unk1 = mex.Read<uintptr_t>(object.Object + 0x10);

					if (!unk1)
						continue;

					const auto unk2 = mex.Read<uintptr_t>(unk1 + 0x30);

					if (!unk2)
						continue;



					const auto unk3 = mex.Read<uintptr_t>(unk2 + 0x30);

					if (!unk3)
						continue;



					/* shouldn't be needed, but in case */
					if (!object.Object)
						continue;

					Vector2 ScreenPos;
					Vector3 OrePos = get_obj_pos(unk3);

					auto distance = Math::Calc3D_Dist(localPos, OrePos);
					if (distance < settings::collectable::max_distance)
					{
						if (CollectibleList.size() > 200)
							CollectibleList.clear();
						CollectibleList.push_back(object);
					}
				}

				else if (class_name == "MiniCopter")
				{
					VehicleClass object;
					object.Object = current_object;
					object.Name = "MiniCopter";
					const auto unk1 = mex.Read<uintptr_t>(object.Object + 0x10);

					if (!unk1)
						continue;

					const auto unk2 = mex.Read<uintptr_t>(unk1 + 0x30);

					if (!unk2)
						continue;



					const auto unk3 = mex.Read<uintptr_t>(unk2 + 0x30);

					if (!unk3)
						continue;



					/* shouldn't be needed, but in case */
					if (!object.Object)
						continue;

					Vector2 ScreenPos;
					Vector3 OrePos = get_obj_pos(unk3);

					auto distance = Math::Calc3D_Dist(localPos, OrePos);
					if (distance < 100)
					{
						if (VehicleList.size() > 100)
							VehicleList.clear();
						VehicleList.push_back(object);
					}
				}
				else if (class_name == "HotAirBalloon")
				{
					VehicleClass object;
					object.Object = current_object;
					object.Name = "HotAirBalloon";
					const auto unk1 = mex.Read<uintptr_t>(object.Object + 0x10);

					if (!unk1)
						continue;

					const auto unk2 = mex.Read<uintptr_t>(unk1 + 0x30);

					if (!unk2)
						continue;



					const auto unk3 = mex.Read<uintptr_t>(unk2 + 0x30);

					if (!unk3)
						continue;



					/* shouldn't be needed, but in case */
					if (!object.Object)
						continue;

					Vector2 ScreenPos;
					Vector3 OrePos = get_obj_pos(unk3);

					auto distance = Math::Calc3D_Dist(localPos, OrePos);
					if (distance < 100)
					{
						if (VehicleList.size() > 100)
							VehicleList.clear();
						VehicleList.push_back(object);
					}
				}
				else if (class_name == "ScrapTransportHelicopter")
				{
					VehicleClass object;
					object.Object = current_object;
					object.Name = "Scrap Helicopter";
					const auto unk1 = mex.Read<uintptr_t>(object.Object + 0x10);

					if (!unk1)
						continue;

					const auto unk2 = mex.Read<uintptr_t>(unk1 + 0x30);

					if (!unk2)
						continue;



					const auto unk3 = mex.Read<uintptr_t>(unk2 + 0x30);

					if (!unk3)
						continue;



					/* shouldn't be needed, but in case */
					if (!object.Object)
						continue;

					Vector2 ScreenPos;
					Vector3 OrePos = get_obj_pos(unk3);

					auto distance = Math::Calc3D_Dist(localPos, OrePos);
					if (distance < 100)
					{
						if (VehicleList.size() > 100)
							VehicleList.clear();
						VehicleList.push_back(object);
					}
				}
				else if (class_name == "MotorRowboat")
				{
					VehicleClass object;
					object.Object = current_object;
					object.Name = "Small Boat";
					const auto unk1 = mex.Read<uintptr_t>(object.Object + 0x10);

					if (!unk1)
						continue;

					const auto unk2 = mex.Read<uintptr_t>(unk1 + 0x30);

					if (!unk2)
						continue;



					const auto unk3 = mex.Read<uintptr_t>(unk2 + 0x30);

					if (!unk3)
						continue;



					/* shouldn't be needed, but in case */
					if (!object.Object)
						continue;

					Vector2 ScreenPos;
					Vector3 OrePos = get_obj_pos(unk3);

					auto distance = Math::Calc3D_Dist(localPos, OrePos);
					if (distance < 100)
					{
						if (VehicleList.size() > 100)
							VehicleList.clear();
						VehicleList.push_back(object);
					}
				}
				else if (class_name == "TreeEntity" || class_name == "SleepingBag" || class_name == "Door" || class_name == "BuildingBlock" || class_name == "KeyLock" || class_name == "OreHotSpot" || class_name == "PlantEntity" || class_name == "VendingMachineMapMarker" || class_name == "ExcavatorArm" || class_name == "EnvSync" || class_name == "FireBall" || class_name == "BaseVehicleSeat" || class_name == "ResourceEntity" || class_name == "PlantEntity" || class_name == "PlantEntity" || class_name == "PlantEntity" || class_name == "PlantEntity")
				{
					continue;
				}
				else if (class_name.find("DroppedItem") != std::string::npos)
				{
					auto CurrentItem = mex.Read<uintptr_t>(current_object + 0x150);
					DWORD64 objectClass = mex.Read<DWORD64>(object + 0x30);
					DWORD64 transform = mex.Read<DWORD64>(objectClass + 0x8);
					DWORD64 visualState = mex.Read<DWORD64>(transform + 0x38);
					Vector2 ScreenPos;
					Vector3 Pos = mex.Read<Vector3>(visualState + 0x90);
					auto distance = Math::Calc3D_Dist(localPos, Pos);
					if (distance < settings::dropped::max_distance)
					{
						if (DroppedItemList.size() > 100)
							DroppedItemList.clear();
						Item object;
						object.Item = CurrentItem;
						object.ObjectClass = objectClass;
						DroppedItemList.push_back(object);
					}
				}
				else if (tag == 0)
				{
					continue;
					//char className[0x100];
					//auto name_pointer = mex.Read<uint64_t>(object + 0x60);
					//mex.Read(name_pointer, &className, sizeof(className));
					//classfile << className << std::endl;
				}
			}
			//classfile.close();
		}

		catch (...)
		{

		}
	}
}

void drawLoop(int Width, int Height)
{
	ScreenWidth = Width;
	ScreenHeight = Height;
	//std::cout << "Setting Up Clocks!" << std::endl;
	ticks += 1;
	if (beforeclock == 0) {
		beforeclock = clock();
	}

	DrawCircle(ScreenWidth / 2, ScreenHeight / 2, settings::aimbot::fov, 3, 1, 0, 0, 1, false);

	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		MenuOpen = !MenuOpen;
	}

	menu.Render();
	//std::cout << "Check if menu open." << std::endl;

	//std::cout << "GOM: " << std::hex << std::uppercase << GOM << std::endl;
	if (!GOM)
	{
		std::cout << "!GOM" << std::endl;
		return;
	}

	DWORD64 taggedObjects = mex.Read<DWORD64>(GOM + 0x8);
	if (!taggedObjects)
	{
		std::cout << "!taggedObjects" << std::endl;
		return;
	}

	DWORD64 gameObject = mex.Read<DWORD64>(taggedObjects + 0x10);
	if (!gameObject)
	{
		std::cout << "!gameObject" << std::endl;
		return;
	}

	DWORD64 objClass = mex.Read<DWORD64>(gameObject + 0x30);
	if (!objClass)
	{
		std::cout << "!objClass" << std::endl;
		return;
	}

	DWORD64 ent = mex.Read<DWORD64>(objClass + 0x18);
	if (!ent)
	{
		std::cout << "!ent" << std::endl;
		return;
	}

	pViewMatrix = mex.Read<Matrix4x4>(ent + 0xDC);

	if (true)
	{
		FOV = settings::aimbot::fov;
		if (settings::player::enabled)
		{
			for (BasePlayer player : PlayerList)
			{
				auto pos = player.GetBonePosition(neck);
				if (player.IsLocalPlayer)
				{
					localPos = pos;
					LocalPlayer = player;
					continue;
				}

				auto player_health = player.GetHealth();
				float healthf = nearbyint(player_health);
				int health = (int)(healthf);

				auto distance = Math::Calc3D_Dist(localPos, pos);
				if (distance < 300 && player_health > 0)
				{
					Vector2 ScreenPos;
					if (WorldToScreen(pos, ScreenPos) && !player.IsSleeping())
					{
						curFOV = distance_cursor(ScreenPos);
						if (FOV > curFOV && !player.IsLocalPlayer && player.Player != LocalPlayer.Player)
						{
							FOV = curFOV;
							closestPlayer = player;
						}

						auto HeldGun = player.GetHeldItem().GetItemName();
						char healthbuffer[0x20]{};
						char distancebuffer[0x20]{};
						sprintf(healthbuffer, "[%d HP]", health);
						sprintf(distancebuffer, "[%dm]", (int)distance);
						if (player.GetPlayerModel().IsVisible())
						{
							int yoffset = 0;
							if (settings::player::name)
							{
								DrawWString(player.GetName(), 15, ScreenPos.x, ScreenPos.y + yoffset, 0, 1, 1);
								yoffset += 15;
							}
							if (settings::player::health)
							{
								DrawString(healthbuffer, 15, ScreenPos.x, ScreenPos.y + yoffset, 0, 1, 1);
								yoffset += 15;
							}
							if (settings::player::distance)
							{
								DrawString(distancebuffer, 15, ScreenPos.x, ScreenPos.y + yoffset, 0, 1, 1);
								yoffset += 15;
							}
							if (settings::player::held_item)
							{
								DrawWString(HeldGun, 15, ScreenPos.x, ScreenPos.y + yoffset, 0, 1, 1);
								yoffset += 15;
							}
							if (settings::player::hotbar)
							{
								if (curFOV <= 30)
								{
									for (int i = 0; i < 6; i++)
									{
										auto ItemName = player.GetPlayerInventory().GetBelt().GetItem(i).GetItemName();
										DrawWString(ItemName, 15, ScreenPos.x, ScreenPos.y + yoffset, 0, 1, 1);
										yoffset += 15;
									}
								}
							}
						}
						else
						{
							int yoffset = 0;
							if (settings::player::name)
							{
								DrawWString(player.GetName(), 15, ScreenPos.x, ScreenPos.y + yoffset, 1, 0, 0);
								yoffset += 15;
							}
							if (settings::player::health)
							{
								DrawString(healthbuffer, 15, ScreenPos.x, ScreenPos.y + yoffset, 1, 0, 0);
								yoffset += 15;
							}
							if (settings::player::distance)
							{
								DrawString(distancebuffer, 15, ScreenPos.x, ScreenPos.y + yoffset, 1, 0, 0);
								yoffset += 15;
							}
							if (settings::player::held_item)
							{
								DrawWString(HeldGun, 15, ScreenPos.x, ScreenPos.y + yoffset, 1, 0, 0);
								yoffset += 15;
							}
							if (settings::player::hotbar)
							{
								if (curFOV <= 30)
								{
									for (int i = 0; i < 6; i++)
									{
										auto ItemName = player.GetPlayerInventory().GetBelt().GetItem(i).GetItemName();
										if (ItemName == L"No Item!")
											continue;
										DrawWString(ItemName, 15, ScreenPos.x, ScreenPos.y + yoffset, 1, 0, 0);
										yoffset += 15;
									}
								}
							}
						}
					}
				}
			}
		}

		if (settings::ore::enabled)
		{
			for (OreClass ore : OreList)
			{
				const auto unk1 = mex.Read<uintptr_t>(ore.Ore + 0x10);

				if (!unk1)
					continue;

				const auto unk2 = mex.Read<uintptr_t>(unk1 + 0x30);

				if (!unk2)
					continue;



				const auto unk3 = mex.Read<uintptr_t>(unk2 + 0x30);

				if (!unk3)
					continue;



				/* shouldn't be needed, but in case */
				if (!ore.Ore)
					continue;




				const auto oreName = mex.Read<uintptr_t>(unk2 + 0x60);
				std::string name = read_ascii(oreName, 64);

				if (name.find("stone-ore") != std::string::npos)
				{
					if (settings::ore::stone)
					{
						Vector2 ScreenPos;
						Vector3 OrePos = get_obj_pos(unk3);

						if (WorldToScreen(OrePos, ScreenPos))
						{
							auto distance = Math::Calc3D_Dist(localPos, OrePos);
							if (distance < settings::ore::max_distance)
							{
								int yoffset = 0;
								if (settings::ore::name)
								{
									DrawString("Stone Ore", 15, ScreenPos.x, ScreenPos.y + yoffset, .5, 0, .5);
									yoffset += 15;
								}
								if (settings::ore::distance)
								{
									char buffer[0x100]{};
									sprintf(buffer, "[%dm]", (int)distance);
									DrawString(buffer, 15, ScreenPos.x, ScreenPos.y + yoffset, .5, 0, .5);
								}
							}
						}
					}
				}
				else if (name.find("metal-ore") != std::string::npos)
				{
					if (settings::ore::metal)
					{
						Vector2 ScreenPos;
						Vector3 OrePos = get_obj_pos(unk3);

						if (WorldToScreen(OrePos, ScreenPos))
						{
							auto distance = Math::Calc3D_Dist(localPos, OrePos);
							if (distance < settings::ore::max_distance)
							{
								int yoffset = 0;
								if (settings::ore::name)
								{
									DrawString("Metal Ore", 15, ScreenPos.x, ScreenPos.y + yoffset, .5, 0, .5);
									yoffset += 15;
								}
								if (settings::ore::distance)
								{
									char buffer[0x100]{};
									sprintf(buffer, "[%dm]", (int)distance);
									DrawString(buffer, 15, ScreenPos.x, ScreenPos.y + yoffset, .5, 0, .5);
								}
							}
						}
					}
				}
				else if (name.find("sulfur-ore") != std::string::npos)
				{
					if (settings::ore::sulfur)
					{
						Vector2 ScreenPos;
						Vector3 OrePos = get_obj_pos(unk3);

						if (WorldToScreen(OrePos, ScreenPos))
						{
							auto distance = Math::Calc3D_Dist(localPos, OrePos);
							if (distance < settings::ore::max_distance)
							{
								int yoffset = 0;
								if (settings::ore::name)
								{
									DrawString("Sulfur Ore", 15, ScreenPos.x, ScreenPos.y + yoffset, .5, 0, .5);
									yoffset += 15;
								}
								if (settings::ore::distance)
								{
									char buffer[0x100]{};
									sprintf(buffer, "[%dm]", (int)distance);
									DrawString(buffer, 15, ScreenPos.x, ScreenPos.y + yoffset, .5, 0, .5);
								}
							}
						}
					}
				}
			}
		}

		if (settings::collectable::enabled)
		{
			for (CollectableClass obj : CollectibleList)
			{
				const auto unk1 = mex.Read<uintptr_t>(obj.Object + 0x10);
				if (!unk1)
					continue;
				const auto unk2 = mex.Read<uintptr_t>(unk1 + 0x30);
				if (!unk2)
					continue;
				const auto unk3 = mex.Read<uintptr_t>(unk2 + 0x30);
				if (!unk3)
					continue;
				if (!obj.Object)
					continue;

				const auto objName = mex.Read<uintptr_t>(unk2 + 0x60);
				std::string name = read_ascii(objName, 64);
				if (name.find("hemp") != std::string::npos)
				{
					Vector2 ScreenPos;
					Vector3 OrePos = get_obj_pos(unk3);

					if (WorldToScreen(OrePos, ScreenPos))
					{
						auto distance = Math::Calc3D_Dist(localPos, OrePos);
						if (distance < settings::collectable::max_distance)
						{
							char buffer[0x100]{};
							sprintf(buffer, "Hemp\n[%dm]", (int)distance);
							auto text = s2ws(buffer);
							DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, 0, 1, 0);
						}
					}
				}
				else if (name.find("metal-collect") != std::string::npos)
				{
					Vector2 ScreenPos;
					Vector3 OrePos = get_obj_pos(unk3);

					if (WorldToScreen(OrePos, ScreenPos))
					{
						auto distance = Math::Calc3D_Dist(localPos, OrePos);
						if (distance < 300)
						{
							char buffer[0x100]{};
							sprintf(buffer, "Metal Col\n[%dm]", (int)distance);
							auto text = s2ws(buffer);
							DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, 0, 1, 0);
						}
					}
				}
				else if (name.find("sulfur") != std::string::npos)
				{
					Vector2 ScreenPos;
					Vector3 OrePos = get_obj_pos(unk3);

					if (WorldToScreen(OrePos, ScreenPos))
					{
						auto distance = Math::Calc3D_Dist(localPos, OrePos);
						if (distance < 300)
						{
							char buffer[0x100]{};
							sprintf(buffer, "Sulfur Col\n[%dm]", (int)distance);
							auto text = s2ws(buffer);
							DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, 0, 1, 0);
						}
					}
				}

				else if (name.find("stone") != std::string::npos)
				{
					Vector2 ScreenPos;
					Vector3 OrePos = get_obj_pos(unk3);

					if (WorldToScreen(OrePos, ScreenPos))
					{
						auto distance = Math::Calc3D_Dist(localPos, OrePos);
						if (distance < 300)
						{
							char buffer[0x100]{};
							sprintf(buffer, "Stone Col\n[%dm]", (int)distance);
							auto text = s2ws(buffer);
							DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, 0, 1, 0);
						}
					}
				}
			}
		}

		if (settings::dropped::enabled)
		{
			for (Item item : DroppedItemList)
			{
				Vector3 Pos = item.GetVisualPosition();
				//std::cout << "Getting Item amount" << std::endl;
				auto amount = item.GetAmount();
				//std::cout << "Getting Item distance" << std::endl;
				auto distance = Math::Calc3D_Dist(localPos, Pos);
				//std::cout << "Distance: " << distance << std::endl;
				if (distance < settings::dropped::max_distance)
				{
					//std::cout << ws2s(item.GetItemName()) << " | " << amount << std::endl;
					Vector2 ScreenPos;
					if (WorldToScreen(Pos, ScreenPos))
					{
						char buffer[0x100]{};
						sprintf(buffer, "[%dm]\n[%d]", (int)distance, amount);
						DrawWString(item.GetItemName(), 15, ScreenPos.x, ScreenPos.y, .5, .5, .5);
						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y + 15, .5, .5, .5);
					}
				}
			}
		}

		if (false)
		{
			for (CrateClass crate : CrateList)
			{
				const auto unk1 = mex.Read<uintptr_t>(crate.Object + 0x10);

				if (!unk1)
					continue;

				const auto unk2 = mex.Read<uintptr_t>(unk1 + 0x30);

				if (!unk2)
					continue;

				const auto unk3 = mex.Read<uintptr_t>(unk2 + 0x30);

				if (!unk3)
					continue;

				/* shouldn't be needed, but in case */
				if (!crate.Object)
					continue;

				const auto objName = mex.Read<uintptr_t>(unk2 + 0x60);
				std::string name = read_ascii(objName, 64);

				if (name.find("crate_tools") != std::string::npos)
				{
					Vector2 ScreenPos;
					auto Pos = GetCurrentObjectPosition(crate.Object);
					auto distance = Math::Calc3D_Dist(localPos, Pos);
					if (distance < 100)
					{
						if (WorldToScreen(Pos, ScreenPos))
						{
							char buffer[0x100]{};
							sprintf(buffer, "Tools Crate\n[%dm]", (int)distance);
							auto text = s2ws(buffer);
							DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, 1, 0, 1);
						}
					}
				}

				else if (strstr(name.c_str(), "assets/bundled/prefabs/radtown/crate_normal.prefab"))
				{
					Vector2 ScreenPos;
					auto Pos = GetCurrentObjectPosition(crate.Object);
					auto distance = Math::Calc3D_Dist(localPos, Pos);
					if (distance < 100)
					{
						if (WorldToScreen(Pos, ScreenPos))
						{
							char buffer[0x100]{};
							sprintf(buffer, "Military Crate\n[%dm]", (int)distance);
							auto text = s2ws(buffer);
							DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, 0, 0, 0);
						}
					}
				}

				else if (strstr(name.c_str(), "assets/bundled/prefabs/radtown/crate_normal_2.prefab"))
				{
					Vector2 ScreenPos;
					auto Pos = GetCurrentObjectPosition(crate.Object);
					auto distance = Math::Calc3D_Dist(localPos, Pos);
					if (distance < 100)
					{
						if (WorldToScreen(Pos, ScreenPos))
						{
							char buffer[0x100]{};
							sprintf(buffer, "Normal Crate\n[%dm]", (int)distance);
							auto text = s2ws(buffer);
							DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, 0, 0, 0);
						}
					}
				}

				else if (name.find("crate_elite") != std::string::npos)
				{
					Vector2 ScreenPos;
					auto Pos = GetCurrentObjectPosition(crate.Object);
					auto distance = Math::Calc3D_Dist(localPos, Pos);
					if (distance < 100)
					{
						if (WorldToScreen(Pos, ScreenPos))
						{
							char buffer[0x100]{};
							sprintf(buffer, "Elite Crate\n[%dm]", (int)distance);
							auto text = s2ws(buffer);
							DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, 0, 0, 0);
						}
					}
				}

				else if (name.find("barrel") != std::string::npos)
				{
					Vector2 ScreenPos;
					auto Pos = GetCurrentObjectPosition(crate.Object);
					auto distance = Math::Calc3D_Dist(localPos, Pos);
					if (distance < 100)
					{
						if (WorldToScreen(Pos, ScreenPos))
						{
							char buffer[0x100]{};
							sprintf(buffer, "Barrel\n[%dm]", (int)distance);
							auto text = s2ws(buffer);
							DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, 1, 1, 1);
						}
					}
				}
			}
		}
	}

	try
	{
		if (settings::aimbot::enabled)
		{
			if (closestPlayer.Player != NULL && LocalPlayer.Player != NULL)
			{
				if ((GetKeyState(settings::aimbot::aim_key) & 0x8000))
				{
					Vector2 ScreenPos;
					auto Pos = closestPlayer.GetBonePosition(neck); //GetBonePosition(closestPlayer, neck);
					auto distance = Math::Calc3D_Dist(localPos, Pos);
					if (distance < 300)
					{
						if (WorldToScreen(Pos, ScreenPos))
						{
							auto fov = distance_cursor(ScreenPos);
							if (fov < settings::aimbot::fov)
							{
								if (settings::aimbot::use_mouse)
								{
									Vector2 target = smooth(ScreenPos);
									INPUT input;
									input.type = INPUT_MOUSE;
									input.mi.mouseData = 0;
									input.mi.time = 0;
									input.mi.dx = target.x;
									input.mi.dy = target.y;
									input.mi.dwFlags = MOUSEEVENTF_MOVE;
									SendInput(1, &input, sizeof(input));
								}
								else
								{
									auto OrigVA = LocalPlayer.GetPlayerInput().GetViewAngles();
									Vector3 LocalPos = LocalPlayer.GetBonePosition(neck); // GetBonePosition(LocalPlayer, neck);
									//Vector3 EnemyPos = Prediction(LocalPos, closestPlayer, neck);
									auto RecAng = LocalPlayer.GetPlayerInput().GetRecoilAngle(); //GetRA(LocalPlayer);
									Vector2 Offset = Math::CalcAngle(LocalPos, Pos) - LocalPlayer.GetPlayerInput().GetViewAngles();
									//printf("Offset VA: %f | %f\n", Offset.x, Offset.y);
									//Normalize(Offset.y, Offset.x);
									Vector2 AngleToAim = LocalPlayer.GetPlayerInput().GetViewAngles() + Offset;
									AngleToAim = AngleToAim - RecAng;
									Normalize(AngleToAim.y, AngleToAim.x);
									LocalPlayer.GetPlayerInput().SetViewAngles(AngleToAim); // SetVA(LocalPlayer, AngleToAim);
								}
							}
						}
					}
				}
				else if ((GetKeyState(VK_LBUTTON) & 0x8000))
				{
					//std::cout << "PSilent Key Pressed!" << std::endl;
					if (settings::aimbot::silent)
					{
						Vector2 ScreenPos;
						auto Pos = closestPlayer.GetBonePosition(neck); //GetBonePosition(closestPlayer, neck);
						auto distance = Math::Calc3D_Dist(localPos, Pos);
						if (distance < 300)
						{
							//std::cout << "distance < 300!" << std::endl;
							if (WorldToScreen(Pos, ScreenPos))
							{
								//std::cout << "WorldToScreen(Pos, ScreenPos)!" << std::endl;
								auto OrigVA = LocalPlayer.GetPlayerInput().GetViewAngles();
								Vector3 LocalPos = LocalPlayer.GetBonePosition(neck); // GetBonePosition(LocalPlayer, neck);
								//Vector3 EnemyPos = Prediction(LocalPos, closestPlayer, neck);
								auto RecAng = LocalPlayer.GetPlayerInput().GetRecoilAngle(); //GetRA(LocalPlayer);
								Vector2 Offset = Math::CalcAngle(LocalPos, Pos) - LocalPlayer.GetPlayerInput().GetViewAngles();
								//printf("Offset VA: %f | %f\n", Offset.x, Offset.y);
								//Normalize(Offset.y, Offset.x);
								Vector2 AngleToAim = LocalPlayer.GetPlayerInput().GetViewAngles() + Offset;
								AngleToAim = AngleToAim - RecAng;
								Normalize(AngleToAim.y, AngleToAim.x);
								LocalPlayer.GetPlayerInput().SetViewAngles(AngleToAim); // SetVA(LocalPlayer, AngleToAim);
								LocalPlayer.GetPlayerInput().SetViewAngles(OrigVA);
								//std::cout << "PSilent Done!" << std::endl;
							}
						}
					}
				}
			}
		}
	}
	catch (...)
	{
		std::cout << "Aimbot Error!" << std::endl;
	}
}

void startoverlay()
{
	std::cout << "Getting Game Window!" << std::endl;
	const auto game_window = FindWindowW(L"UnityWndClass", nullptr);
	std::cout << "Setting Overlay Options!" << std::endl;
	DirectOverlaySetOption(D2DOV_DRAW_FPS | D2DOV_FONT_ARIAL);
	std::cout << "Creating Overlay!" << std::endl;
	DirectOverlaySetup(drawLoop, game_window);
	std::cout << "Initializing Menu!" << std::endl;
	menu.Initialize(L"Elemental Rust");
	std::cout << "Creating Player ESP Menu!" << std::endl;
	MenuTab PlayerESPTab(L"Player ESP");
	PlayerESPTab.AddItem(MenuItem(L"Enabled", &settings::player::enabled));
	PlayerESPTab.AddItem(MenuItem(L"Show Names", &settings::player::name));
	PlayerESPTab.AddItem(MenuItem(L"Show Health", &settings::player::health));
	PlayerESPTab.AddItem(MenuItem(L"Show Distance", &settings::player::distance));
	PlayerESPTab.AddItem(MenuItem(L"Show Held Item", &settings::player::held_item));
	PlayerESPTab.AddItem(MenuItem(L"Show Hotbar", &settings::player::hotbar));
	PlayerESPTab.AddItem(MenuItem(L"Max Distance", &settings::player::max_distance, 1, 1000));

	std::cout << "Creating Ore ESP Menu!" << std::endl;
	MenuTab OreESPTab(L"Ore ESP");
	OreESPTab.AddItem(MenuItem(L"Enabled", &settings::ore::enabled));
	OreESPTab.AddItem(MenuItem(L"Show Names", &settings::ore::name));
	OreESPTab.AddItem(MenuItem(L"Show Distance", &settings::ore::distance));
	OreESPTab.AddItem(MenuItem(L"Max Distance", &settings::ore::max_distance, 1, 1000));

	std::cout << "Creating Collectable ESP Menu!" << std::endl;
	MenuTab CollectableESPTab(L"Collectable ESP");
	CollectableESPTab.AddItem(MenuItem(L"Enabled", &settings::collectable::enabled));
	CollectableESPTab.AddItem(MenuItem(L"Show Names", &settings::collectable::name));
	CollectableESPTab.AddItem(MenuItem(L"Show Distance", &settings::collectable::distance));
	CollectableESPTab.AddItem(MenuItem(L"Max Distance", &settings::collectable::max_distance, 1, 1000));

	std::cout << "Creating Dropped ESP Menu!" << std::endl;
	MenuTab DroppedESPTab(L"Dropped ESP");
	DroppedESPTab.AddItem(MenuItem(L"Enabled", &settings::dropped::enabled));
	DroppedESPTab.AddItem(MenuItem(L"Show Names", &settings::dropped::name));
	DroppedESPTab.AddItem(MenuItem(L"Show Distance", &settings::dropped::distance));
	DroppedESPTab.AddItem(MenuItem(L"Max Distance", &settings::dropped::max_distance, 1, 1000));

	std::cout << "Creating Aimbot Menu!" << std::endl;
	MenuTab aimtab(L"Aimbot");
	aimtab.AddItem(MenuItem(L"Aimbot Enabled", &settings::aimbot::enabled));
	aimtab.AddItem(MenuItem(L"PSilent Enabled (Glitchy)", &settings::aimbot::silent));
	aimtab.AddItem(MenuItem(L"Use Mouse", &settings::aimbot::use_mouse));
	aimtab.AddItem(MenuItem(L"FOV", &settings::aimbot::fov, 10, 500, 5)); //settings::aimbot::smooth
	aimtab.AddItem(MenuItem(L"Smooth", &settings::aimbot::smooth, 1, 100, 1)); //settings::aimbot::smooth
	aimtab.AddItem(MenuItem(L"Aim Key", &settings::aimbot::aim_key, 1, 250));
	aimtab.AddItem(MenuItem(L"PSilent Key", &settings::aimbot::silent_key, 1, 250));
	aimtab.AddItem(MenuItem(L"Max Distance", &settings::aimbot::max_distance, 10, 1000));

	std::cout << "Creating Misc Menu!" << std::endl;
	MenuTab misctab(L"Misc");
	misctab.AddItem(MenuItem(L"Spider Enabled", &settings::misc::spider));
	misctab.AddItem(MenuItem(L"DebugCamera Enabled", &settings::misc::debug_cam));
	misctab.AddItem(MenuItem(L"No Recoil Enabled", &settings::misc::no_recoil));
	misctab.AddItem(MenuItem(L"All Automatic Enabled", &settings::misc::automatic));
	misctab.AddItem(MenuItem(L"Fat Bullet Enabled", &settings::misc::fat_bullet));
	misctab.AddItem(MenuItem(L"Always Day Enabled", &settings::misc::always_day));


	std::cout << "Adding Tabs to Menu!" << std::endl;
	menu.AddTab(PlayerESPTab);
	menu.AddTab(OreESPTab);
	menu.AddTab(CollectableESPTab);
	menu.AddTab(DroppedESPTab);
	menu.AddTab(aimtab);
	menu.AddTab(misctab);
	std::cout << std::endl << std::endl << "\tControls:" << std::endl;
	std::cout << "\t\t[Insert]\tShow/Hide Menu" << std::endl;
	std::cout << "\t\t[UP/DOWN]\tNavigate Menu Up/Down" << std::endl;
	std::cout << "\t\t[LEFT/RIGHT]\tChange Selected Menu Item" << std::endl;
	std::cout << "\t\t[END]\t\tSwitch Menu Tabs" << std::endl;
	std::cout << "Credits to Colton for his menu!" << std::endl;
	getchar();
}

void InjectIntoRust(int Id)
{
	hProcess = 0;
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, Id);
	if (!hProcess)
	{
		std::cout << "[Aspect] OpenProcess failed with errorcode " << GetLastError() << std::endl;
		ErrorExit("OpenProcess");
	}
	auto Dll = new ManualMap::WDLL();
	Dll->IsDLL = true;
	Dll->hThread = OpenThread(THREAD_ALL_ACCESS, false, GetThreadId(Id));
	//std::cout << "DLL->hThread: " << Dll->hThread << std::endl;
	if (!ManualMap::LoadFileA(hProcess, dllpath.c_str(), ManualMap::HIJACK_THREAD | ManualMap::CALL_EXPORT, Dll))
	{
		Sleep(1000);
		InjectIntoRust(Id);
	}
	delete Dll;
}

void InjectIntoRust2(int Id)
{
	hProcess = 0;
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, Id);
	if (!hProcess)
	{
		std::cout << "[Aspect] OpenProcess failed with errorcode " << GetLastError() << std::endl;
		ErrorExit("OpenProcess");
	}
	auto Dll = new ManualMap::WDLL();
	Dll->IsDLL = true;
	Dll->hThread = OpenThread(THREAD_ALL_ACCESS, false, GetThreadId(Id));
	//std::cout << "DLL->hThread: " << Dll->hThread << std::endl;
	if (!ManualMap::LoadFileA(hProcess, dllpath2.c_str(), ManualMap::HIJACK_THREAD | ManualMap::CALL_EXPORT, Dll))
	{
		Sleep(1000);
		InjectIntoRust(Id);
	}
	delete Dll;
}

int WINAPI main(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	std::string title = "Elemental Rust | Version: " + sec.version;
	SetConsoleTitle(title.c_str());
	if (false)
	{
		std::cout << "[Elemental] Loader Version Outdated!" << std::endl;
		std::cout << "[Elemental] Please go to https://aspectnetwork.net/aspect/rust/download to get the latest download!" << std::endl;;
		Sleep(10000);
	}
	else
	{
		std::cout << "Enter Username:" << std::endl;
		std::string username;
		std::cin >> username;
		std::cout << "Enter Password:" << std::endl;
		std::string password;
		std::cin >> password;
		
		if (sec.authenticate(username, password))
		{

			system("CLS");
			if (Memory::CleanPIDDBCacheTable())
			{
				std::cout << "[Elemental] Table Cleaned!" << std::endl;
			}
			else
			{
				std::cout << "[Elemental] Failed to clean table." << std::endl;
			}
			std::cout << "[Elemental] Getting Files!" << std::endl;
			if (false)
			{
				if (CreateDirectory(folderpath.c_str(), NULL) ||
					ERROR_ALREADY_EXISTS == GetLastError())
				{
					DeleteUrlCacheEntry(dllurl.c_str());
					HRESULT hr = URLDownloadToFile(
						NULL,   // A pointer to the controlling IUnknown interface (not needed here)
						dllurl.c_str(),
						dllpath.c_str(),
						0,      // Reserved. Must be set to 0.
						NULL); // status callback interface (not needed for basic use)
					if (!SUCCEEDED(hr))
					{
						std::cout << "[Elemental] An error occured during file update #1!" << std::endl << "[Elemental] Error code = 0x" << std::hex << hr << std::endl;
					}
					else
					{
						if (CreateDirectory(folderpath.c_str(), NULL) ||
							ERROR_ALREADY_EXISTS == GetLastError())
						{
							DeleteUrlCacheEntry(driverurl.c_str());
							HRESULT hr = URLDownloadToFile(
								NULL,   // A pointer to the controlling IUnknown interface (not needed here)
								driverurl.c_str(),
								driverpath.c_str(),
								0,      // Reserved. Must be set to 0.
								NULL); // status callback interface (not needed for basic use)
							if (!SUCCEEDED(hr))
							{
								std::cout << "[Elemental] An error occured during file update #2!" << std::endl << "[Elemental] Error code = 0x" << std::hex << hr << std::endl;
							}
							else
							{
								if (CreateDirectory(folderpath.c_str(), NULL) ||
									ERROR_ALREADY_EXISTS == GetLastError())
								{
									DeleteUrlCacheEntry(dllurl2.c_str());
									HRESULT hr = URLDownloadToFile(
										NULL,   // A pointer to the controlling IUnknown interface (not needed here)
										dllurl2.c_str(),
										dllpath2.c_str(),
										0,      // Reserved. Must be set to 0.
										NULL); // status callback interface (not needed for basic use)
									if (!SUCCEEDED(hr))
									{
										std::cout << "[Elemental] Driver already running!" << std::endl;
									}
									else
									{
										std::cout << "[Elemental] Got Files!" << std::endl;
									}
								}
							}
						}
					}
				}
			}
			auto IsRustOpen = FindWindowW(L"UnityWndClass", nullptr);
			bool DidWeInject = false;
			if (IsRustOpen)
			{
				InjectTheDll = false;
			}
			if (true)
			{
				if (true)
				{
					if (true)
						std::cout << "[Elemental] Waiting For Game." << std::endl;
					else
						std::cout << "[Elemental] Could not protect process." << std::endl;

					std::map<std::uint32_t, std::uint8_t> UsedProcessIds;
					while (true)
					{
						//Sleep(250);
						auto ProcIds = GetProcessIds(std::wstring(L"RustClient.exe"));
						for (auto Id : ProcIds)
						{
							if (DidWeInject)
							{
								if (GetAsyncKeyState(VK_INSERT) & 1)
								{
									if (!overlaycreated)
									{
										std::cout << "Creating Player Thread!" << std::endl;
										std::thread PlayerThread(PlayerThreadFunc);
										PlayerThread.detach();
										std::cout << "Creating GameHax Thread!" << std::endl;
										std::thread GameHax(DoGameHax);
										GameHax.detach();
										std::cout << "Creating FBT Thread!" << std::endl;
										std::thread FBT(FatBulletThread);
										FBT.detach();
										std::thread FBT2(FatBulletThread);
										FBT2.detach();
										std::cout << "Creating startoverlay Thread!" << std::endl;
										startoverlay();
										std::cout << "Done!" << std::endl;
										overlaycreated = true;
									}
									else
										MenuOpen = !MenuOpen;
								}
								
								
								if (GetAsyncKeyState(VK_NUMPAD0) & 1)
								{
									std::cout << "Injecting Haxx 2" << std::endl;
									InjectIntoRust2(Id);
									std::cout << "Injected Haxx 2" << std::endl;
								}
								
								continue;
							}
							std::cout << "[Elemental] Loading!" << std::endl;
							mex.Open("RustClient.exe");
							std::cout << "[Elemental] Proc ID: " << Memory::process_id << std::endl;
							std::cout << "[Elemental] Loaded!" << std::endl;
							std::cout << "[Elemental] Press Insert AFTER You Join A Server." << std::endl;
							DidWeInject = true;
							//break;
							UsedProcessIds[Id] = 1;
							//delete CheatDll;
						}
					}

					/* When you close your hack, please call those two functions:
						Bypass::Driver::CloseDriver();
						Bypass::Installer::UninstallService("Amdkfca");  */

				}
				else
					std::cout << "OpenDriver failed." << std::endl;

			}
			else
				std::cout << "InstallService failed." << std::endl;
			Sleep(5000);
		}
		else
		{
			system("CLS");
			std::cout << "Wrong username/password.\nPlease relaunch and try again!" << std::endl;
			Sleep(10000);
		}
	}
	return 1;
}

std::vector<std::uint32_t> GetProcessIds(const std::wstring& processName)
{
	std::vector<std::uint32_t> procs;
	PROCESSENTRY32W processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Snapshot == INVALID_HANDLE_VALUE) return procs;

	Process32FirstW(Snapshot, &processInfo);
	if (wcsstr(processName.c_str(), processInfo.szExeFile))
	{
		CloseHandle(Snapshot);
		procs.push_back(processInfo.th32ProcessID);
		return procs;
	}

	while (Process32NextW(Snapshot, &processInfo))
	{
		if (wcsstr(processName.c_str(), processInfo.szExeFile))
		{
			procs.push_back(processInfo.th32ProcessID);
		}
	}

	CloseHandle(Snapshot);
	return procs;
};

DWORD GetThreadId(DWORD dwProcessId)
{
	THREADENTRY32 threadinfo;
	threadinfo.dwSize = sizeof(threadinfo);

	HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessId);
	if (Snapshot == INVALID_HANDLE_VALUE) return false;

	Thread32First(Snapshot, &threadinfo);

	while (Thread32Next(Snapshot, &threadinfo))
	{
		if (threadinfo.th32ThreadID && threadinfo.th32OwnerProcessID == dwProcessId)
		{
			CloseHandle(Snapshot);
			return threadinfo.th32ThreadID;
		}
	}
	CloseHandle(Snapshot);
	return 0;
};

HWND WINAPI InitializeWin(HINSTANCE hInst)
{
	WNDCLASSEX wndClass;

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbClsExtra = NULL;
	wndClass.cbWndExtra = NULL;
	wndClass.hCursor = LoadCursor(0, IDC_ARROW);
	wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	wndClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	wndClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	wndClass.hInstance = hInst;
	wndClass.lpfnWndProc = MyWindowProc;
	wndClass.lpszClassName = WINNAME;
	wndClass.lpszMenuName = WINNAME;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wndClass))
	{
		exit(1);
	}

	ThisWindow = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, WINNAME, WINNAME, WS_POPUP, 1, 1, ScreenWidth, ScreenHeight, 0, 0, 0, 0);
	SetLayeredWindowAttributes(ThisWindow, 0, 0.1f, LWA_ALPHA);
	SetLayeredWindowAttributes(ThisWindow, RGB(0, 0, 0), 0, ULW_COLORKEY);
	MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea(ThisWindow, &margins);
	D3DInitialize(ThisWindow);

	return ThisWindow;
}

void UpdateWinPosition()
{
	while (true)
	{
		UpdateSurface(ThisWindow);
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	return;
}

void WINAPI UpdateSurface(HWND hWnd)
{
	RECT wSize;
	HWND tWnd;

	tWnd = FindWindow(NULL, "UnityWndClass");

	if (tWnd)
	{
		GetWindowRect(tWnd, &wSize);
		ScreenHeight = wSize.right - wSize.left;
		ScreenHeight = wSize.bottom - wSize.top;

		DWORD dwStyle = GetWindowLong(tWnd, GWL_STYLE);
		if (dwStyle & WS_BORDER)
		{
			wSize.top += 23; ScreenHeight -= 23;
			//wSize.left += 10; rWidth -= 10;
		}

		if (hWnd)
		{
			MoveWindow(hWnd, wSize.left, wSize.top, ScreenWidth, ScreenHeight, true);
		}
	}

	return;
}

LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage) {
	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &MARGIN);
		break;

	case WM_PAINT:
		D3DRender();
		break;

	case WM_DESTROY:
		PostQuitMessage(1);
		break;

	default:
		ImGui_ImplWin32_WndProcHandler(hWnd, uMessage, wParam, lParam);
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
		break;
	}

	return 0;
}