

//#include "framework.h"
//#include "GamaOverlay.h"
//#include "Paint.h"
#include "../UserMode/entityPosition.h"
#include "Memory.h"
#include "Structs.h"
#include "Drawing.h"
#include "Offsets.h"
#include <iostream>
#include <thread>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <chrono>
#include <fstream>
#include "auth.hpp"
#include <string>
#include "skStr.h"
#include <limits>
#include <Windows.h>

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);

using namespace KeyAuth;

std::string name = "Gama"; // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = "mE8DY3kCev"; // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = "273a0cbd35987b36558a5d45b7da55e3cc9a9422a9eda93fb6774dd00ffff6bb"; // app secret, the blurred text on licenses tab and other tabs
std::string version = "1.0"; // leave alone unless you've changed version on website
std::string url = "https://keyauth.win/api/1.1/"; // change if you're self-hosting
std::string sslPin = "ssl pin key (optional)"; // don't change unless you intend to pin public certificate key. you can get here in the "Pin SHA256" field https://www.ssllabs.com/ssltest/analyze.html?d=keyauth.win&latest. If you do this you need to be aware of when SSL key expires so you can update it

/*
	Video on what ownerid and secret are https://youtu.be/uJ0Umy_C6Fg

	Video on how to add KeyAuth to your own application https://youtu.be/GB4XW_TsHqA

	Video to use Web Loader (control loader from customer panel) https://youtu.be/9-qgmsUUCK4
*/

api KeyAuthApp(name, ownerid, secret, version, url, sslPin);


float EntFeetX;
float EntFeetY;

int ScreenWidth = 1920;
int ScreenHeight = 1080;



// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;


// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



static D3D11_BLEND_DESC desc = {};
ID3D11BlendState* g_pBlendState;



#define ABS(x) ((x < 0)? (-x) : (x))
Vector2 TopLeftFOV = { 750,350 };
Vector2 TopRightFOV = { 1150, 350 };
Vector2 BottomLeftFOV = { 750, 750 };
Vector2 BottomRightFOV = { 1150 , 750 };


bool Aimbot = true;//0 false 1 true
bool GlowEsp = true;
bool SnapLines = true;
bool BoxEsp = true;
bool RainbowCircle = true;
bool HealthSheildBars = true;
bool ClosestEnemyInCircle = true;
bool ClosestEnemyToCenterOfScreen = false;

static float Lock = 0.908f;//.935 good
static float Speed = 370.f;
static float CircleRadius = 170.f;
static int SaveYourCPU = 60;
static float TeamGlowEspColor[3] = { 0.0f, 255.f, 0.0f };
static float EnemyGlowEspColor[3] = { 255.0f, 0.0f, 0.0f };
static int BonePos = 6;

static float DistanceFromEnemy = 5000.f;
static float LineThickness = 1.f;
float VisibleVar;

#define MAX_NAME_LEN 60

inline const char* const BoolToString(bool b)
{
	return b ? "true" : "false";
}



void ItemEsp() {
	uintptr_t LocalPlayer = Read<uintptr_t>(base_address + Offsets::LocalPlayer);
	while (true) {
		for (int i = 0; i < 1000; i++) {

			//DWORD64 Entity = GetEntityById(i, base_address);
			uintptr_t Entity = Read<uintptr_t>(base_address + Offsets::EntityList + (i << 5));
			if (Entity == NULL)
				continue;

			int itemid = Read<int>(Entity + 0x16b8);
			if (itemid == NULL)
				continue;
			cout << itemid << endl;



			Vector3 Origin = Read<Vector3>(Entity + Offsets::Origin);



			Vector3 LocalOrigin = Read<Vector3>(LocalPlayer + Offsets::Origin);
			Vector3 deltaVec = { Origin.x - LocalOrigin.x,Origin.y - LocalOrigin.y, Origin.z - LocalOrigin.z };
			float deltaSQRT = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);
			float DistanceFromCurrentEnemy = deltaSQRT;
			//draw->AddLine(ImVec2(windowWidth / 2, windowHeight), ImVec2(W2SItem.x, W2SItem.y), IM_COL32(0, 0, 255, 255), LineThickness);



			if (DistanceFromCurrentEnemy > 10000) {
				continue;
			}
			auto draw = ImGui::GetBackgroundDrawList();
			uint64_t viewRenderer = Read<uint64_t>(base_address + Offsets::ViewRender);
			uint64_t viewMatrix = Read<uint64_t>(viewRenderer + Offsets::ViewMatrix);
			Matrix m = Read<Matrix>(viewMatrix);
			Vector3 W2SItem = _WorldToScreen(Origin, m);


			if (itemid == 41 || itemid == 77 || itemid == 109 || itemid == 192 || itemid == 62) //Item ID to shine
			{
				draw->AddLine(ImVec2(windowWidth / 2, windowHeight), ImVec2(W2SItem.x, W2SItem.y), IM_COL32(0, 0, 255, 255), LineThickness);

				

				Write<int>(Entity + 0x380, 1);
				Write<float>(Entity + 0x2EC, 0); //R
				Write<float>(Entity + 0x2C4, 255);//G
				Write<float>(Entity + 0x1A8, 0);//B


				Write<int>(Entity + 0x3C8, 1);
				Write<int>(Entity + 0x3D0, 2);
				Write<GlowMode>(Entity + 0x2c0, { 101,127,46,90 });
				Write<float>(Entity + 0x1D0, 0); //R
				Write<float>(Entity + 0x1D4, 255);//G
				Write<float>(Entity + 0x1D8, 0);//B
				/*
				Write<int>(Entity + 0x3C8, 1);
				Write<int>(Entity + 0x3D0, 2);
				Write<GlowMode>(Entity + 0x2c0, { 101,102,46,96 });
				Write<float>(Entity + 0x1D0, 0.f);
				Write<float>(Entity + 0x1D4, 122.f);
				Write<float>(Entity + 0x1D8, 122.f);
				*/
			}
		}
	}
}


int Menu() {
	WNDCLASSEX wc1 = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Login"), NULL };
	::RegisterClassEx(&wc1);
	//HWND hwnd1 = ::CreateWindow(wc1.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 0, 0, 500, 500, NULL, NULL, wc1.hInstance, NULL);
	HWND hwnd1 = CreateWindowEx(NULL, (wc1.lpszClassName), "Login", WS_POPUP, windowWidth / 2, windowHeight / 3, 300, 150, 0, 0, 0, 0);


	if (!CreateDeviceD3D(hwnd1))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc1.lpszClassName, wc1.hInstance);
		return 1;
	}


	::ShowWindow(hwnd1, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd1);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io1 = ImGui::GetIO(); (void)io1;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd1);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


	bool show_demo_window1 = true;
	bool show_another_window1 = false;
	bool Login = true;
	ImVec4 clear_color1 = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);//10,10,10 goodish

	// Main loop
	bool done1 = false;
	char username1[50] = "Username";
	char password1[50] = "Password";
	int tab = 0;
	const char* Status = "";
	while (!done1) {



		MSG msg1;
		while (::PeekMessage(&msg1, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg1);
			::DispatchMessage(&msg1);
			if (msg1.message == WM_QUIT)
				done1 = true;
		}
		if (done1)
			break;



		if (GetAsyncKeyState(VK_RBUTTON) && GetActiveWindow() == hwnd1) {
			POINT p;
			GetCursorPos(&p);
			MoveWindow(hwnd1, p.x, p.y, 300, 150, true);

		}
		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::GetStyle().Colors[ImGuiCol_Border] = ImColor(0, 255, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Tab] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabActive] = ImColor(0, 190, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabHovered] = ImColor(0, 65, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImColor(0, 65, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImColor(0, 190, 0, 255);


		auto draw1 = ImGui::GetBackgroundDrawList();

		size_t size = 20;


		ImGui::Begin("Window Name", &Login, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2(10, 10));
		ImGui::SetWindowSize(ImVec2(280, 130));

		if (tab == 0) {
			ImGui::InputText("Username", username1, size);
			ImGui::InputText("Password", password1, size);
			if (ImGui::Button("Login")) {
				KeyAuthApp.login(username1, password1);

				if (!KeyAuthApp.data.success)
				{
					std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
					Sleep(1500);
					exit(0);

				}
				KeyAuthApp.check();
				Sleep(1000);
				done1 = true;
			}


		}

		ImGui::End();

		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color1.x * clear_color1.w, clear_color1.y * clear_color1.w, clear_color1.z * clear_color1.w, clear_color1.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();

	ShowWindow(hwnd1, SW_HIDE);
}

// Main code
int main(int, char**)
{
	SetConsoleTitleA(skCrypt("Loader"));
	
	
	std::cout << skCrypt("\n\n Connecting..");
	KeyAuthApp.init();
	if (!KeyAuthApp.data.success)
	{
		std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
		Sleep(1500);
		exit(0);
	}

	//************************************************************************************************************************************************
	//************************************************************************************************************************************************
	//************************************************************************************************************************************************
	//*************************************************Login******************************************************************************************
	//************************************************************************************************************************************************
	//************************************************************************************************************************************************



	WNDCLASSEX wc1 = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Login"), NULL };
	::RegisterClassEx(&wc1);
	//HWND hwnd1 = ::CreateWindow(wc1.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 0, 0, 500, 500, NULL, NULL, wc1.hInstance, NULL);
	HWND hwnd1 = CreateWindowEx(NULL, (wc1.lpszClassName), "Login", WS_POPUP, windowWidth / 2, windowHeight/ 3, 300, 150, 0, 0, 0, 0);
	
	
	if (!CreateDeviceD3D(hwnd1))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc1.lpszClassName, wc1.hInstance);
		return 1;
	}

	
	::ShowWindow(hwnd1, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd1);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io1 = ImGui::GetIO(); (void)io1;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd1);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


	bool show_demo_window1 = true;
	bool show_another_window1 = false;
	bool Login = true;
	ImVec4 clear_color1 = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);//10,10,10 goodish

	// Main loop
	bool done1 = false;
	char username1[50] = "Username";
	char password1[50] = "Password";
	int tab = 0;
	const char* Status = "";
	while (!done1){
		
		

		MSG msg1;
		while (::PeekMessage(&msg1, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg1);
			::DispatchMessage(&msg1);
			if (msg1.message == WM_QUIT)
				done1 = true;
		}
		if (done1)
			break;
		
		

		if (GetAsyncKeyState(VK_RBUTTON) && GetActiveWindow() == hwnd1) {
			POINT p;
			GetCursorPos(&p);
			MoveWindow(hwnd1, p.x, p.y, 300, 150, true);
			
		}
		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::GetStyle().Colors[ImGuiCol_Border] = ImColor(0, 255, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Tab] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabActive] = ImColor(0, 190, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabHovered] = ImColor(0, 65, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImColor(0, 65, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImColor(0, 190, 0, 255);

		
		
		


		auto draw1 = ImGui::GetBackgroundDrawList();
		// 
		//auto draw = ImGui::GetForegroundDrawList();

		///ImGui::Render();

		//draw1->AddRectFilled(ImVec2(50, 50), ImVec2(100, 100), IM_COL32(255, 255, 255, 255), 5.f);

		
		size_t size = 20;
		

		ImGui::Begin("Window Name", &Login, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2(10,10));
		ImGui::SetWindowSize(ImVec2(280, 130));

		//if (ImGui::BeginTabBar("sad")) {
		//	if (ImGui::BeginTabItem("Login")) {

				//ImGui::Text("Login");
		//draw1->AddLine(ImVec2(20, 75), ImVec2(730, 30), IM_COL32(0, 255, 0, 255), 3.f);
		//if (ImGui::Button("Login",(ImVec2(350,20)))) {
		//	tab = 0;
		//}
		//ImGui::SameLine();
		//if (ImGui::Button("Register",(ImVec2(350, 20)))) {
		//	tab = 1;
		//}
		//if (ImGui::Button("Login1")) {
		//	system("kdmapper.exe Driver.sys");
		//}
		
		if (tab == 0) {
			ImGui::InputText("Username", username1, size);
			ImGui::InputText("Password", password1, size);
			if (ImGui::Button("Login")) {
				KeyAuthApp.login(username1, password1);

				if (!KeyAuthApp.data.success)
				{
					std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
					Sleep(1500);
					exit(0);

				}
				KeyAuthApp.check();
				Sleep(1000);
				done1 = true;
			}
			
			
		}
				
		//	}
		//}

		ImGui::End();
		

		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color1.x * clear_color1.w, clear_color1.y * clear_color1.w, clear_color1.z * clear_color1.w, clear_color1.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	//::DestroyWindow(hwnd1);
	//::UnregisterClass(wc1.lpszClassName, wc1.hInstance);
	//::CloseWindow(hwnd1);
	
	//return 0;


	
	ShowWindow(hwnd1, SW_HIDE);
	//SendMessage(hwnd1, WM_CLOSE, 0, NULL);



	//************************************************************************************************************************************************
	//************************************************************************************************************************************************
	//************************************************************************************************************************************************
	//*************************************************Login End**************************************************************************************
	//************************************************************************************************************************************************
	//************************************************************************************************************************************************


	
	
	WNDCLASSEX wc11 = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Login"), NULL };
	::RegisterClassEx(&wc11);
	//HWND hwnd1 = ::CreateWindow(wc1.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 0, 0, 500, 500, NULL, NULL, wc1.hInstance, NULL);
	HWND hwnd11 = CreateWindowEx(NULL, (wc11.lpszClassName), "Login", WS_POPUP, windowWidth / 2, windowHeight / 3, 300, 150, 0, 0, 0, 0);


	if (!CreateDeviceD3D(hwnd11))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc11.lpszClassName, wc11.hInstance);
		return 1;
	}


	::ShowWindow(hwnd11, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd11);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io11 = ImGui::GetIO(); (void)io11;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd11);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


	ImVec4 clear_color11 = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);//10,10,10 goodish

	// Main loop
	bool done11 = false;
	bool Main = true;

	while (!done11)
	{
		MSG msg11;
		while (::PeekMessage(&msg11, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg11);
			::DispatchMessage(&msg11);
			if (msg11.message == WM_QUIT)
				done11 = true;
		}
		if (done11)
			break;



		if (GetAsyncKeyState(VK_RBUTTON) && GetActiveWindow() == hwnd11) {
			POINT p1;
			GetCursorPos(&p1);
			MoveWindow(hwnd11, p1.x, p1.y, 300, 150, true);

		}
		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::GetStyle().Colors[ImGuiCol_Border] = ImColor(0, 255, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Tab] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabActive] = ImColor(0, 190, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabHovered] = ImColor(0, 65, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImColor(0, 65, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImColor(0, 190, 0, 255);


		auto draw11 = ImGui::GetBackgroundDrawList();

		size_t size1 = 20;


		ImGui::Begin("Window Name", &Main, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2(10, 10));
		ImGui::SetWindowSize(ImVec2(280, 130));

		if (ImGui::Button("Map Driver")) {
			system("kdmapper.exe Driver.sys");
			break;
		}
		if (ImGui::Button("Play")) {
			
			break;
		}

		ImGui::End();


		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha1[4] = { clear_color11.x * clear_color11.w, clear_color11.y * clear_color11.w, clear_color11.z * clear_color11.w, clear_color11.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha1);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	//::DestroyWindow(hwnd1);
	//::UnregisterClass(wc1.lpszClassName, wc1.hInstance);
	//::CloseWindow(hwnd1);

	//return 0;



	ShowWindow(hwnd11, SW_HIDE);
	

	//SendMessage(hwnd1, WM_CLOSE, 0, NULL);

	//std::thread (Menu);
	

	/*

	std::cout << skCrypt("\n\n App data:");
	//std::cout << skCrypt("\n Number of users: ") << KeyAuthApp.data.numUsers;
	//std::cout << skCrypt("\n Number of online users: ") << KeyAuthApp.data.numOnlineUsers;
	//std::cout << skCrypt("\n Number of keys: ") << KeyAuthApp.data.numKeys;
	std::cout << skCrypt("\n Application Version: ") << KeyAuthApp.data.version;
	//std::cout << skCrypt("\n Customer panel link: ") << KeyAuthApp.data.customerPanelLink;
	KeyAuthApp.check();
	std::cout << skCrypt("\n Current Session Validation Status: ") << KeyAuthApp.data.message;

	std::cout << skCrypt("\n\n [1] Login\n [2] Register\n [3] Upgrade\n [4] License key only\n\n Choose option: ");

	int option;
	std::string username;
	std::string password;
	std::string key;

	std::cin >> option;
	switch (option)
	{
	case 1:
		std::cout << skCrypt("\n\n Enter username: ");
		std::cin >> username;
		std::cout << skCrypt("\n Enter password: ");
		std::cin >> password;
		KeyAuthApp.login(username, password);
		break;
	case 2:
		std::cout << skCrypt("\n\n Enter username: ");
		std::cin >> username;
		std::cout << skCrypt("\n Enter password: ");
		std::cin >> password;
		std::cout << skCrypt("\n Enter license: ");
		std::cin >> key;
		KeyAuthApp.regstr(username, password, key);
		break;
	case 3:
		std::cout << skCrypt("\n\n Enter username: ");
		std::cin >> username;
		std::cout << skCrypt("\n Enter license: ");
		std::cin >> key;
		KeyAuthApp.upgrade(username, key);
		break;
	case 4:
		std::cout << skCrypt("\n Enter license: ");
		std::cin >> key;
		KeyAuthApp.license(key);
		break;
	default:
		std::cout << skCrypt("\n\n Status: Failure: Invalid Selection");
		Sleep(3000);
		exit(0);
	}

	if (!KeyAuthApp.data.success)
	{
		std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
		Sleep(1500);
		exit(0);
	}

	std::cout << skCrypt("\n User data:");
	std::cout << skCrypt("\n Username: ") << KeyAuthApp.data.username;
	//std::cout << skCrypt("\n IP address: ") << KeyAuthApp.data.ip;
	//std::cout << skCrypt("\n Hardware-Id: ") << KeyAuthApp.data.hwid;
	std::cout << skCrypt("\n Create date: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.data.createdate)));
	std::cout << skCrypt("\n Last login: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.data.lastlogin)));
	std::cout << skCrypt("\n Subscription name: ") << KeyAuthApp.data.subscription;
	std::cout << skCrypt("\n Subscription expiry: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.data.expiry)));

	KeyAuthApp.check();
	std::cout << skCrypt("\n Current Session Validation Status: ") << KeyAuthApp.data.message;
	*/
	/*
	KeyAuthApp.web_login();

	std::cout << "\n Waiting for button to be clicked";
	KeyAuthApp.button("close");
	*/


	/*
	// download file, change file.exe to whatever you want.
	// remember, certain paths like windows folder will require you to turn on auto run as admin https://stackoverflow.com/a/19617989

	std::vector<std::uint8_t> bytes = KeyAuthApp.download("167212");
	std::ofstream file("file.exe", std::ios_base::out | std::ios_base::binary);
	file.write((char*)bytes.data(), bytes.size());
	file.close();
	*/

	// KeyAuthApp.setvar("discord", "test#0001"); // set the variable 'discord' to 'test#0001'
	// std::cout << "\n\n User variable data: " + KeyAuthApp.getvar("discord"); // display the user variable witn name 'discord'

	// let's say you want to send request to https://keyauth.win/api/seller/?sellerkey=f43795eb89d6060b74cdfc56978155ef&type=black&ip=1.1.1.1&hwid=abc
	// but doing that from inside the loader is a bad idea as the link could get leaked.
	// Instead, you should create a webhook with the https://keyauth.win/api/seller/?sellerkey=f43795eb89d6060b74cdfc56978155ef part as the URL
	// then in your loader, put the rest of the link (the other paramaters) in your loader. And then it will send request from KeyAuth server and return response in string resp

	// you have to encode the & sign with %26
	// std::string resp = KeyAuthApp.webhook("P5NHesuZyf", "%26type=black%26ip=1.1.1.1%26hwid=abc");
	// std::cout << "\n Response recieved from webhook request: " + resp;

	// KeyAuthApp.log("user logged in"); // send event to logs. if you set discord webhook in app settings, it will send there too
	// KeyAuthApp.ban(); // ban the current user, must be logged in

	//std::cout << skCrypt("\n\n Closing in 5 seconds...");
	//Sleep(5000);
	// 
	//exit(0);














	
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("GamaClass"), NULL };
    ::RegisterClassEx(&wc);
    //HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_POPUP, 00, 00, 1920, 1080, NULL, NULL, wc.hInstance, NULL);


	HWND hwnd = CreateWindowEx(WS_EX_LAYERED, (wc.lpszClassName), "Gama Overlay", WS_POPUP, 0, 0, 1920, 1080, 0, 0, 0, 0);
	




	//HWND hwnd = CreateWindowEx(WS_EX, (wc.lpszClassName), "Gama Overlay", WS_POPUP, 0, 0, 1920, 1080, 0, 0, 0, 0);

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), RGB(255, 255, 255, ), LWA_COLORKEY);
    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    //SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED);

	/*
	if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {
		SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT); //WS_EX_TOPMOST | WS_EX_TRANSPARENT//off
	}
	else {
		SetWindowLong(hwnd, GWL_EXSTYLE, NULL);
	}
	*/
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	
    //SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);

    //SetLayeredWindowAttributes(hwnd, RGB(255, 255, 255), 100, LWA_ALPHA);
	


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    //ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // 
    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 20.0f);

    // Main loop
    bool done = false;

	
	//static int GeneralGlowMode = 101;
	//static int BorderGlowMode = 101;
	//static int BorderSize = 46;
	//static int TransparentLevel = 90;

    HDC hdc = GetDC(FindWindowA(NULL, "Apex Legends"));//Getting the Window to Draw Over

    process_id = get_process_id("r5apex.exe");//Getting the Process Id
    base_address = get_module_base_address("r5apex.exe");//Getting The Module Base

    cout << "Process id found at " << process_id << endl;//Printing the Process Id 
    cout << "Process Base Address found at 0x" << hex << base_address << endl;//Printing the Module Base 

    float ClosestEnt = 999999999999.f;
    int closest = -1;
    int enemyInFOV = -1;
	int ClosestEntToCenterOfScreen = 9999;
	int ClosestEntToCenter = -1;
	//std::thread first(Draw);Loop
	//std::thread first(Loop);
	

	std::ifstream GamaPresets("GamaPresets.txt");

	std::string AimbotStrRead;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, AimbotStrRead);
	}
	if (AimbotStrRead == "true")
		Aimbot = true;
	else if (AimbotStrRead == "false")
		Aimbot = false;

	std::string GlowEspString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, GlowEspString);
	}
	if (GlowEspString == "true")
		GlowEsp = true;
	else if (GlowEspString == "false")
		GlowEsp = false;

	std::string SnapLinesString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, SnapLinesString);
	}
	if (SnapLinesString == "true")
		SnapLines = true;
	else if (SnapLinesString == "false")
		SnapLines = false;

	std::string BoxEspString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, BoxEspString);
	}
	if (BoxEspString == "true")
		BoxEsp = true;
	else if (BoxEspString == "false")
		BoxEsp = false;

	std::string RainbowCircleString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, RainbowCircleString);
	}
	if (RainbowCircleString == "true")
		RainbowCircle = true;
	else if (RainbowCircleString == "false")
		RainbowCircle = false; 

		std::string HealthSheildBarsString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, HealthSheildBarsString);
	}
	if (HealthSheildBarsString == "true")
		HealthSheildBars = true;
	else if (HealthSheildBarsString == "false")
		HealthSheildBars = false;


	/*
	std::string ClosestEnemyInCircleString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, ClosestEnemyInCircleString);
	}
	if (ClosestEnemyInCircleString == "true")
		ClosestEnemyInCircle = true;
	else if (ClosestEnemyInCircleString == "false")
		ClosestEnemyInCircle = false;

	std::string ClosestEnemyToCenterOfScreenString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, ClosestEnemyToCenterOfScreenString);
	}
	if (ClosestEnemyToCenterOfScreenString == "true")
		ClosestEnemyToCenterOfScreen = true;
	else if (ClosestEnemyToCenterOfScreenString == "false")
		ClosestEnemyToCenterOfScreen = false;
		*/

	std::string LockString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, LockString);
	}
	Lock = std::stof(LockString);

	std::string SpeedString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, SpeedString);
	}
	Speed = std::stof(SpeedString);

	std::string CircleRadiusString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, CircleRadiusString);
	}
	CircleRadius = std::stof(CircleRadiusString);

	std::string DistanceFromEnemyString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, DistanceFromEnemyString);
	}
	DistanceFromEnemy = std::stof(DistanceFromEnemyString);

	std::string LineThicknessString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, LineThicknessString);
	}
	LineThickness = std::stof(LineThicknessString);

	std::string BonePosString;
	for (int i = 1; i <= 1; i++) {
		std::getline(GamaPresets, BonePosString);
	}
	BonePos = std::stof(BonePosString);

	//cout << AimbotStrRead << endl;
	//istringstream(AimbotStrRead) >> Aimbot;
	// 
	std::thread first(ItemEsp);
    while (!done)
    {
		
		

		

		/*
		bool Aimbot = true;
		bool GlowEsp = true;
		bool SnapLines = true;
		bool BoxEsp = true;
		bool RainbowCircle = true;

		static float Lock = 0.908f;//.935 good
		static float Speed = 370.f;
		static float CircleRadius = 170.f;
		static int SaveYourCPU = 60;
		static float GlowEspColor[3] = { 0.0f, 255.f, 0.0f };

		static float DistanceFromEnemy = 5000.f;
		static float LineThickness = 1.f;
		*/

		//std::string input = Aimbot;


		std::string AimbotString = BoolToString(Aimbot);
		std::string GlowEspString = BoolToString(GlowEsp);
		std::string SnapLinesString = BoolToString(SnapLines);
		std::string BoxEspString = BoolToString(BoxEsp);
		std::string RainbowCircleString = BoolToString(RainbowCircle);
		std::string HealthSheildBarsString = BoolToString(HealthSheildBars);

		/*
		std::string ClosestEnemyInCircleString = BoolToString(ClosestEnemyInCircle);
		std::string ClosestEnemyToCenterOfScreenString = BoolToString(ClosestEnemyToCenterOfScreen);
		*/

		std::ostringstream LockStr;
		LockStr << Lock;
		std::string LockString(LockStr.str());

		std::ostringstream SpeedStr;
		SpeedStr << Speed;
		std::string SpeedString(SpeedStr.str());

		std::ostringstream CircleRadiusStr;
		CircleRadiusStr << CircleRadius;
		std::string CircleRadiusString(CircleRadiusStr.str());

		std::ostringstream DistanceFromEnemyStr;
		DistanceFromEnemyStr << DistanceFromEnemy;
		std::string DistanceFromEnemyString(DistanceFromEnemyStr.str());

		std::ostringstream LineThicknessStr;
		LineThicknessStr << LineThickness;
		std::string LineThicknessString(LineThicknessStr.str());

		std::ostringstream BonePosStr;
		BonePosStr << BonePos;
		std::string BonePosString(BonePosStr.str());





		std::ofstream out("GamaPresets.txt");

		out << AimbotString << "\n";
		out << GlowEspString << "\n";
		out << SnapLinesString << "\n";
		out << BoxEspString << "\n";
		out << RainbowCircleString << "\n";
		out << HealthSheildBarsString << "\n";
		out << LockString << "\n";
		out << SpeedString << "\n";
		out << CircleRadiusString << "\n";
		out << DistanceFromEnemyString << "\n";
		out << LineThicknessString << "\n";
		out << BonePosString << "\n";
		/*
		out << ClosestEnemyInCircleString << "\n";
		out << ClosestEnemyToCenterOfScreenString << "\n";
		*/
		out.close();
		



		if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {
			SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST);
		}
		else {
			SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT);
		}
		
		
		//Sleep(SaveYourCPU);
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		
		

		ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImColor(0, 20, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImColor(0, 200, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Tab] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabActive] = ImColor(0, 190, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabHovered] = ImColor(0, 65, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 200, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = ImColor(0, 200, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ResizeGrip] = ImColor(0, 255, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ResizeGripHovered] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ResizeGripActive] = ImColor(0, 50, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Border] = ImColor(0, 255, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_BorderShadow] = ImColor(0, 255, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = ImColor(0, 255, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImColor(0, 100, 0, 255); 
		ImGui::GetStyle().Colors[ImGuiCol_Header] = ImColor(255, 0, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = ImColor(0, 50, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = ImColor(0, 50, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImColor(0, 200, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImColor(0, 100, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImColor(0, 65, 0, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImColor(0, 190, 0, 255);
		
		


			static float CircleColor[3] = { 0.0f, 255.f, 0.0f };
            ImGui::Begin("Gama");             
			if (ImGui::BeginTabBar("sad")) {
				if (ImGui::BeginTabItem("ESP")) {
					//ImGui::Checkbox("TriggerBot", &toggleTriggerBot);
					ImGui::Checkbox("GlowESP", &GlowEsp);
					ImGui::Checkbox("BoxESP", &BoxEsp);
					ImGui::Checkbox("HealthSheildBars", &HealthSheildBars);
					ImGui::Checkbox("SnapLines", &SnapLines);
					ImGui::SliderFloat("Distance", &DistanceFromEnemy, 0.0f, 100000.0f);
					ImGui::SliderFloat("Line Thickness", &LineThickness, 0.0f, 10.f);

					//ImGui::SliderInt("GeneralGlowMode", &GeneralGlowMode, 0, 127);
					//ImGui::SliderInt("BorderGlowMode", &BorderGlowMode, 0, 127);
					//ImGui::SliderInt("BorderSize", &BorderSize, 0, 127);
					//ImGui::SliderInt("TransparentLevel", &TransparentLevel, 0, 127);
					ImGui::ColorEdit3("TeamGlowESP", TeamGlowEspColor);
					ImGui::ColorEdit3("EnemyGlowESP", EnemyGlowEspColor);
					
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("MISC")) {
					//ImGui::Checkbox("BHOP", &toggleBhop);
					//ImGui::Checkbox("TriggerBot", &toggleTriggerBot);
					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("AIMBOT")) {
					//ImGui::Checkbox("TriggerBot", &toggleTriggerBot);
					
					ImGui::Checkbox("AimBot", &Aimbot);
					ImGui::Checkbox("Rainbow Circle", &RainbowCircle);

					ImGui::Checkbox("ClosestEnemyInCircle", &ClosestEnemyInCircle);
					ImGui::Checkbox("ClosestEnemyToCenterOfScreen", &ClosestEnemyToCenterOfScreen);
					if (ImGui::Button("Reset Aimbot")) {
						cout << "Aimbot Reset" << endl;
						float ClosestEnt = 999999999999.f;
						int closest = -1;
					}
					ImGui::ColorEdit3("Circle Color", CircleColor);
					ImGui::SliderFloat("Lock", &Lock, 0.0f, 1.0f);
					ImGui::SliderInt("BonePos", &BonePos, 1, 8);
					ImGui::SliderFloat("Speed", &Speed, 0.0f, 500.0f); 
					ImGui::SliderFloat("CircleRadius", &CircleRadius, 0.0f, 1000.0f); 
					ImGui::SliderInt("SaveYourCPU", &SaveYourCPU, 0, 1000); 
					
				

					ImGui::EndTabItem();
				}
			}
			

            ImGui::End();
       

		Sleep(SaveYourCPU);
		Matrix ViewMatrix = Read<Matrix>(base_address + Offsets::ViewMatrix);
		uintptr_t LocalPlayer = Read<uintptr_t>(base_address + Offsets::LocalPlayer);
		/*
		for (int Entity1 = 0; Entity1 < 1000; Entity1++) {
			//uintptr_t Base = Read<uintptr_t>(base_address + 0x1f7f738);

			uintptr_t Base = Read<uintptr_t>(base_address + 0x1f7f738 + (Entity1 << 5));

			if (Base == 0) {
				continue;
			}
			
			Write<int>(Base + 0x3C8, 1);
			Write<int>(Base + 0x3D0, 2);
			Write<GlowMode>(Base + 0x2c0, { 101,127,46,90 });
			Write<float>(Base + 0x1D0, 0); //R
			Write<float>(Base + 0x1D4, 255);//G
			Write<float>(Base + 0x1D8, 0);//B
			
			
			int itemid = Read<int>(Base + 0x16b8);

			uintptr_t itemidAddr = Read<uintptr_t>(Base + 0x16b8);
			Vector3 Origin = Read<Vector3>(Base + Offsets::Origin);

			char* SigName = Read<char*>(Base + 0x0580);
			cout << SigName << endl;
			//cout << Origin.x <<endl;
			//cout << Origin.y << endl;
			//cout << Origin.z << endl;

			auto draw = ImGui::GetBackgroundDrawList();

			uint64_t viewRenderer = Read<uint64_t>(base_address + Offsets::ViewRender);
			uint64_t viewMatrix = Read<uint64_t>(viewRenderer + Offsets::ViewMatrix);
			Matrix m = Read<Matrix>(viewMatrix);
			Vector3 W2SItem = _WorldToScreen(Origin, m);
			draw->AddLine(ImVec2(windowWidth / 2, windowHeight), ImVec2(W2SItem.x, W2SItem.y), IM_COL32(0, 0, 255, 255), LineThickness);
			if (SigName == "prop_survival") {
				cout << "sad" << endl;
			}

			
			
			//char* Base = Read<char*>(base_address + Offsets::EntityList + (Entity1 << 5));
			// 
			//char* Base = Read<char*>(base_address + 0x1f7f738 + (Entity1 << 5)); //
			// 
			//uintptr_t Base = Read<uintptr_t>(base_address + Offsets::EntityList + (Entity1 << 5));
			
			//if ("prop_survival" == (Base + 0x0580 m_iSignifierName)) {
				// get m_customScriptInt  
			//	cout << "s" << endl;
			//}
			//else {
				// not an item
			//}

			
			
			

			//int itemid = Read<int>(Base + 0x16b8);//m_customScriptInt
			//Vector3 Origin = Read<Vector3>(Base + Offsets::Origin);
			//cout << Origin.x << endl;
			//cout << Origin.y << endl;
			//cout << Origin.z << endl;
			// 
			//cout << Base << endl;
		}
		*/
		/*
		DWORD64 GetEntityById(int Ent, DWORD64 Base) {
		
			DWORD64 EntityList = Base + OFFSET_ENTITYLIST; //updated
			DWORD64 BaseEntity = Read<DWORD64>(EntityList);
			if (!BaseEntity)
				return NULL;
			return  Read<DWORD64>(EntityList + (Ent << 5));
		}
		*/
		

		for (int Entity = 0; Entity < 100; Entity++) { // For Loop so that we can loop through every Entity in Game (Enemies and TeamMates)



			uintptr_t Base = Read<uintptr_t>(base_address + Offsets::EntityList + (Entity << 5));//Reading the EntityList and storing Each Enemy (its inside a for loop so it will store all 100 entity inside the 'Base' for max loop times)
			//memcpy(&matrix, (BYTE*)(base_address + Offsets::ViewMatrix), sizeof(matrix));
			// 
			if (Base == NULL) {
				continue;
			}
			//auto begin = std::chrono::high_resolution_clock::now();

			//if (Base + Offsets::TeamId == LocalPlayer + Offsets::TeamId) { //my team
				//Write<int>(Base + 0x3C8, 1);
				//Write<int>(Base + 0x3D0, 2);
				//Write<GlowMode>(Base + 0x2c0, { 101,101,46,90 });
			//	Write<float>(Base + 0x1D0, 255.f); //R
			//	Write<float>(Base + 0x1D4, 0.f);//G
			//	Write<float>(Base + 0x1D8, 0.f);//B
			//}else{
			
			//}



			Vector3 ammoPoolCapacity = Read<Vector3>(LocalPlayer + 0x257c - 0x14);

			//Vector3 value = { 10.f, 30.f, 0.f };

			//cout << ammoPoolCapacity.x << " ammo X" << endl;
			//cout << ammoPoolCapacity.y << " ammo y" << endl;

			//Write<Vector3>(LocalPlayer + 0x257c - 0x14, value);

			Vector3 LocalOrigin = Read<Vector3>(LocalPlayer + Offsets::Origin);
			Vector3 Origin = Read<Vector3>(Base + Offsets::Origin);

			uint64_t viewRenderer = Read<uint64_t>(base_address + Offsets::ViewRender);
			uint64_t viewMatrix = Read<uint64_t>(viewRenderer + Offsets::ViewMatrix);
			Matrix m = Read<Matrix>(viewMatrix);
			Vector3 entFeet1 = Read<Vector3>(Base + Offsets::Origin);
			Vector3 w2sEntFeet1 = _WorldToScreen(entFeet1, m); //if (w2sEntFeet.z <= 0.f) continue;





			//DrawNotSeenLine(hdc, TopLeftFOV.x, TopLeftFOV.y, TopRightFOV.x, TopRightFOV.y);
			//DrawNotSeenLine(hdc, TopRightFOV.x, TopRightFOV.y, BottomRightFOV.x, BottomRightFOV.y);
			//DrawNotSeenLine(hdc, BottomRightFOV.x, BottomRightFOV.y, BottomLeftFOV.x, BottomLeftFOV.y);
			//DrawNotSeenLine(hdc, BottomLeftFOV.x, BottomLeftFOV.y, TopLeftFOV.x, TopLeftFOV.y);

			//DrawNotSeenLine(hdc, 500, 500, 1000, 1000);


			//DrawNotSeenLine(hdc, 0, 0, 550, 500);

			//std::thread first(Loop);

	

			
			Vector3 entityHead;
			entityHead.x = entFeet1.x;
			entityHead.y = entFeet1.y;
			entityHead.z = entFeet1.z + 75.f;

			Vector3 w2sEntHead = _WorldToScreen(entityHead, m);

			Vector3 entityLeftVert;
			entityLeftVert.x = entFeet1.x;
			entityLeftVert.y = entFeet1.y - 15.f;
			entityLeftVert.z = entFeet1.z;

			Vector3 w2sEntLeftVert = _WorldToScreen(entityLeftVert, m);

			Vector3 entityRightVert;
			entityRightVert.x = entFeet1.x;
			entityRightVert.y = entFeet1.y + 15.f;
			entityRightVert.z = entFeet1.z;

			Vector3 w2sEntRightVert = _WorldToScreen(entityRightVert, m);


			int OwnTeam5 = Read<int>(LocalPlayer + Offsets::TeamId);
			int Team5 = Read<int>(Base + Offsets::TeamId);
			float Health5 = Read<int>(Base + Offsets::Health);
			float MaxHealth5 = Read<int>(Base + Offsets::MaxHealth);
			float SheildHealth5 = Read<int>(Base + Offsets::m_shieldHealth);
			float SheildMaxHealth5 = Read<int>(Base + Offsets::m_shieldHealthMax);
			int Knocked5 = Read<int>(Base + Offsets::Knocked);
			int Dead5 = Read<int>(Base + Offsets::Dead);
			/*
			float Visible = Read<float>(Base + 0x1b14); 
			//float VisibleFlag = Read<float>(Base + 0x09b8);

			
			
			VisibleVar = Visible;

			if (Visible > VisibleVar) {

				cout << "Visible" << endl;
				
			}
			*/
			//if (Visible > VisibleVar)
			
			

			//cout << w2sEntFeet1.x << endl;
			//cout << w2sEntFeet1.y << endl;

			
			float NewVisible = Read<float>(Base + 0x1b14);
			if (GlowEsp == true && Team5 == OwnTeam5) {


				Write<int>(Base + 0x3C8, 1);
				Write<int>(Base + 0x3D0, 2);
				//Write<GlowMode>(Base + 0x2c0, { GeneralGlowMode,BorderGlowMode,BorderSize,TransparentLevel });// 101,101,46,90 GeneralGlowMode,BorderGlowMode,BorderSize,TransparentLevel
				Write<GlowMode>(Base + 0x2c0, { 101,127,46,90 });
				Write<float>(Base + 0x1D0, TeamGlowEspColor[0]); //R
				Write<float>(Base + 0x1D4, TeamGlowEspColor[1]);//G
				Write<float>(Base + 0x1D8, TeamGlowEspColor[2]);//B
			}
			else {
				Write<int>(Base + 0x3C8, 1);
				Write<int>(Base + 0x3D0, 2);
				//Write<GlowMode>(Base + 0x2c0, { GeneralGlowMode,BorderGlowMode,BorderSize,TransparentLevel });// 101,101,46,90 GeneralGlowMode,BorderGlowMode,BorderSize,TransparentLevel
				Write<GlowMode>(Base + 0x2c0, { 101,127,46,90 });
				Write<float>(Base + 0x1D0, EnemyGlowEspColor[0]); //R
				Write<float>(Base + 0x1D4, EnemyGlowEspColor[1]);//G
				Write<float>(Base + 0x1D8, EnemyGlowEspColor[2]);//B
			}
			/*
			else if (GlowEsp == true && Team5 != OwnTeam5) {
				if (GlowEsp == true && Team5 != OwnTeam5 && NewVisible > Visible) {
					Write<int>(Base + 0x3C8, 1);
					Write<int>(Base + 0x3D0, 2);
					//Write<GlowMode>(Base + 0x2c0, { GeneralGlowMode,BorderGlowMode,BorderSize,TransparentLevel });// 101,101,46,90 GeneralGlowMode,BorderGlowMode,BorderSize,TransparentLevel
					Write<GlowMode>(Base + 0x2c0, { 101,127,46,90 });
					Write<float>(Base + 0x1D0, 255); //R
					Write<float>(Base + 0x1D4, 0);//G
					Write<float>(Base + 0x1D8, 0);//B
				}
				else if (GlowEsp == true && Team5 != OwnTeam5 && Visible != 0.0f) {

					Write<int>(Base + 0x3C8, 1);
					Write<int>(Base + 0x3D0, 2);
					//Write<GlowMode>(Base + 0x2c0, { GeneralGlowMode,BorderGlowMode,BorderSize,TransparentLevel });// 101,101,46,90 GeneralGlowMode,BorderGlowMode,BorderSize,TransparentLevel
					Write<GlowMode>(Base + 0x2c0, { 101,127,46,90 });
					Write<float>(Base + 0x1D0, 255); //R
					Write<float>(Base + 0x1D4, 255);//G
					Write<float>(Base + 0x1D8, 255);//B
				}
			}
			
			Write<float>(Visible,0);//B
			*/

			if (Base == LocalPlayer) {
				continue;
			}
			auto draw = ImGui::GetBackgroundDrawList();
			// 
			//auto draw = ImGui::GetForegroundDrawList();
			
			///ImGui::Render();
			int v1 = rand() % 255;
			int v2 = rand() % 255;
			int v3 = rand() % 255;
			if (Aimbot == true && RainbowCircle == true) {
				draw->AddCircle(ImVec2(1920 / 2, 1080 / 2), CircleRadius, IM_COL32(v1, v2, v3, 255), 100, LineThickness);
			}
			else if(Aimbot == true) {
				
				draw->AddCircle(ImVec2(1920 / 2, 1080 / 2), CircleRadius, IM_COL32(CircleColor[0], CircleColor[1], CircleColor[2], 255), 100, LineThickness);
			}
			
			Vector3 deltaVec = { Origin.x - LocalOrigin.x,Origin.y - LocalOrigin.y, Origin.z - LocalOrigin.z };
			float deltaSQRT = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);
			float DistanceFromCurrentEnemy = deltaSQRT;

			if (DistanceFromCurrentEnemy >= DistanceFromEnemy) {
				continue;
			}
			if (DistanceFromCurrentEnemy <= DistanceFromEnemy) {
				if (Knocked5 != 0) {
					
					draw->AddLine(ImVec2(windowWidth / 2, windowHeight), ImVec2(w2sEntFeet1.x, w2sEntFeet1.y), IM_COL32(0, 0, 255, 255), LineThickness);
				}
				else if (SnapLines == true && Team5 != OwnTeam5) {
					draw->AddLine(ImVec2(windowWidth / 2, windowHeight), ImVec2(w2sEntFeet1.x, w2sEntFeet1.y), IM_COL32(255, 0, 0, 255), LineThickness);
				}
				else if (Team5 == OwnTeam5 && SnapLines == true) {
					draw->AddLine(ImVec2(windowWidth / 2, windowHeight), ImVec2(w2sEntFeet1.x, w2sEntFeet1.y), IM_COL32(0, 255, 0, 255), LineThickness);
				}
				
				float HealthPerc = Health5 / MaxHealth5;
				float ShieldPerc = SheildHealth5 / SheildMaxHealth5;

				int BoxHeight = w2sEntFeet1.y - w2sEntHead.y;
				int BoxWidth = entityLeftVert.y - entityRightVert.y;
				//int HealthHeight = BoxHeight * HealthPerc;

				Vector2 TopHealth, BotHealth, TopShield,BotShield;

				BotHealth.x = (w2sEntLeftVert.x);
				BotHealth.y = (w2sEntLeftVert.x);
				TopHealth.x = (w2sEntLeftVert.x);
				//TopHealth.y = (HealthPerc * BoxHeight);
				TopHealth.y = round(HealthPerc * BoxHeight);
				TopShield.y = round(ShieldPerc * BoxHeight);
				//TopHealth.y = round(HealthPerc * w2sEntHead.y);

				//cout << HealthPerc << endl;
				//cout << Health5 << endl;
				//cout << TopHealth.y << endl;
				//cout << SheildHealth5 << endl;
				//cout << ShieldPerc << endl;
				//cout << TopShield.y << endl;

				if (HealthSheildBars == true) {


					draw->AddLine(ImVec2(w2sEntLeftVert.x + 5, w2sEntLeftVert.y), ImVec2(w2sEntLeftVert.x + 5, w2sEntLeftVert.y - TopHealth.y), IM_COL32(136, 19, 214, 255), 3.f);//HP

					draw->AddLine(ImVec2(w2sEntRightVert.x - 5, w2sEntRightVert.y), ImVec2(w2sEntRightVert.x - 5, w2sEntRightVert.y - TopShield.y), IM_COL32(0, 0, 255, 255), 3.f);//Shield
				}

				if (BoxEsp == true) {
					
					if (Knocked5 != 0) {
						draw->AddText(ImVec2(w2sEntHead.x, w2sEntHead.y), IM_COL32(0, 0, 255, 255), "Knocked");
						draw->AddLine(ImVec2(w2sEntLeftVert.x, w2sEntLeftVert.y), ImVec2(w2sEntRightVert.x, w2sEntRightVert.y), IM_COL32(0, 0, 255, 255), LineThickness);//bottom
						draw->AddLine(ImVec2(w2sEntLeftVert.x, w2sEntLeftVert.y), ImVec2(w2sEntLeftVert.x, w2sEntHead.y), IM_COL32(0, 0, 255, 255), LineThickness);//left
						draw->AddLine(ImVec2(w2sEntRightVert.x, w2sEntRightVert.y), ImVec2(w2sEntRightVert.x, w2sEntHead.y), IM_COL32(0, 0, 255, 255), LineThickness);//right
						draw->AddLine(ImVec2(w2sEntLeftVert.x, w2sEntHead.y), ImVec2(w2sEntRightVert.x, w2sEntHead.y), IM_COL32(0, 0, 255, 255), LineThickness);//top
					}
					else if (Team5 == OwnTeam5) {
						draw->AddText(ImVec2(w2sEntHead.x, w2sEntHead.y), IM_COL32(0, 255, 0, 255), "Alive");
						draw->AddLine(ImVec2(w2sEntLeftVert.x, w2sEntLeftVert.y), ImVec2(w2sEntRightVert.x, w2sEntRightVert.y), IM_COL32(0, 255, 0, 255), LineThickness);//bottom
						draw->AddLine(ImVec2(w2sEntLeftVert.x, w2sEntLeftVert.y), ImVec2(w2sEntLeftVert.x, w2sEntHead.y), IM_COL32(0, 255, 0, 255), LineThickness);//left
						draw->AddLine(ImVec2(w2sEntRightVert.x, w2sEntRightVert.y), ImVec2(w2sEntRightVert.x, w2sEntHead.y), IM_COL32(0, 255, 0, 255), LineThickness);//right
						draw->AddLine(ImVec2(w2sEntLeftVert.x, w2sEntHead.y), ImVec2(w2sEntRightVert.x, w2sEntHead.y), IM_COL32(0, 255, 0, 255), LineThickness);//top
					}
					else if (Team5 != OwnTeam5) {
						draw->AddText(ImVec2(w2sEntHead.x, w2sEntHead.y), IM_COL32(255, 0, 0, 255), "Alive");
						draw->AddLine(ImVec2(w2sEntLeftVert.x, w2sEntLeftVert.y), ImVec2(w2sEntRightVert.x, w2sEntRightVert.y), IM_COL32(255, 0, 0, 255), LineThickness);//bottom
						draw->AddLine(ImVec2(w2sEntLeftVert.x, w2sEntLeftVert.y), ImVec2(w2sEntLeftVert.x, w2sEntHead.y), IM_COL32(255, 0, 0, 255), LineThickness);//left
						draw->AddLine(ImVec2(w2sEntRightVert.x, w2sEntRightVert.y), ImVec2(w2sEntRightVert.x, w2sEntHead.y), IM_COL32(255, 0, 0, 255), LineThickness);//right
						draw->AddLine(ImVec2(w2sEntLeftVert.x, w2sEntHead.y), ImVec2(w2sEntRightVert.x, w2sEntHead.y), IM_COL32(255, 0, 0, 255), LineThickness);//top
					}

					
				}

				
			}

			
			/*
			if (Aimbot == true) {
				draw->AddLine(ImVec2(TopLeftFOV.x, TopLeftFOV.y), ImVec2(TopRightFOV.x, TopRightFOV.y), IM_COL32(255, 0, 0, 255), 3.f);
				draw->AddLine(ImVec2(TopRightFOV.x, TopRightFOV.y), ImVec2(BottomRightFOV.x, BottomRightFOV.y), IM_COL32(255, 0, 0, 255), 3.f);
				draw->AddLine(ImVec2(BottomRightFOV.x, BottomRightFOV.y), ImVec2(BottomLeftFOV.x, BottomLeftFOV.y), IM_COL32(255, 0, 0, 255), 3.f);
				draw->AddLine(ImVec2(BottomLeftFOV.x, BottomLeftFOV.y), ImVec2(TopLeftFOV.x, TopLeftFOV.y), IM_COL32(255, 0, 0, 255), 3.f);
			}
			*/

			ImGui::Render();
			
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
			g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			
			
			
			//float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
			//g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clearColor);

			//g_pSwapChain->Present(1, 0); // Present with vsync//was checked
			// 
			// 
			//g_pSwapChain->Present(0, 0); // Present without vsync

			
			//auto end = std::chrono::high_resolution_clock::now();
			//////auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
			//auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

			//cout << elapsed << endl;
			

			if (SnapLines == true) {
				//DrawNotSeenLine(hdc, ScreenWidth / 2, ScreenHeight, w2sEntFeet1.x, w2sEntFeet1.y);//good un comment when done
			}
			//bool ClosestEnemyInCircle = true;
			//bool ClosestEnemyToCenterOfScreen = true;
			if (ClosestEnemyInCircle == true) {
				ClosestEnemyToCenterOfScreen = false;
			}
			if (ClosestEnemyToCenterOfScreen == true) {
				ClosestEnemyInCircle = false;
			}
			
			if (GetAsyncKeyState(VK_RBUTTON) && Aimbot == true && ClosestEnemyInCircle == true) {
				//if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {


					//uint64_t viewRenderer = Read<uint64_t>(base_address + Offsets::ViewRender);
					//uint64_t viewMatrix = Read<uint64_t>(viewRenderer + Offsets::ViewMatrix);
					//Matrix m = Read<Matrix>(viewMatrix);

					//mouse_movee(w2sEntFeet.x, w2sEntFeet.y);

				
				for (int i = 0; i < 100; i++) {


					uintptr_t newBase = Read<uintptr_t>(base_address + Offsets::EntityList + (i << 5));

					int OwnTeam = Read<int>(LocalPlayer + Offsets::TeamId);
					int Team = Read<int>(newBase + Offsets::TeamId);
					float Health = Read<int>(newBase + Offsets::Health);
					float MaxHealth = Read<int>(newBase + Offsets::MaxHealth);
					int Knocked = Read<int>(newBase + Offsets::Knocked);
					int Dead = Read<int>(newBase + Offsets::Dead);
					Vector2 FOV = Read<Vector2>(LocalPlayer + Offsets::FOV);


					if (newBase == NULL || Team == OwnTeam || Health <= 0 || Health >= 101) {
						continue;
					}

					if (Team != OwnTeam && Health > 0 && Health < 101 && Dead == 0 && Knocked == 0) {



						uint64_t viewRenderer2 = Read<uint64_t>(base_address + Offsets::ViewRender);
						uint64_t viewMatrix2 = Read<uint64_t>(viewRenderer2 + Offsets::ViewMatrix);
						Matrix m2 = Read<Matrix>(viewMatrix2);
						Vector3 entFeet = Read<Vector3>(newBase + Offsets::Origin);
						Vector3 entHead = entFeet; entHead.z += 35.f;
						Vector3 w2sEntFeet = _WorldToScreen(entFeet, m2); //if (w2sEntFeet.z <= 0.f) continue;				
						Vector3 w2sEntHead = _WorldToScreen(entHead, m2); //if (w2sEntHead.z <= 0.f) continue;
						
						float distance = sqrt((w2sEntFeet.x - ScreenWidth / 2) * (w2sEntFeet.x - ScreenWidth / 2) + (w2sEntFeet.y - ScreenHeight / 2) * (w2sEntFeet.y - ScreenHeight / 2));
						if (distance < CircleRadius) {
						//if (w2sEntFeet.x >= TopLeftFOV.x && w2sEntFeet.y >= TopLeftFOV.y && w2sEntFeet.x <= TopRightFOV.x && w2sEntFeet.y >= TopRightFOV.y && w2sEntFeet.x >= BottomLeftFOV.x && w2sEntFeet.y <= BottomLeftFOV.y && w2sEntFeet.x <= BottomRightFOV.x && w2sEntFeet.y <= BottomRightFOV.y) {//this works


							//if (w2sEntFeet.x >= 500 && w2sEntFeet.y >= 500 && w2sEntFeet.x <= 1000 && w2sEntFeet.y <= 500 && w2sEntFeet.x >= 500 && w2sEntFeet.y >= 1000 && w2sEntFeet.x <= 1000 && w2sEntFeet.y <= 1000) {
							//	cout << "is in FOV square" << endl;
							//}


							//Vector3 deltaVec = { w2sEntFeet.x - LocalOrigin.x,w2sEntFeet.y - LocalOrigin.y, w2sEntFeet.z - LocalOrigin.z };
							Vector3 deltaVec = { entFeet.x - LocalOrigin.x,entFeet.y - LocalOrigin.y, entFeet.z - LocalOrigin.z };
							float deltaSQRT = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);
							float DistanceFromEnemy = deltaSQRT;

							//cout << DistanceFromEnemy << endl;

							//float distance = entFeet.x - LocalOrigin.x;
							if (DistanceFromEnemy < ClosestEnt) {
								//if (DistanceFromEnemy < ClosestEnt) {
								ClosestEnt = DistanceFromEnemy;
								closest = i;
							}
						}
					}

				}



				uintptr_t closestEntity = Read<uintptr_t>(base_address + Offsets::EntityList + (closest << 5));

				int OwnTeam = Read<int>(LocalPlayer + Offsets::TeamId);
				int Team = Read<int>(closestEntity + Offsets::TeamId);
				float Health = Read<int>(closestEntity + Offsets::Health);
				float MaxHealth = Read<int>(closestEntity + Offsets::MaxHealth);
				int Knocked = Read<int>(closestEntity + Offsets::Knocked);
				int Dead = Read<int>(closestEntity + Offsets::Dead);

				if (Team != OwnTeam && Health > 0 && Health < 101 && Dead == 0 && Knocked == 0) {
					if (LocalPlayer != closestEntity) {
						Vector3 newLocalPlayerOrigin = Read<Vector3>(LocalPlayer + Offsets::Origin);
						Vector3 closestEntityplayerVectorOrigin = Read<Vector3>(closestEntity + Offsets::Origin);

						Vector3 newDeltaVec = { closestEntityplayerVectorOrigin.x - newLocalPlayerOrigin.x,closestEntityplayerVectorOrigin.y - newLocalPlayerOrigin.y, closestEntityplayerVectorOrigin.z - newLocalPlayerOrigin.z };
						float deltaVecLength = sqrt(newDeltaVec.x * newDeltaVec.x + newDeltaVec.y * newDeltaVec.y + newDeltaVec.z * newDeltaVec.z);
						Vector3 neww2sEntFeet = _WorldToScreen(closestEntityplayerVectorOrigin, m); //if (w2sEntFeet.z <= 0.f) continue;	

						Vector3 newHeadPos;
						newHeadPos.x = newLocalPlayerOrigin.x;
						newHeadPos.y = newLocalPlayerOrigin.y + 5.f;
						newHeadPos.z = newLocalPlayerOrigin.z + .000000001f;
						Vector3 HeadLocation = _WorldToScreen(newHeadPos, m);

						//DrawGreenLine(hdc, ScreenWidth / 2, ScreenHeight, neww2sEntFeet.x, neww2sEntFeet.y);



						if (Team != OwnTeam && Health > 0 && Health < 101 && Dead == 0 && Knocked == 0) {
							float distance = sqrt((neww2sEntFeet.x - ScreenWidth / 2) * (neww2sEntFeet.x - ScreenWidth / 2) + (neww2sEntFeet.y - ScreenHeight / 2) * (neww2sEntFeet.y - ScreenHeight / 2));
							if(distance < CircleRadius) {
							//if (neww2sEntFeet.x >= TopLeftFOV.x && neww2sEntFeet.y >= TopLeftFOV.y && neww2sEntFeet.x <= TopRightFOV.x && neww2sEntFeet.y >= TopRightFOV.y && neww2sEntFeet.x >= BottomLeftFOV.x && neww2sEntFeet.y <= BottomLeftFOV.y && neww2sEntFeet.x <= BottomRightFOV.x && neww2sEntFeet.y <= BottomRightFOV.y) {//this works
								//if (w2sEntFeet1.x >= TopLeftFOV.x && w2sEntFeet1.y >= TopLeftFOV.y && w2sEntFeet1.x <= TopRightFOV.x && w2sEntFeet1.y >= TopRightFOV.y && w2sEntFeet1.x >= BottomLeftFOV.x && w2sEntFeet1.y <= BottomLeftFOV.y && w2sEntFeet1.x <= BottomRightFOV.x && w2sEntFeet1.y <= BottomRightFOV.y) {



								float pitch = -asin(newDeltaVec.z / deltaVecLength) * (180 / 3.14159265358 + 5);
								float yaw = atan2(newDeltaVec.y, newDeltaVec.x) * (180 / 3.14159265358);

								float slowpitch = -asin(newDeltaVec.z / deltaVecLength);
								float slowyaw = atan2(newDeltaVec.y, newDeltaVec.x);

								//float pitch = -asin(deltaVec.z / deltaSQRT) * (180 / 3.14159265358 + 5);
								//float yaw = atan2(deltaVec.y, deltaVec.x) * (180 / 3.14159265358);

								//Vector3 value = { pitch, yaw, 0.f }; 
								//Write<Vector3>(LocalPlayer + 0x257c - 0x14, value);

								/*

								Vector3 playerView = Read<Vector3>(LocalPlayer + 0x257c - 0x14);
								float viewX = playerView.x;
								float viewY = playerView.y;

								for (float x = viewX; x == pitch; x = x + .1f) {
									Sleep(100);
									Vector3 value2 = { x, 0.f, 0.f };
									Write<Vector3>(LocalPlayer + 0x257c - 0x14, value2);//this works
								}

								for (float y = viewY; y == yaw; y = y + .1f) {
									Sleep(100);
									Vector3 value2 = { 0.f, yaw, 0.f };
									Write<Vector3>(LocalPlayer + 0x257c - 0x14, value2);//this works
								}
								*/


								Vector3 value = { pitch, yaw, 0.f };
								Vector3 valueSlow = { 1, 1, 0.f };

								//Write<Vector3>(LocalPlayer + 0x257c - 0x14, valuediv);
								// 
								// 
								//Sleep(60);//aim sleep
								// 
								//Write<Vector3>(LocalPlayer + 0x257c - 0x14, value);//this works


								//for (auto t{ ammoPoolCapacity.x }; t <= pitch; t += 0.1) {//x
								//	std::cout << std::lerp(0.0, 10.0, t) << endl;
								//}

								Vector3 LocalPlayerViewAngles = Read<Vector3>(LocalPlayer + 0x257c - 0x14);

								float LocalPlayerPitch = LocalPlayerViewAngles.x;
								float LocalPlayerYaw = LocalPlayerViewAngles.y;

								float dstBetweenPitch = LocalPlayerPitch - pitch;

								float dstBetweenPitchDiv = dstBetweenPitch / 2;

								float newLocalPlayerPitch = dstBetweenPitchDiv + LocalPlayerPitch;

								Vector3 valueSlowPitch = { newLocalPlayerPitch, 0.f, 0.f };

								//Write<Vector3>(LocalPlayer + 0x257c - 0x14, valueSlowPitch);

								uint64_t viewRenderer1 = Read<uint64_t>(base_address + Offsets::ViewRender);
								uint64_t viewMatrix1 = Read<uint64_t>(viewRenderer + Offsets::ViewMatrix);
								Matrix m1 = Read<Matrix>(viewMatrix);


								float aimspeed = 390.f;//Speed At which it will aim
								float lock = 0.935f;//Target Locking Power (High = Aimlock).98 is high .935 no ban
								float Fov = 100.f;//Fov Aim

								Vector3 AimAtPoss = WorldToScreen(GetBonePos(closestEntity, BonePos, closestEntityplayerVectorOrigin), m1, ScreenWidth, ScreenHeight);
								//Vector3 AimAtD = WorldToScreen(GetBonePos(Entity, 6, w2sEntFeet1), m1, ScreenWidth, ScreenHeight);
								//if (CenterDistance(HeadLocation.x, HeadLocation.y, ScreenWidth / 2, ScreenHeight / 2) <= Fov) {
									//auto Aimat = Vector2((float)round(AimAtPoss.x), (float)round(AimAtPoss.y));
									//AimAtPos(ScreenWidth, ScreenHeight, Aimat.x, Aimat.y, aimspeed, lock, false);
									// 
									// 
								AimAtPos(ScreenWidth, ScreenHeight, AimAtPoss.x, AimAtPoss.y, Speed, Lock, false);//best 
								//AimAtPos(ScreenWidth, ScreenHeight, AimAtD.x, AimAtD.y, aimspeed, lock, false);
							//AimAtPos(ScreenWidth, ScreenHeight, HeadLocation.x, HeadLocation.y, aimspeed, lock, false);//goodish
								//AimAtPos(ScreenWidth, ScreenHeight, neww2sEntFeet.x, neww2sEntFeet.y -= 10, aimspeed, lock, false);//goodish

							//}

							/*
							int m_pCommands = Read<int>(Offsets::cinput + 0xF8);//<int>(Offsets::cinput + 0xF8, true);


							auto first_command_number = Read<int>(m_pCommands) ;
							while (first_command_number == Read<int>(m_pCommands)); //verifies that current usercmd is latest
							auto next_cmd_number = Read<int>(m_pCommands + 0x218) + 1 ; //Grab next current usercmd (0x218 is size of UserCmd)

							int current_command{ m_pCommands + 0x218 * (next_cmd_number % 750) };
							while (next_cmd_number != Read<int>(current_command)); //Wait for usercmd to be accessed
							int old_command{ m_pCommands + 0x218 * ((next_cmd_number - 1) % 750) };

							Vector2 view_angles{ 0.0f, 0.0f };
							Write(old_command + 0x38, 1); //Write to buttons
							Write(old_command + 0xC, view_angles); //Write to viewangles

							next_cmd_number = Read<int>(current_command) + 1;

							*/

							//for (auto t{ 0.0 }; t <= 1.0; t += 0.1) {//y
							//	std::cout << std::lerp(0.0, 10.0, t) << endl;
							//}
							//for (auto t{ 0.0 }; t <= 1.0; t += 0.1) {//y
							//	std::cout << std::lerp(0.0, 10.0, t) << endl;
							//}
							//for (auto t{ LocalPlayerViewAngles.x }; t <= slowpitch; t += 0.1) {//y
							//	float slowPitch = std::lerp(0, 10, t);
							//	valueSlow.x = slowPitch * (180 / 3.14159265358 + 5);
							//	Write<Vector3>(LocalPlayer + 0x257c - 0x14, valueSlow);//this works
							//}
							//for (auto t{ LocalPlayerViewAngles.y }; t <= slowyaw; t += 0.1) {//y
							//	float slowYaw = std::lerp(0, 10, t);
							//	valueSlow.y = slowYaw * (180 / 3.14159265358);
							//	Write<Vector3>(LocalPlayer + 0x257c - 0x14, valueSlow);//this works
							//}



							//const float a = LocalPlayerViewAngles.x, b = pitch;
							//std::cout << (a == std::lerp(a, b, 0.5f)) << " H" << endl;


							//float gfi = 10;
							//Vector3 gft = { 10, 10, 10 };

							//for (auto t{ gft.x }; t <= 1.0; t += 0.1) {//y
							//	std::cout << std::lerp(0, 10, t) << endl;
							//}

							/*
							float aX, aY;


							float centerX = ScreenWidth / 2.f;
							float centerY = ScreenHeight / 2.f;

							float enemyX = w2sEntFeet1.x;
							float enemyY = w2sEntFeet1.y;

							aX = enemyX - centerX;
							aY = enemyY - centerY;

							mouse_event(MOUSEEVENTF_MOVE, aX, aY, 0, 0);

							*/

							/*
							float aimspeed = 250.f;//Speed At which it will aim
							float lock = 5.f;//Target Locking Power (High = Aimlock)
							float Fov = 100.f;//Fov Aim
							Vector3 AimAtPoss = WorldToScreen(GetBonePos(Entity, 6, Origin), ViewMatrix, ScreenWidth, ScreenHeight);
							if (CenterDistance(HeadLocation.x, HeadLocation.y, ScreenWidth / 2, ScreenHeight / 2) <= Fov) {
								auto Aimat = Vector2((float)round(AimAtPoss.x), (float)round(AimAtPoss.y));
								AimAtPos(ScreenWidth, ScreenHeight, Aimat.x, Aimat.y, aimspeed, lock, false);
								//cout << " 2d x" << endl;
							}
							*/

								//cout << ClosestEnt << endl;
								//cout << closest << endl;

								ClosestEnt = 999999.f;
								closest = -1;
							}
						}
					}
				}
			}
			else if (GetAsyncKeyState(VK_RBUTTON) && Aimbot == true && ClosestEnemyToCenterOfScreen == true) {
				for (int i = 0; i < 100; i++) {


					uintptr_t newBase = Read<uintptr_t>(base_address + Offsets::EntityList + (i << 5));

					int OwnTeam = Read<int>(LocalPlayer + Offsets::TeamId);
					int Team = Read<int>(newBase + Offsets::TeamId);
					float Health = Read<int>(newBase + Offsets::Health);
					float MaxHealth = Read<int>(newBase + Offsets::MaxHealth);
					int Knocked = Read<int>(newBase + Offsets::Knocked);
					int Dead = Read<int>(newBase + Offsets::Dead);
					Vector2 FOV = Read<Vector2>(LocalPlayer + Offsets::FOV);


					if (newBase == NULL || Team == OwnTeam || Health <= 0 || Health >= 101) {
						continue;
					}

					if (Team != OwnTeam && Health > 0 && Health < 101 && Dead == 0 && Knocked == 0) {//alive

						uint64_t viewRenderer2 = Read<uint64_t>(base_address + Offsets::ViewRender);
						uint64_t viewMatrix2 = Read<uint64_t>(viewRenderer2 + Offsets::ViewMatrix);
						Matrix m2 = Read<Matrix>(viewMatrix2);
						Vector3 entFeet = Read<Vector3>(newBase + Offsets::Origin);
						Vector3 entHead = entFeet; entHead.z += 35.f;
						Vector3 w2sEntFeet = _WorldToScreen(entFeet, m2); //if (w2sEntFeet.z <= 0.f) continue;				
						Vector3 w2sEntHead = _WorldToScreen(entHead, m2); //if (w2sEntHead.z <= 0.f) continue;

						float distance = sqrt((w2sEntFeet.x - ScreenWidth / 2) * (w2sEntFeet.x - ScreenWidth / 2) + (w2sEntFeet.y - ScreenHeight / 2) * (w2sEntFeet.y - ScreenHeight / 2));

						if (distance < CircleRadius) {
						
							Vector3 deltaVec = { entFeet.x - LocalOrigin.x,entFeet.y - LocalOrigin.y, entFeet.z - LocalOrigin.z };
							float deltaSQRT = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y + deltaVec.z * deltaVec.z);
							float DistanceFromEnemy = deltaSQRT;


							Vector2 deltaVec1 = { w2sEntHead.x - ScreenWidth / 2,w2sEntHead.y - ScreenHeight / 2};
							float deltaSQRT1 = sqrt(deltaVec.x * deltaVec.x + deltaVec.y * deltaVec.y);

							if (deltaSQRT1 < ClosestEnemyToCenterOfScreen) {
								//if (DistanceFromEnemy < ClosestEnt) {
								ClosestEnemyToCenterOfScreen = deltaSQRT1;
								ClosestEntToCenter = i;
							}
						}
					}

				}

				uintptr_t closestEntity = Read<uintptr_t>(base_address + Offsets::EntityList + (closest << 5));

				int OwnTeam = Read<int>(LocalPlayer + Offsets::TeamId);
				int Team = Read<int>(closestEntity + Offsets::TeamId);
				float Health = Read<int>(closestEntity + Offsets::Health);
				float MaxHealth = Read<int>(closestEntity + Offsets::MaxHealth);
				int Knocked = Read<int>(closestEntity + Offsets::Knocked);
				int Dead = Read<int>(closestEntity + Offsets::Dead);

				if (Team != OwnTeam && Health > 0 && Health < 101 && Dead == 0 && Knocked == 0) {
					if (LocalPlayer != closestEntity) {
						Vector3 newLocalPlayerOrigin = Read<Vector3>(LocalPlayer + Offsets::Origin);
						Vector3 closestEntityplayerVectorOrigin = Read<Vector3>(closestEntity + Offsets::Origin);

						Vector3 newDeltaVec = { closestEntityplayerVectorOrigin.x - newLocalPlayerOrigin.x,closestEntityplayerVectorOrigin.y - newLocalPlayerOrigin.y, closestEntityplayerVectorOrigin.z - newLocalPlayerOrigin.z };
						float deltaVecLength = sqrt(newDeltaVec.x * newDeltaVec.x + newDeltaVec.y * newDeltaVec.y + newDeltaVec.z * newDeltaVec.z);
						Vector3 neww2sEntFeet = _WorldToScreen(closestEntityplayerVectorOrigin, m); //if (w2sEntFeet.z <= 0.f) continue;	

						Vector3 newHeadPos;
						newHeadPos.x = newLocalPlayerOrigin.x;
						newHeadPos.y = newLocalPlayerOrigin.y + 5.f;
						newHeadPos.z = newLocalPlayerOrigin.z + .000000001f;
						Vector3 HeadLocation = _WorldToScreen(newHeadPos, m);

						if (Team != OwnTeam && Health > 0 && Health < 101 && Dead == 0 && Knocked == 0) {
							float distance = sqrt((neww2sEntFeet.x - ScreenWidth / 2) * (neww2sEntFeet.x - ScreenWidth / 2) + (neww2sEntFeet.y - ScreenHeight / 2) * (neww2sEntFeet.y - ScreenHeight / 2));
							if (distance < CircleRadius) {


								uint64_t viewRenderer1 = Read<uint64_t>(base_address + Offsets::ViewRender);
								uint64_t viewMatrix1 = Read<uint64_t>(viewRenderer + Offsets::ViewMatrix);
								Matrix m1 = Read<Matrix>(viewMatrix);


								//float aimspeed = 390.f;//Speed At which it will aim
								//float lock = 0.935f;//Target Locking Power (High = Aimlock).98 is high .935 no ban
								//float Fov = 100.f;//Fov Aim

								Vector3 AimAtPoss = WorldToScreen(GetBonePos(closestEntity, BonePos, closestEntityplayerVectorOrigin), m1, ScreenWidth, ScreenHeight);

								AimAtPos(ScreenWidth, ScreenHeight, AimAtPoss.x, AimAtPoss.y, Speed, Lock, false);//best 


								ClosestEnemyToCenterOfScreen = 999999.f;
								ClosestEntToCenter = -1;
							}
						}
					}
				}
			}
			
			//ClosestEnt = 999999.f;
			//closest = -1;
			// 
			//float Distance = (Vector3::Distance(LocalOrigin, Origin)) / 36.5f; //Calculating the distance of each enemy from ourself (not Our teammate but only YOU)

			//cout << Base + Offsets::Health;
			//cout << Base + Offsets::Dead;
			//cout << LocalPlayer + Offsets::Dead << endl;

			//char Dist[100];
			//snprintf(Dist, sizeof(Dist), "%f", Distance);//Storing Distance(float value) as a character so that we can print it

			//DrawCrosshair(hdc, ScreenWidth, ScreenHeight, 60);//Draw CrossHasir
			//if ((int)Distance <= 2000) {

			/*
			int OwnTeam = Read<int>(LocalPlayer + Offsets::TeamId);
			int Team = Read<int>(Base + Offsets::TeamId);
			float Health = Read<int>(Base + Offsets::Health);
			float MaxHealth = Read<int>(Base + Offsets::MaxHealth);
			int Knocked = Read<int>(Base + Offsets::Knocked);
			int Dead = Read<int>(Base + Offsets::Dead);
			//string PlayerName = Read<string>(Base + Offsets::PlayerName);
			Vector3 ViewAngle = Read<Vector3>(LocalPlayer + Offsets::ViewAngle);

			float ViewAngleX = Read<float>(LocalPlayer + Offsets::OViewAngleX);
			float ViewAngleY = Read<float>(LocalPlayer + Offsets::OViewAngleY);

			Vector3 HeadPos;
			HeadPos.x = Origin.x;
			HeadPos.y = Origin.y;
			HeadPos.z = Origin.z + 75.f;

			Vector3 OriginPos = WorldToScreen(Origin, ViewMatrix, ScreenWidth, ScreenHeight);//Converting Enemy position to 2d 
			Vector3 HeadLocation = WorldToScreen(HeadPos, ViewMatrix, ScreenWidth, ScreenHeight);//Converting Enemy HeadLocation to 2d
			*/

			//DrawNotSeenLine(hdc, ScreenWidth / 2, ScreenHeight / 2, Origin.x, Origin.y);//better
			/*

			float height = HeadLocation.y - OriginPos.y;//Calculations for box height
			float width = height / 2.4f;//Calculations for box width


			if (OriginPos.z >= 0.01f && Team != OwnTeam && Health > 0 && Health < 101 && Dead == 0) {
				if (Knocked > 0) {
					DrawKnockedString(hdc, OriginPos.x, OriginPos.y + 30, "Knocked");//originPos is OG
					DrawKnockedString(hdc, OriginPos.x, OriginPos.y + 10, Dist);
					DrawKnockedRectangle(hdc, OriginPos.x - (width / 2), OriginPos.y, width, height, 1);
					DrawKnockedLine(hdc, ScreenWidth / 2, ScreenHeight / 8, HeadLocation.x, HeadLocation.y);
				} else { //if not knocked then do other stuff
					DrawNotSeenString(hdc, OriginPos.x, OriginPos.y + 10, Dist);//Draw Ditance of enemy
					DrawNotSeenRectangle(hdc, OriginPos.x - (width / 2), OriginPos.y, width, height, 1);
					DrawNotSeenLine(hdc, ScreenWidth / 2, ScreenHeight / 8, HeadLocation.x, HeadLocation.y);//Draw a line from screen top to enemy head
				}

				/*
				if (GetAsyncKeyState(VK_LSHIFT)) {//Holding Left Shift will activate Aimbot
					cout << "shift" << endl;
					float aimspeed = 250.f;//Speed At which it will aim
					float lock = 5.f;//Target Locking Power (High = Aimlock)
					float Fov = 100.f;//Fov Aim
					Vector3 AimAtPoss = WorldToScreen(GetBonePos(Entity, 6, Origin), ViewMatrix, ScreenWidth, ScreenHeight);
					if (CenterDistance(HeadLocation.x, HeadLocation.y, ScreenWidth / 2, ScreenHeight / 2) <= Fov) {
						auto Aimat = Vector2((float)round(AimAtPoss.x), (float)round(AimAtPoss.y));
						AimAtPos(ScreenWidth, ScreenHeight, Aimat.x, Aimat.y, aimspeed, lock, false);
					}
				}
				*/

			
		}


		if (GetAsyncKeyState(VK_DELETE)) {
			break;
		}
		//ImGui::Render();
		g_pSwapChain->Present(1, 0);
    }

	

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


    

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;



    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);


}


std::string tm_to_readable_time(tm ctx) {
	char buffer[80];

	strftime(buffer, sizeof(buffer), "%a %m/%d/%y %H:%M:%S %Z", &ctx);

	return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
	auto cv = strtol(timestamp.c_str(), NULL, 10); // long

	return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
	std::tm context;

	localtime_s(&context, &timestamp);

	return context;
}

