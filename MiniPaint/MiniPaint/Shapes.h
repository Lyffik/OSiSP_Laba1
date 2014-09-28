#pragma once

#include "stdafx.h"

class Shape abstract
{
protected:
	int x, y;

public:
	static int penWidth, penStyle;
	static COLORREF penColor;

	Shape(int, int);
	int getX();
	int getY();
	void setX(int);
	void setY(int);
	virtual void draw(HDC&, int, int) = 0;
};

class Line : public Shape
{
public:
	Line(int, int);
	virtual void draw(HDC&, int, int) override;
};

class Pencil : public Line
{
public:
	Pencil(int, int);
	virtual void draw(HDC&, int, int) override;
};

class CustomRectangle : public Shape
{
public:
	CustomRectangle(int, int);
	void draw(HDC&, int, int) override;
};

class CustomEllipse : public Shape
{
public:
	CustomEllipse(int, int);
	void draw(HDC&, int, int) override;
};