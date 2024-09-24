//   ���δ���1  ���ڱ���ʹ��λͼ��ˢ��ָ��͸��ɫ  
//By MoreWindows-(http://blog.csdn.net/MoreWindows)  
#include <windows.h>  

const WCHAR szAppName[] = L"���δ���1 MoreWindows-(http://blog.csdn.net/MoreWindows)";

/*
* ��������: GetWindowSize
* ��������: �õ����ڵĿ��
* hwnd      ���ھ��
* pnWidth   ���ڿ�
* pnHeight  ���ڸ�
*/
void GetWindowSize(HWND hwnd, int *pnWidth, int *pnHeight);


/*
* ��������: InitBitmapWindow
* ��������: λͼ���ڳ�ʼ��
* hinstance ����ʵ��
* hBitmap   λͼ���
* nCmdshow  ��ʾ��ʽ-��ShowWindow�����ĵڶ���������ͬ
*/
BOOL InitBitmapWindow(HINSTANCE hinstance, HBITMAP hBitmap, int nCmdshow);

// λͼ������Ϣ������  
LRESULT CALLBACK BitmapWindowWndPrco(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParm);


int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR     lpCmdLine,
	int       nCmdShow)
{
	//���ô��ڱ�����ˢΪͼƬ��ˢ����ָ��͸����ɫ�����Դ���͸������  
	HBITMAP  hBitmap;
	hBitmap = (HBITMAP)LoadImage(NULL, L"Untitled.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBitmap == NULL)
	{
		MessageBox(NULL, L"λͼ����ʧ��", L"Error", MB_ICONERROR);
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
	wndclass.hbrBackground = CreatePatternBrush(hBitmap);//λͼ��ˢ  
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
		// ���÷ֲ�����  
		SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		// ����͸��ɫ  
		COLORREF clTransparent = RGB(255, 0, 255);
		SetLayeredWindowAttributes(hwnd, clTransparent, 0, LWA_COLORKEY);
	}
	return 0;


	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE: //����Esc��ʱ�˳�  
			SendMessage(hwnd, WM_DESTROY, 0, 0);
			return 0;
		}
		break;


	case WM_LBUTTONDOWN: //�����������ʱ������ҷ����  
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