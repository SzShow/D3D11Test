
#include "dx11.h"
#include <Windows.h>

LRESULT CALLBACK WndProc(
	HWND hwnd,
	UINT msg,
	WPARAM wp,
	LPARAM lp) {

	switch (msg)
	{
	case WM_DESTROY:
		MessageBox(hwnd, TEXT("tanasin..."),
			TEXT("Kitty"), MB_ICONINFORMATION);
		PostQuitMessage(0);
		return 0;
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wp, lp);

}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ int nCmdShow) {
	
	WNDCLASS wndClass;
	HWND hWnd;
	CD3D11 cd3d11;
	MSG msg;

	//Windowê∂ê¨ïî

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszClassName = TEXT("hoge");
	wndClass.lpszMenuName = NULL;

	if (!RegisterClass(&wndClass)) {
		return -1;
	}

	hWnd = CreateWindow(
		TEXT("hoge"), TEXT("DirectX_Test"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100, 800, 600, 
		NULL, NULL, hInstance, NULL
	);

	if (hWnd == NULL) 
		return 0;

	cd3d11.Create(hWnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		cd3d11.Render();
		DispatchMessage(&msg);
	}

	cd3d11.Release();

	return msg.wParam;

}
