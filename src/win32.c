#include <hap.h>

#ifdef OS_Windows

#include <Windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/wglext.h>

#include "window_management.h"


struct Win32Window {
	HAPEngine*      engine;

	HWND            ref;

	HGLRC           renderingContext;
	HDC             deviceContext;
	HINSTANCE       application;
};


typedef struct Win32Window Win32Window;


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static PAINTSTRUCT ps;

	HAPEngine *engine = NULL;
	Win32Window *window = GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (window != NULL) engine = (*window).engine;

	switch (uMsg) {
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		window_render(NULL);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_SIZE:
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		PostMessage(hWnd, WM_PAINT, 0, 0);
		return 0;

	case WM_CHAR:
		switch (wParam) {
		case 27:
			PostQuitMessage(0);
			break;
		}
		return 0;

	case WM_CLOSE:
		if (engine) {
			(*engine).log_info(engine, "System window was closed by the user", (*engine).name);
			(*engine).isRunning = false;
		} else {
			fprintf(stderr, "A window was closed, but %s was unable to get the engine reference.", (*engine).name);
		}

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void* window_create(HAPEngine *engine, const int width, const int height) {
	const char* windowClassName = "OpenGL";

	int         pf;
	WNDCLASS    wc;
	PIXELFORMATDESCRIPTOR pfd;

	Win32Window* window = calloc(1, sizeof(Win32Window));

	if (window == NULL) {
		(*engine).log_error(engine, "Could not allocate memory for window - you may be out of memory");
		return NULL;
	}

	(*window).application = GetModuleHandle(NULL);
	(*window).engine = engine;

	wc.style = CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = (*window).application;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = windowClassName;

	if (!RegisterClass(&wc)) {
		(*engine).log_error(engine, "RegisterClass() failed: Cannot register window class.");
		return NULL;
	}

	(*window).ref = CreateWindow(
		windowClassName,
		(*engine).name,
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		10, 10,
		width, height,
		NULL, NULL,
		(*window).application,
		window
	);

	if ((*window).ref == NULL) {
		(*engine).log_error(engine, "CreateWindow() failed:  Cannot create a window");
		return NULL;
	}

	(*window).deviceContext = GetDC((*window).ref);

	/* there is no guarantee that the contents of the stack that become
	the pfd are zeroed, therefore _make sure_ to clear these bits. */
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = (byte) (WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	pfd.cColorBits = 32;

	pf = ChoosePixelFormat((*window).deviceContext, &pfd);
	if (pf == 0) {
		(*engine).log_error(engine, "Could not find a suitable pixel format.");
		return 0;
	}

	if (SetPixelFormat((*window).deviceContext, pf, &pfd) == FALSE) {
		(*engine).log_error(engine, "Could not the expected pixel format");
		return 0;
	}

	DescribePixelFormat((*window).deviceContext, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	ReleaseDC((*window).ref, (*window).deviceContext);

	(*window).deviceContext = GetDC((*window).ref);
	(*window).renderingContext = wglCreateContext((*window).deviceContext);

	SetLastError(0);
	SetWindowLongPtr((*window).ref, GWLP_USERDATA, (LONG_PTR)window);

	if (GetLastError() != 0) {
		(*engine).log_error(engine, "Failed to assign window context via SetWindowLongPtr");
	}

	ShowWindow((*window).ref, SW_SHOWNORMAL);

	return (void*) window;
}


int window_update(void* state) {
	MSG   msg;
	Win32Window *window = (Win32Window*)state;

	wglMakeCurrent((*window).deviceContext, (*window).renderingContext);

	while (PeekMessage(&msg, (*window).ref, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


void window_render(void* state) {}


void window_close(void* state) {
	Win32Window *window = (Win32Window*)state;

	wglMakeCurrent(NULL, NULL);
	ReleaseDC((*window).ref, (*window).deviceContext);
	wglDeleteContext((*window).renderingContext);

	DestroyWindow((*window).ref);
}

#endif
