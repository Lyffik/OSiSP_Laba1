#include "stdafx.h"
#include "MiniPaint.h"
#include <windows.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <tchar.h>
#include "commdlg.h"
#include "Initialization.h"
#include "Shapes.h"
#include <string>

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > String;

enum Tools { PENCIL, LINE, RECTANGLE, ELLIPSE, POLY,TEXT };
#define MAX_LOADSTRING 100


HINSTANCE hInst;								
TCHAR szTitle[MAX_LOADSTRING];					
TCHAR szWindowClass[MAX_LOADSTRING];			


ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


HDC hdc;
HDC memoryHdc=0;
HDC drawingHdc=0;
HDC printDc=0;
HBITMAP printBitmap;
HBITMAP memoryBitmap;
HBITMAP drawBitmap;
HPEN pen;
HBRUSH brush;
BOOL drawing=false;
draw drawMode;
Shape* shape=NULL;
Tools ToolId = PENCIL;
POINT prevCoord;
unsigned int penWidth = 1;
COLORREF penColor = RGB(0, 0, 0);
HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
CHOOSECOLOR cc;
COLORREF acrCustClr[16];
INT prevX = -1, prevY = -1, startX = -1, startY = -1; 
BOOL isPolyLine; 
String str;
PRINTDLG pd;
DOCINFO di;
HANDLE       hOld;
BOOL pan=false;
double scale=1;
int xBegin = 0, yBegin = 0;
int offsetX=0;
int offsetY=0;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MINIPAINT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	srand((unsigned int)(time(NULL)));
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINIPAINT));

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


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; 

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


void SaveMetaFile(HWND hWnd, HDC windowDC)
{
	TCHAR szFilters[] = _T("Metafile (*.emf)\0\0");
	TCHAR szFilePathName[_MAX_PATH] = _T("");
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = szFilters;
	ofn.lpstrFile = szFilePathName; // This will hold the file name
	ofn.lpstrDefExt = _T("emf");
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = _T("Save File");
	ofn.Flags = OFN_OVERWRITEPROMPT;
	ofn.lpstrInitialDir = _T("D:\\");
	// Open the file save dialog, and choose the file name
	GetSaveFileName(&ofn);

	RECT rect;
	HGDIOBJ hOldBrush;
	HBRUSH hBrush;
	HDC hDCMeta;
	HENHMETAFILE hemf;

	GetClientRect(hWnd, &rect);
	hDCMeta = CreateEnhMetaFile(windowDC, ofn.lpstrFile, NULL, NULL);

	hBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	hOldBrush = SelectObject(hDCMeta, hBrush);

	BitBlt(hDCMeta, 0, 0, rect.right, rect.bottom, windowDC, 0, 0, SRCCOPY);

	SelectObject(hDCMeta, hOldBrush);
	hemf = CloseEnhMetaFile(hDCMeta);
	DeleteEnhMetaFile(hemf);
}

void OpenMetaFile(HWND hWnd, HDC memDC, WPARAM wParam, LPARAM lParam)
{

	TCHAR szFilters[] = _T("Metafile (*.emf)\0\0");
	TCHAR szFilePathName[_MAX_PATH] = _T("");
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = szFilters;
	ofn.lpstrFile = szFilePathName; // This will hold the file name
	ofn.lpstrDefExt = _T("emf");
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = _T("Open File");
	ofn.Flags = OFN_OVERWRITEPROMPT;
	ofn.lpstrInitialDir = _T("D:\\");
	// Open the file save dialog, and choose the file name
	GetOpenFileName(&ofn);

	RECT rect;
	HENHMETAFILE hemf;

	// open metafile
	hemf = GetEnhMetaFile(ofn.lpstrFile);
	GetClientRect(hWnd, &rect);
	// Draw the picture. 
	PlayEnhMetaFile(memDC, hemf, &rect);
	PostMessage(hWnd, WM_PAINT, wParam, lParam);
	DeleteEnhMetaFile(hemf);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	static HDC TempHdc;
static	RECT rect;
	switch (message)
	{
	case WM_CREATE:
	{
					  hdc = GetDC(hWnd);
					  initializeDcs(hWnd, hdc, drawingHdc, drawBitmap, memoryHdc, memoryBitmap);
					  rect.top = 0;
					  rect.left = 0;
					  rect.right = 2500;
					  rect.bottom = 2500;
					  break;
	}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_PENCOLOR:
		{
							 ZeroMemory(&cc, sizeof(CHOOSECOLOR));
							 cc.lStructSize = sizeof(CHOOSECOLOR);
							 cc.hwndOwner = hWnd;
							 cc.lpCustColors = (LPDWORD)acrCustClr;
							 cc.Flags = CC_FULLOPEN | CC_RGBINIT;

							 if (ChooseColor(&cc) == TRUE)
							 {
								 HPEN pen;
								 Shape::penColor = cc.rgbResult;
								 pen = CreatePen(Shape::penStyle, Shape::penWidth, Shape::penColor);
								 DeleteObject(SelectObject(drawingHdc, pen));
								 DeleteObject(SelectObject(memoryHdc, pen));
							 }
							 break;
		}
			break;
		case IDM_PENCIL:
			ToolId = PENCIL;
			break;
		case IDM_LINE:
			ToolId = LINE;
			break;
		case IDM_RECT:
			ToolId = RECTANGLE;
			break;
		case IDM_ELLIPSE:
			ToolId = ELLIPSE;
			break;
		case IDM_POLYGON:
			isPolyLine = FALSE;
			prevX = -1;
			prevY = -1;
			ToolId = POLY;
			break;
		case IDM_POLYLINE:
			isPolyLine = TRUE;
			prevX = -1;
			prevY = -1;
			ToolId = POLY;
			break;
		case ID_TEXT:
			ToolId = TEXT;
			break;
		case ID_NEW:
			
			hdc = GetDC(hWnd);
			memoryHdc = 0;
			drawingHdc = 0;
			DeleteObject(SelectObject(drawingHdc, pen));
			DeleteObject(SelectObject(drawingHdc, brush));
			DeleteObject(SelectObject(memoryHdc, pen));
			DeleteObject(SelectObject(memoryHdc, brush));
			prevX = -1;
			prevY = -1;
			startX = -1;
			startY = -1;
			offsetX = 0;
			offsetY = 0;
			xBegin = 0;
			yBegin = 0;
			scale = 1;
			initializeDcs(hWnd, hdc, drawingHdc, drawBitmap, memoryHdc, memoryBitmap);
			GetClientRect(hWnd, &rect);
			FillRect(hdc, &rect, WHITE_BRUSH);
			HPEN pen;
			Shape::penWidth += GET_WHEEL_DELTA_WPARAM(wParam) / 20;
			if (Shape::penWidth < 0)
				Shape::penWidth = 0;
			pen = CreatePen(Shape::penStyle, Shape::penWidth, Shape::penColor);
			DeleteObject(SelectObject(drawingHdc, pen));
			DeleteObject(SelectObject(memoryHdc, pen));
			RECT tempRect;
			GetClientRect(hWnd, &tempRect);
			tempRect.left = offsetX;
			tempRect.top = offsetY;
			tempRect.right *= scale;
			tempRect.bottom *= scale;
			BitBlt(drawingHdc, tempRect.left, tempRect.top, tempRect.right, tempRect.bottom, memoryHdc, 0, 0, SRCCOPY);
			drawMode = BUFFER;
		InvalidateRect(hWnd, NULL, FALSE);
			
			break;
		case ID_OPEN:
			OpenMetaFile(hWnd, memoryHdc, wParam, lParam);
			break;
		case ID_SAVE:
			SaveMetaFile(hWnd, hdc);
			break;
		case ID_PRINT:
			ZeroMemory(&pd, sizeof(pd));
			pd.lStructSize = sizeof(pd);
			pd.hwndOwner = hWnd;
			pd.hDevMode = NULL; 
			pd.hDevNames = NULL; 
			pd.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
			pd.nCopies = 1;
			pd.nFromPage = 0xFFFF;
			pd.nToPage = 0xFFFF;
			pd.nMinPage = 1;
			pd.nMaxPage = 0xFFFF;


			if (PrintDlg(&pd) == TRUE)
			{
				int Rx = GetDeviceCaps(pd.hDC, LOGPIXELSX);
				int Ry = GetDeviceCaps(pd.hDC, LOGPIXELSY);
				di.cbSize = sizeof(DOCINFO);
				di.lpszDocName = (LPCWSTR)"Print Picture";
				di.fwType = NULL;
				di.lpszDatatype = NULL;
				di.lpszOutput = NULL;
				StartDoc(pd.hDC, &di);
				StartPage(pd.hDC);
				RECT rect;
				GetClientRect(hWnd, &rect);
				printDc = CreateCompatibleDC(hdc);
				printBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
				int Rx1 = GetDeviceCaps(printDc, LOGPIXELSX);
				int Ry1 = GetDeviceCaps(printDc, LOGPIXELSY);
				hOld = SelectObject(printDc, printBitmap);
				FillRect(printDc, &rect, WHITE_BRUSH);
				StretchBlt(printDc, 0, 0, (int)(rect.right*scale), (int)(rect.bottom*scale),
					memoryHdc, xBegin, yBegin, rect.right, rect.bottom, SRCCOPY);
				SelectObject(printDc, (HBRUSH)GetStockObject(NULL_BRUSH));
				SelectObject(printDc, (HPEN)GetStockObject(BLACK_PEN));
				Rectangle(printDc, 0, 0, (int)(rect.right*scale), (int)(rect.bottom*scale));
				StretchBlt(pd.hDC, 0, 0, (int)((float)(0.5*rect.right*((float)(Rx / Rx1)))), (int)((float)(0.5*rect.bottom*((float)(Ry / Ry1)))),
					memoryHdc, 0, 0, rect.right, rect.bottom, SRCCOPY);
				SelectObject(printDc, hOld);
				DeleteObject(printBitmap);
				DeleteDC(printDc);
				EndPage(pd.hDC);
				EndDoc(pd.hDC);
				DeleteDC(pd.hDC);
			}
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
	case WM_RBUTTONDOWN:
	{
		break;
	}
	case WM_LBUTTONDOWN:
	{
						   int x = (short)LOWORD(lParam)*scale+offsetX;
						   int y = (short)HIWORD(lParam)*scale + offsetY;
						   if (ToolId == PENCIL)
						   {
							   shape = new Pencil(x, y);
							   drawMode = BUFFER;
						   }
						   else
						   {
							   switch (ToolId)
							   {
							   case LINE:
								   shape = new Line(x, y);
								   break;

							   case RECTANGLE:
								   shape = new CustomRectangle(x, y);
								   break;

							   case ELLIPSE:
								   shape = new CustomEllipse(x, y);
								   break;
							   case POLY:
								   if (prevX == -1 && prevY == -1)
								   {
									   prevX = x;
									   prevY = y;
									   startX = prevX;
									   startY = prevY;
								   }
								   shape = new Line(prevX, prevY);
								   break;
							   case TEXT:
								   prevX = x;
								   prevY = y;
								   str.clear();
								   break;
							   }
							   drawMode = CURRENT;
						   }
						   SetCapture(hWnd);
	}
	case WM_MOUSEMOVE:
	{
						 int x = (short)LOWORD(lParam)*scale + offsetX;
						 int y = (short)HIWORD(lParam)*scale + offsetY;
						 prevCoord.x = x;
						 prevCoord.y = y;
						 RECT tempRect;
						 GetClientRect(hWnd, &tempRect);
						 tempRect.left = offsetX;
						 tempRect.top = offsetY;
						 tempRect.right *= scale;
						 tempRect.bottom *= scale;
						 BitBlt(drawingHdc, 0, 0, tempRect.right, tempRect.bottom, memoryHdc, 0, 0, SRCCOPY);
						 if (wParam & MK_LBUTTON)
						 {
							 if (shape)
							 {
								 if (ToolId == PENCIL)
								 {
									 shape->draw(memoryHdc,x, y);
									 drawMode = BUFFER;
								 }
								 else
								 {
									 BitBlt(drawingHdc, 0, 0, tempRect.right, tempRect.bottom, memoryHdc, 0, 0, SRCCOPY);
									 shape->draw(drawingHdc, x, y);
									 drawMode = CURRENT;
								 }
							 }
						 }
						 else
						 {
							 BitBlt(drawingHdc, 0, 0, tempRect.right, tempRect.bottom, memoryHdc, 0, 0, SRCCOPY);
							 MoveToEx(drawingHdc, prevCoord.x, prevCoord.y, NULL);
							 LineTo(drawingHdc, prevCoord.x, prevCoord.y);
							 drawMode = BUFFER;
							 InvalidateRect(hWnd, NULL, FALSE);
						 }
						 InvalidateRect(hWnd, NULL, FALSE);
						 break;
	}
	case WM_LBUTTONUP:
	{
						 ReleaseCapture();
						 int x = (short)LOWORD(lParam)*scale + offsetX;
						 int y = (short)HIWORD(lParam)*scale + offsetY;
						 if ((ToolId != PENCIL) && shape != NULL)
						 {
							 if (prevX != -1 && prevY != -1)
							 {
								 prevX = x;
								 prevY = y;
							 }
							 GetClientRect(hWnd, &rect);
							 shape->draw(memoryHdc, x, y);
							 drawMode = BUFFER;
							 InvalidateRect(hWnd, NULL, FALSE);
						 }
						 delete shape;
						 shape = NULL;
						 break;
	}
	case WM_LBUTTONDBLCLK:
		if (ToolId == 4)
		{
			int x = (short)LOWORD(lParam)*scale + offsetX;
			int y = (short)HIWORD(lParam)*scale + offsetY;
			shape = new Line(prevX, prevY);
			ReleaseCapture();
			GetClientRect(hWnd, &rect);
			if (!isPolyLine)
				shape->draw(memoryHdc, startX, startY);
			else
				shape->draw(memoryHdc, prevX, prevY);
			drawMode = BUFFER;
			InvalidateRect(hWnd, NULL, FALSE);
			prevX = -1;
			prevY = -1;
			startX = -1;
			startY = -1;
			delete shape;
			shape = NULL;
		}
		break;
	case WM_PAINT:
		TempHdc = BeginPaint(hWnd, &ps);
		RECT tempRect;
		GetClientRect(hWnd, &tempRect);
		tempRect.left = offsetX;
		tempRect.top = offsetY;
		tempRect.right *= scale;
		tempRect.bottom *= scale;
		switch (drawMode)
		{
			case CURRENT:

					StretchBlt(hdc, 0, 0, rect.right, rect.bottom, drawingHdc, tempRect.left, tempRect.top, tempRect.right, tempRect.bottom, SRCCOPY);
				break;

			case BUFFER:
			
				StretchBlt(hdc, 0, 0, rect.right, rect.bottom, memoryHdc,tempRect.left, tempRect.top, tempRect.right, tempRect.bottom, SRCCOPY);
				break;
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_MOUSEWHEEL:
	{
						  GetClientRect(hWnd, &rect);
						  FillRect(hdc, &rect, WHITE_BRUSH);
						  if (GET_KEYSTATE_WPARAM(wParam) == MK_CONTROL)
						  {
							  drawMode = BUFFER;
							  offsetX += GET_WHEEL_DELTA_WPARAM(wParam);

						  }
						  else if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
							{
								  if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
								  {
									  scale /= 1.25;
								  }
								  else
								  {
									  scale *= 1.25;
								  }
							  }
							else if (GET_KEYSTATE_WPARAM(wParam) == (MK_SHIFT | MK_CONTROL))
								  {
										drawMode = BUFFER;
									  offsetY += GET_WHEEL_DELTA_WPARAM(wParam);
								  }
							else
							{

									  HPEN pen;
									  Shape::penWidth += GET_WHEEL_DELTA_WPARAM(wParam) / 20;
									  if (Shape::penWidth < 0)
										  Shape::penWidth = 0;
									  pen = CreatePen(Shape::penStyle, Shape::penWidth, Shape::penColor);
									  DeleteObject(SelectObject(drawingHdc, pen));
									  DeleteObject(SelectObject(memoryHdc, pen));
									  RECT tempRect;
									  GetClientRect(hWnd, &tempRect);
									  tempRect.left = offsetX;
									  tempRect.top = offsetY;
									  tempRect.right *= scale;
									  tempRect.bottom *= scale;
									  BitBlt(drawingHdc, tempRect.left, tempRect.top, tempRect.right, tempRect.bottom, memoryHdc, 0, 0, SRCCOPY);
									  MoveToEx(drawingHdc, prevCoord.x, prevCoord.y, NULL);
									  LineTo(drawingHdc, prevCoord.x, prevCoord.y);
									  drawMode = BUFFER;
							 
						}
						  InvalidateRect(hWnd, NULL, FALSE);
						  break;
	}
	case	WM_ERASEBKGND:
		GetClientRect(hWnd, &rect);
		FillRect(hdc, &rect, WHITE_BRUSH);
		break;
	case WM_CHAR:
		if (ToolId == 5)
		{

			str += (TCHAR)wParam;
			GetClientRect(hWnd, &rect);
			TextOut(memoryHdc, prevX, prevY, str.data(), str.size());
			drawMode = BUFFER;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case WM_MOVE:
		InvalidateRect(hWnd, NULL, true);
		UpdateWindow(hWnd);
		break;
	case WM_DESTROY:
		ReleaseDC(hWnd, hdc);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

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
