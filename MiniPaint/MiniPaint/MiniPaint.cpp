// MiniPaint.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "MiniPaint.h"
#include <math.h>
#include <cstdlib>
#include <ctime>
#include "commdlg.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void ChangePenWidth(HWND , UINT , WPARAM , LPARAM );
void ChangePenColor(HWND, UINT, WPARAM, LPARAM);
void DrawPen(HWND, UINT, WPARAM, LPARAM);
void DrawLine(HWND, UINT, WPARAM, LPARAM);

HDC hdc;
HDC memoryHdc;
HDC drawingHdc;
HBITMAP memoryBitmap;
HBITMAP drawBitmap;
HPEN pen;
HBRUSH brush;
BOOL drawing=false;

unsigned int penWidth = 1;
COLORREF penColor = RGB(0, 0, 0);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MINIPAINT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	srand((unsigned int)(time(NULL)));
	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINIPAINT));

	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINIPAINT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MINIPAINT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	-Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC TempHdc;
	switch (message)
	{
	case WM_CREATE:
	{
		RECT rect;
		hdc = GetDC(hWnd);
		GetClientRect(hWnd, &rect);
		
		pen = (HPEN)GetStockObject(BLACK_PEN);
		brush = (HBRUSH)GetStockObject(NULL_BRUSH);

		memoryHdc = CreateCompatibleDC(hdc);
		memoryBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);

		drawingHdc = CreateCompatibleDC(hdc);
		drawBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);

		DeleteObject(SelectObject(drawingHdc, drawBitmap));
		DeleteObject(SelectObject(drawingHdc, (HBRUSH)WHITE_BRUSH));
		PatBlt(drawingHdc, 0, 0, rect.right, rect.bottom, PATCOPY);

		DeleteObject(SelectObject(memoryHdc, memoryBitmap));
		DeleteObject(SelectObject(memoryHdc, (HBRUSH)WHITE_BRUSH));
		PatBlt(drawingHdc, 0, 0, rect.right, rect.bottom, PATCOPY);

		DeleteObject(SelectObject(drawingHdc, pen));
		DeleteObject(SelectObject(drawingHdc, brush));
		DeleteObject(SelectObject(memoryHdc,pen));
		DeleteObject(SelectObject(memoryHdc, brush));

		break;
	}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_PENCOLOR:
		{
			CHOOSECOLOR color;
			COLORREF ccref[16];
			memset(&color, 0, sizeof(color));
			color.lStructSize = sizeof(CHOOSECOLOR);
			color.hwndOwner = NULL;
			color.lpCustColors = ccref;
			color.rgbResult = penColor;
			color.Flags = CC_RGBINIT;
			if (ChooseColor(&color))
			{
				penColor = color.rgbResult;
			}
		}
			break;
		case IDM_Line:
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		TempHdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_RBUTTONDOWN:
	{
		ChangePenColor( hWnd,  message,  wParam,  lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	{
						  
						   break;
	}
	case WM_LBUTTONUP:
	{
						
						   break;
	}
	case WM_MOUSEMOVE:
	{
					
		break;
	}
	case WM_MOUSEWHEEL:
	{
		 break; 
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void DrawLine(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int x1, x2, y1, y2;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
						   hdc = GetDC(hWnd);
						   SelectObject(hdc, pen);
						   x1 = x2 = LOWORD(lParam);
						   y1 = y2 = HIWORD(lParam);
						   MoveToEx(hdc, x1, y1, NULL);
						   LineTo(hdc, x2, y2);
						   break;
	}
	case WM_LBUTTONUP:
	{
						 hdc = GetDC(hWnd);
						 SelectObject(hdc, pen);
						 SetROP2(hdc, R2_COPYPEN);
						 x2 = LOWORD(lParam);
						 y2 = HIWORD(lParam);
						 MoveToEx(hdc, x1, y1, NULL);
						 LineTo(hdc, x2, y2);
						 break;

	}
	case WM_MOUSEMOVE:
	{ 
		hdc = GetDC(hWnd);
		SelectObject(hdc, pen);
		if (wParam& MK_LBUTTON)
		{
			SetROP2(hdc, R2_NOTXORPEN);
			MoveToEx(hdc, x1, y1, NULL);
			LineTo(hdc, x2, y2);
			x2 = LOWORD(lParam);
			y2 = HIWORD(lParam);
			MoveToEx(hdc, x1, y1, NULL);
			LineTo(hdc, x2, y2);
		}
		break;
	}
	}
	ReleaseDC(hWnd, hdc);
}


void DrawPen(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	hdc = GetDC(hWnd);
	static int xold = 0, yold = 0;
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	if (wParam& MK_LBUTTON)
	{
		SelectObject(hdc, pen);
		MoveToEx(hdc, xold, yold, NULL);
		LineTo(hdc, x, y);
		ReleaseDC(hWnd, hdc);
	}
	xold = x; yold = y;
}

void ChangePenColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	hdc = GetDC(hWnd);
	DeleteObject(pen);
	penColor = RGB(rand() % 256, rand() % 256, rand() % 256);
	pen = CreatePen(PS_SOLID, penWidth, penColor);
	ReleaseDC(hWnd, hdc);
}
void ChangePenWidth(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	hdc = GetDC(hWnd);
	int delta = GET_WHEEL_DELTA_WPARAM(wParam);
	if (delta > 0)
	{
		if (wParam & MK_SHIFT)
			penWidth = penWidth + 2;
		else
			penWidth++;
	}
	else
	{
		if (penWidth > 1)
		{
			if ((wParam & MK_SHIFT) & (penWidth>3))
				penWidth = penWidth - 2;
			else
				penWidth--;
		}
	}
	DeleteObject(pen);
	pen = CreatePen(PS_SOLID, penWidth, penColor);
	ReleaseDC(hWnd, hdc);
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
