#include <Windows.h>

// 代码原地址
// https://blog.51cto.com/nowpaper/712620

struct rgn_bmp
{
	// 需要镂空的多边形
	HRGN hregion;
	// 需要把这个画到窗口
	HBITMAP hskin;
};

LRESULT WindowOnCreate(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CREATESTRUCT *pcs;
	struct rgn_bmp *pbmp;
	LRESULT result = 0;

	pcs = (CREATESTRUCT *)lparam;
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(pbmp = (struct rgn_bmp *)pcs->lpCreateParams));

	HBITMAP hskin;
	HBITMAP hmask;
	int width, height;

	hskin = (HBITMAP)LoadImage(NULL, L"Untitled.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hmask = (HBITMAP)LoadImage(NULL, L"Mask.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	HRGN line_region;	// 组合多边形的单位, 我们用它来解决多多边形组合
	HRGN hregion;	// 需要镂空的多边形
	int first = 0;		// 第一次多边形创建标志, 在那之前我们并不知道多边形是否存在

	BITMAP bmp;
	unsigned int x, y;

	GetObject(hmask, sizeof(BITMAP), &bmp);
	HDC hdc = CreateCompatibleDC(NULL);
	SelectObject(hdc, hmask);	// 将 mask 的数据放入 hdc, 方便取色计算
	width = bmp.bmWidth;
	height = bmp.bmHeight;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			// 添加非背景
			if (GetPixel(hdc, x, y) != RGB(255, 0, 255))
			{
				if (!first)
				{
					first = 1;
					hregion = CreateRectRgn(x, y, x + 1, y + 1);
				}
				else
				{
					line_region = CreateRectRgn(x, y, x + 1, y + 1);
					CombineRgn(hregion, hregion, line_region, RGN_OR);
					DeleteObject(line_region);
				}
			}
		}
	}
	DeleteDC(hdc);
	SetWindowRgn(hwnd, hregion, 1);	// 设置新的多边形区域为窗口区域

	DeleteObject(hmask);

	pbmp->hregion = hregion;
	pbmp->hskin = hskin;

	return(result);
}
VOID WindowOnClose(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//
}
VOID WindowOnDestroy(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	struct rgn_bmp *pbmp;
	LRESULT result = 0;

	pbmp = (struct rgn_bmp *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	if (pbmp->hskin)
	{
		//
	}
}
VOID WindowOnSize(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	HDWP hdwp;
	int width, height;

	width = LOWORD(lparam);
	height = HIWORD(lparam);

	//hdwp = BeginDeferWindowPos(1);

	//DeferWindowPos(hdwp, hlistview, NULL, 4, 4, width - 8, height - 8, SWP_NOZORDER);

	//EndDeferWindowPos(hdwp);
}

LRESULT WindowOnPaint(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	struct rgn_bmp *pbmp;
	PAINTSTRUCT ps;
	RECT *prect;
	HDC hdc;
	LRESULT result = 0;

	pbmp = (struct rgn_bmp *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	if (pbmp->hskin)
	{
		prect = &ps.rcPaint;

		hdc = BeginPaint(hwnd, &ps);
		if (hdc)
		{
			HDC hdc1;
			HGDIOBJ hgdiobj;

			hdc1 = CreateCompatibleDC(hdc);
			hgdiobj = SelectObject(hdc1, pbmp->hskin);
			BitBlt(hdc, prect->left, prect->top, prect->right - prect->left, prect->bottom - prect->top,
				hdc1, prect->left, prect->top, SRCCOPY);
			SelectObject(hdc1, hgdiobj);
			DeleteDC(hdc1);

			EndPaint(hwnd, &ps);
		}
	}

	return(result);
}
LRESULT WindowOnLButtonDown(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;

	// 让无边框窗口可以使用鼠标拖着移动
	ReleaseCapture();
	SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);

	return(result);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;

	switch (message)
	{
	case WM_CREATE:
		WindowOnCreate(hwnd, wparam, lparam);
		break;
	case WM_CLOSE:
		WindowOnClose(hwnd, wparam, lparam);
		break;
	case WM_DESTROY:
		WindowOnDestroy(hwnd, wparam, lparam);

		PostQuitMessage(0);
		break;
	case WM_SIZE:
		WindowOnSize(hwnd, wparam, lparam);
		break;
	case WM_PAINT:
		WindowOnPaint(hwnd, wparam, lparam);
		break;
	case WM_LBUTTONDOWN:
		WindowOnLButtonDown(hwnd, wparam, lparam);
		break;
	default:
		break;
	}

	if (result == 0)
	{
		result = DefWindowProc(hwnd, message, wparam, lparam);
	}

	return(result);
}

HWND CreateMainWindow(HINSTANCE hinstance, struct rgn_bmp *pbmp)
{
	MSG  msg;
	HWND hwnd;
	WNDCLASSEX wc;
	int width, height;

	width = 200;
	height = 445;

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"RegionWindowClass";

	RegisterClassEx(&wc);

	// 先不加 WS_VISIBLE 标志
	hwnd = CreateWindowEx(0,
		wc.lpszClassName,   // window class name
		NULL,				// window caption
		WS_POPUP,      // window style
		(GetSystemMetrics(SM_CXSCREEN) - width) / 2,            // initial x position
		(GetSystemMetrics(SM_CYSCREEN) - height) / 2,            // initial y position
		width,            // initial x size
		height,            // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hinstance,                // program instance handle
		(LPVOID)pbmp);                    // creation parameters

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return(hwnd);
}

int wmain(int argc, WCHAR *argv[])
{
	struct rgn_bmp pbmp[1];

	CreateMainWindow((HINSTANCE)GetModuleHandle(NULL), pbmp);

	return(0);
}