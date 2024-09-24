//   异形窗口1  窗口背景使用位图画刷再指定透明色  
//By MoreWindows-(http://blog.csdn.net/MoreWindows)  
#include <windows.h>  

const WCHAR szAppName[] = L"异形窗口1 MoreWindows-(http://blog.csdn.net/MoreWindows)";

/*
* 函数名称: GetWindowSize
* 函数功能: 得到窗口的宽高
* hwnd      窗口句柄
* pnWidth   窗口宽
* pnHeight  窗口高
*/
void GetWindowSize(HWND hwnd, int *pnWidth, int *pnHeight);


/*
* 函数名称: InitBitmapWindow
* 函数功能: 位图窗口初始化
* hinstance 进程实例
* hBitmap   位图句柄
* nCmdshow  显示方式-与ShowWindow函数的第二个参数相同
*/
BOOL InitBitmapWindow(HINSTANCE hinstance, HBITMAP hBitmap, int nCmdshow);

// 位图窗口消息处理函数  
LRESULT CALLBACK BitmapWindowWndPrco(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParm);


int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR     lpCmdLine,
	int       nCmdShow)
{
	//设置窗口背景画刷为图片画刷，再指定透明颜色即可以创建透明区域。  
	HBITMAP  hBitmap;
	hBitmap = (HBITMAP)LoadImage(NULL, L"Untitled.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBitmap == NULL)
	{
		MessageBox(NULL, L"位图加载失败", L"Error", MB_ICONERROR);
		return 0;
	}
	if (!InitBitmapWindow(hInstance, hBitmap, nCmdShow))
		return 0;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(hBitmap);

	return msg.wParam;
}


BOOL InitBitmapWindow(HINSTANCE hinstance, HBITMAP hBitmap, int nCmdshow)
{
	HWND hwnd;
	WNDCLASS wndclass;

	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	wndclass.lpfnWndProc = BitmapWindowWndPrco;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hinstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = CreatePatternBrush(hBitmap);//位图画刷  
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, L"Program Need Windows NT!", L"Error", MB_ICONERROR);
		return FALSE;
	}

	BITMAP bm;
	GetObject(hBitmap, sizeof(bm), &bm);
	hwnd = CreateWindowEx(WS_EX_TOPMOST,
		szAppName,
		szAppName,
		WS_POPUP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		bm.bmWidth,
		bm.bmHeight,
		NULL,
		NULL,
		hinstance,
		NULL);
	if (hwnd == NULL)
		return FALSE;

	ShowWindow(hwnd, nCmdshow);
	UpdateWindow(hwnd);

	return TRUE;
}

LRESULT CALLBACK BitmapWindowWndPrco(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParm)
{
	static HDC s_hdcMem;
	static HBRUSH s_hBackBrush;

	switch (message)
	{
	case WM_CREATE:
	{
		// 设置分层属性  
		SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		// 设置透明色  
		COLORREF clTransparent = RGB(255, 0, 255);
		SetLayeredWindowAttributes(hwnd, clTransparent, 0, LWA_COLORKEY);
	}
	return 0;


	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE: //按下Esc键时退出  
			SendMessage(hwnd, WM_DESTROY, 0, 0);
			return 0;
		}
		break;


	case WM_LBUTTONDOWN: //当鼠标左键点击时可以拖曳窗口  
		PostMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
		return 0;


	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParm);
}

void GetWindowSize(HWND hwnd, int *pnWidth, int *pnHeight)
{
	RECT rc;
	GetWindowRect(hwnd, &rc);
	*pnWidth = rc.right - rc.left;
	*pnHeight = rc.bottom - rc.top;
}