#include<windows.h>
#include "3rd/glew-2.1.0/GL/glew.h"

#include <GL/GL.h>
#include <GL/GLU.h>
#include <iostream>

#include "VRT.h"

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")


// settings
unsigned int SCR_WIDTH = 512;
unsigned int SCR_HEIGHT = 512;

HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE   hIns = NULL;
bool	keys[256];			// Array Used For The Keyboard Routine
bool	fullscreen = FALSE;	// Fullscreen Flag Set To Fullscreen Mode By Default

bool firstFrame = true;
bool lbudown = false, lbuup = true;
UINT g_nXCoor1 = 0, g_nXCoor2 = 0, g_nXCoor3 = 0; //The position of X 
UINT g_nYCoor1 = 0, g_nYCoor2 = 0, g_nYCoor3 = 0; //The position of Y


// camera
//Scamera mCameraNode(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

VRT* pVRT;

void InitScene()
{
	pVRT = new VRT();
	pVRT->Init();
}
GLvoid MoveCamera();
float pImage[512*512];
void DrawScene()
{
	MoveCamera();

	//render to screen
	pVRT->Render();

	//render to pImage
	//pVRT->RenderToTarget(pImage,512,512);
}
void FiniScene()
{
	delete pVRT;
}
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);						// Reset The Current Viewport	
}

GLvoid MoveCamera()
{
}

bool OnSpecialKeyDown(int key, int x, int y)  //按键按下
{
	switch (key)
	{
	case VK_LEFT:
		keys[VK_LEFT] = true;
		break;
	case VK_RIGHT:
		keys[VK_RIGHT] = true;
		break;
	case VK_UP:
		keys[VK_UP] = true;
		break;
	case VK_DOWN:
		keys[VK_DOWN] = true;
		break;
	case VK_SPACE:
		keys[VK_SPACE] = true;
		break;
	case VK_PRIOR:break;
	case VK_NEXT:break;
	case VK_HOME:break;
	case VK_END:break;
	case VK_INSERT:break;
	case VK_DELETE:break;
	case 'A':
		keys['A'] = true;
		break;
	case 'D':
		keys['D'] = true;
		break;
	case 'W':
		keys['W'] = true;
		break;
	case 'S':
		keys['S'] = true;
		break;
	case 'C':
		keys['C'] = true;
		break;
	case 'X':
		keys['X'] = true;
		break;
	default:
		return false;
	}
	return true;
}

bool OnSpecialKeyUp(int key, int x, int y)//按键松开
{
	switch (key)
	{
	case VK_LEFT:
		keys[VK_LEFT] = false;
		break;
	case VK_RIGHT:
		keys[VK_RIGHT] = false;
		break;
	case VK_UP:
		keys[VK_UP] = false;
		break;
	case VK_DOWN:
		keys[VK_DOWN] = false;
		break;
	case VK_PRIOR:break;
	case VK_SPACE:
		keys[VK_SPACE] = false;
		break;
	case VK_NEXT:break;
	case VK_HOME:break;
	case VK_END:break;
	case VK_INSERT:break;
	case VK_DELETE:break;
	case 'A':
		keys['A'] = false;
		break;
	case 'D':
		keys['D'] = false;
		break;
	case 'W':
		keys['W'] = false;
		break;
	case 'S':
		keys['S'] = false;
		break;
	case 'C':
		keys['C'] = false;
		break;
	case 'X':
		keys['X'] = false;
		break;
	default:
		return false;
	}
	return true;
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}
	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}
	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}
}
LRESULT CALLBACK GLWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
		keys[wParam] = TRUE;					// If So, Mark It As TRUE

		int virtKey = (int)wParam;

		// Get cursor position client coordinates.
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hwnd, &point);
		int xPos = (int)point.x;
		int yPos = (int)point.y;

		OnSpecialKeyDown(virtKey, xPos, yPos);

		return 0;								// Jump Back
	}

	case WM_KEYUP:								// Has A Key Been Released?
	{
		keys[wParam] = FALSE;					// If So, Mark It As FALSE

		int virtKey = (int)wParam;

		// get the cursor position in client coordinates
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hwnd, &point);
		int xPos = (int)point.x;
		int yPos = (int)point.y;

		OnSpecialKeyUp(virtKey, xPos, yPos);

		return 0;								// Jump Back
	}

	case WM_SIZE:								// Resize The OpenGL Window
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;								// Jump Back
	}
	case WM_LBUTTONDOWN:  //0x00A1#define WM_LBUTTONDOWN
	{
		lbudown = true;
		g_nXCoor1 = LOWORD(lParam);
		g_nYCoor1 = HIWORD(lParam);
		lbuup = false;
		break;
	}
	case WM_LBUTTONUP:  //0x0201#define WM_LBUTTONUP
	{
		lbuup = true;
		g_nXCoor2 = LOWORD(lParam);
		g_nYCoor2 = HIWORD(lParam);
		break;
	}
	case WM_MOUSEMOVE:  //WM_MOUSEMOVE
	{
		if (lbudown && (!lbuup))
		{
			g_nXCoor2 = LOWORD(lParam);
			g_nYCoor2 = HIWORD(lParam);
			float dx = (float(g_nXCoor2) - float(g_nXCoor1))*0.25*3.1415 / 600 * 5;  //计算X方向旋转的角度
			float dy = (float(g_nYCoor2) - float(g_nYCoor1))*0.25*3.1415 / 600 * 5;  //计算X方向旋转的角度

			lbuup = false;
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		int deltaZ = HIWORD(wParam);
	}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR IpCmdLine, int nShowCmd)
{
	WNDCLASSEX wndclass;
	wndclass.cbClsExtra = 0;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = NULL;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = NULL;
	wndclass.hIconSm = NULL;
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = GLWindowProc;
	wndclass.lpszClassName = "GLWindow";//name
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	ATOM atom = RegisterClassEx(&wndclass);
	if (!atom)
	{
		MessageBox(NULL, "Notice", "Error", MB_OK);
		return 0;
	}
	//Set the size of the window (without frame)
	RECT rect;
	rect.left = 0;
	rect.right = SCR_WIDTH;
	rect.top = 0;
	rect.bottom = SCR_HEIGHT;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, NULL);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	//Create Window
	hWnd = CreateWindowEx(NULL, "GLWindow", "OpenGL Window", WS_OVERLAPPEDWINDOW,
		100, 100, windowWidth, windowHeight,
		NULL, NULL, hInstance, NULL);
	hIns = hInstance;
	//Pixel format
	hDC = GetDC(hWnd);//device
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nVersion = 1;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);

	glewInit();
	int main_version, sub_version, release_version;
	const char* version = (const char*)glGetString(GL_VERSION);
	/*sscanf(version, "%d.%d.%d", &main_version, &sub_version, &release_version);
	if (main_version < 2)
	{
		MessageBox(NULL, L"verson of opengl is not supported!", MB_OK, 0);
		return 0;
	}*/

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	std::string str = version;
	InitScene();
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				KillGLWindow();
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);//调用GLWindowProc
		}
		DrawScene();
		//mScene.DrawScene();
		SwapBuffers(hDC);
	}
	FiniScene();
	return 0;
}