#pragma once

#include "stdafx.h"



	void initializeDcs(HWND &hWnd,
		HDC &hdc,
		HDC &drawingHdc,
		HBITMAP &drawBitmap,
		HDC &memoryHdc,
		HBITMAP &memoryBitmap);

	enum draw { CURRENT, BUFFER, PEN };

