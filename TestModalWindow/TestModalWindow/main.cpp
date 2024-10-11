#include <Windows.h>
#include <CommCtrl.h>

#include <stdio.h>

struct modal_dialog
{
	COLORREF colors[16];

	HWND hbutton;

	int closed;
};

struct main_window
{
	struct modal_dialog pdlg[1];

	HINSTANCE hinstance;

	HWND hbutton;
};

HWND GetBottomHwnd(HWND hwnd)
{
	HWND hparent;

	while (hparent = GetParent(hwnd))
	{
		hwnd = hparent;
	}
	return(hwnd);
}
HWND GetParentUntil(HWND hwnd, HWND hend)
{
	HWND hparent;

	while (hwnd != hend && (hparent = GetParent(hwnd)))
	{
		hwnd = hparent;
	}
	return(hwnd);
}

void SetFont(HWND hwnd, HFONT hfont)
{
	if (hfont == NULL)
	{
		hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	}
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
}
void SetGuiFont(HWND hwnd)
{
	SetFont(hwnd, NULL);
}

LRESULT DialogOnCreate(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CREATESTRUCT *pcs;
	struct modal_dialog *pdlg;
	HINSTANCE hinstance;
	HWND hbutton;
	LRESULT result = 0;

	pcs = (CREATESTRUCT *)lparam;
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(pdlg = (struct modal_dialog *)pcs->lpCreateParams));

	hinstance = pcs->hInstance;

	pdlg->hbutton =
		hbutton = CreateWindowEx(WS_EX_LTRREADING, WC_BUTTON, L"选择字体", WS_CHILD | WS_VISIBLE, 4, 4, 90, 20, hwnd, NULL, hinstance, NULL);
	SetGuiFont(hbutton);

	return(result);
}
VOID DialogOnClose(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//
}
VOID DialogOnDestroy(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	struct modal_dialog *pdlg;

	pdlg = (struct modal_dialog *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	pdlg->closed = 1;
}
VOID DialogOnSize(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	HDWP hdwp;
	int width, height;

	width = LOWORD(lparam);
	height = HIWORD(lparam);

	//hdwp = BeginDeferWindowPos(1);

	//DeferWindowPos(hdwp, hlistview, NULL, 4, 4, width - 8, height - 8, SWP_NOZORDER);

	//EndDeferWindowPos(hdwp);
}

VOID DialogOnPaint(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(hwnd, &ps);
	if (hdc)
	{
		//

		EndPaint(hwnd, &ps);
	}
}

VOID DialogOnCommand(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	struct modal_dialog *pdlg;

	pdlg = (struct modal_dialog *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (HIWORD(wparam))
	{
	case BN_CLICKED:
		if (lparam == (LPARAM)pdlg->hbutton)
		{
			CHOOSECOLOR cc;

			memset(&cc, 0, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			// 其他成员可以根据需要进行设置，例如cc.hwndOwner, cc.rgbResult等
			cc.hwndOwner = hwnd; // hwnd是父窗口的句柄  
			cc.rgbResult = RGB(255, 0, 0); // 设置初始颜色为红色可以根据需要设置其他成员，如Flags, lpCustColors等
			cc.lpCustColors = (LPDWORD)pdlg->colors;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;
			if (ChooseColor(&cc)) {
				// 用户选择了颜色，cc.rgbResult包含了用户选择的颜色的RGB值  
				COLORREF selectedColor = cc.rgbResult;
				// 可以使用selectedColor进行后续操作，如设置窗口背景色等  
			}
			else {
				// 用户取消了对话框或发生了错误  
			}
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;

	switch (message)
	{
	case WM_CREATE:
		DialogOnCreate(hwnd, wparam, lparam);
		break;
	case WM_CLOSE:
		DialogOnClose(hwnd, wparam, lparam);
		break;
	case WM_DESTROY:
		DialogOnDestroy(hwnd, wparam, lparam);

		//PostQuitMessage(0);
		break;
	case WM_SIZE:
		DialogOnSize(hwnd, wparam, lparam);
		break;
	case WM_PAINT:
		//DialogOnPaint(hwnd, wparam, lparam);
		break;
	case WM_COMMAND:
		DialogOnCommand(hwnd, wparam, lparam);
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

HWND CreateModalDialog(HINSTANCE hinstance, HWND hparent, struct modal_dialog *pdlg)
{
	HWND hwnd;
	WNDCLASSEX wc;
	int width, height;

	width = 400;
	height = 300;

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DlgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"MyModalDialogClass";

	RegisterClassEx(&wc);

	hwnd = CreateWindowEx(0,
		wc.lpszClassName,   // window class name
		NULL,				// window caption
		WS_CAPTION | WS_SYSMENU | WS_VISIBLE,      // window style
		(GetSystemMetrics(SM_CXSCREEN) - width) / 2,            // initial x position
		(GetSystemMetrics(SM_CYSCREEN) - height) / 2,            // initial y position
		width,            // initial x size
		height,            // initial y size
		hparent,                     // parent window handle
		NULL,                     // window menu handle
		hinstance,                // program instance handle
		(LPVOID)pdlg);                    // creation parameters

	UpdateWindow(hwnd);

	return(hwnd);
}

LRESULT WindowOnCreate(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CREATESTRUCT *pcs;
	struct main_window *pwin;
	HINSTANCE hinstance;
	HWND hbutton;
	LRESULT result = 0;

	pcs = (CREATESTRUCT *)lparam;
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(pwin = (struct main_window *)pcs->lpCreateParams));

	pwin->hinstance =
		hinstance = pcs->hInstance;

	pwin->hbutton =
		hbutton = CreateWindowEx(WS_EX_LTRREADING, WC_BUTTON, L"生成模窗口", WS_CHILD | WS_VISIBLE, 4, 4, 90, 20, hwnd, NULL, hinstance, NULL);
	SetGuiFont(hbutton);

	return(result);
}
VOID WindowOnClose(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//
}
VOID WindowOnDestroy(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
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

VOID WindowOnPaint(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	HDWP hdwp;
	int width, height;
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(hwnd, &ps);
	if (hdc)
	{
		//

		EndPaint(hwnd, &ps);
	}
}

LRESULT WindowOnCommand(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	struct main_window *pwin;
	HWND hdlg;
	LRESULT result = 0;

	pwin = (struct main_window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (HIWORD(wparam))
	{
	case BN_CLICKED:
		if (lparam == (LPARAM)pwin->hbutton)
		{
			struct modal_dialog *pdlg = pwin->pdlg;

			pdlg->closed = 0;

			EnableWindow(hwnd, 0);

			// hwnd 是否传递进去都一样
			hdlg = CreateModalDialog(pwin->hinstance, hwnd, pdlg);

			//DefWindowProc(hwnd, WM_COMMAND, wparam, lparam);

			result = 1;

			MSG  msg;

			//while (GetMessage(&msg, NULL, 0, 0))
			//{
			//	TranslateMessage(&msg);
			//	DispatchMessage(&msg);
			//}
			while (!pdlg->closed)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_PAINT || 
						GetParentUntil(msg.hwnd, hdlg) == hdlg)
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
			}

			EnableWindow(hwnd, 1);
		}
		break;
	default:
		break;
	}

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
		//WindowOnPaint(hwnd, wparam, lparam);
		break;
	case WM_ERASEBKGND:
		//listbox_on_erasebkgnd(hwnd, wparam, lparam);
		//HostListOnEraseBackground(pd, hwnd);
		result = 1;
		if (1)
		{
			return(0);
		}
		break;
	case WM_COMMAND:
		result = WindowOnCommand(hwnd, wparam, lparam);
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

HWND CreateMainWindow(HINSTANCE hinstance, struct main_window *pwin)
{
	HWND hwnd;
	WNDCLASSEX wc;
	int width, height;

	width = 640;
	height = 480;

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
	wc.lpszClassName = L"MyMainWindowClass";

	RegisterClassEx(&wc);

	hwnd = CreateWindowEx(0,
		wc.lpszClassName,   // window class name
		NULL,				// window caption
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,      // window style
		(GetSystemMetrics(SM_CXSCREEN) - width) / 2,            // initial x position
		(GetSystemMetrics(SM_CYSCREEN) - height) / 2,            // initial y position
		width,            // initial x size
		height,            // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hinstance,                // program instance handle
		(LPVOID)pwin);                    // creation parameters

	UpdateWindow(hwnd);

	return(hwnd);
}

int wmain(int argc, WCHAR *argv[])
{
	HINSTANCE hinstance;
	struct main_window pwin[1];
	HWND hwnd;

	hinstance = (HINSTANCE)GetModuleHandle(NULL);

	hwnd = CreateMainWindow(hinstance, pwin);

	MSG  msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return(0);
}