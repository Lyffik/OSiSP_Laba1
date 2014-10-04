#include "Initialization.h"

void initializeDcs(HWND &hWnd,HDC &hdc,HDC &drawingHdc,HBITMAP &drawBitmap,HDC &memoryHdc,HBITMAP &memoryBitmap)
{
	RECT rect;
	HBRUSH brush;
	HPEN pen;

	hdc = GetDC(hWnd);
	rect.top = 0;
	rect.left = 0;
	rect.right = 2500;
	rect.bottom = 2000;

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
	PatBlt(memoryHdc, 0, 0, rect.right, rect.bottom, PATCOPY);

	DeleteObject(SelectObject(drawingHdc, pen));
	DeleteObject(SelectObject(drawingHdc, brush));
	DeleteObject(SelectObject(memoryHdc, pen));
	DeleteObject(SelectObject(memoryHdc, brush));

}

