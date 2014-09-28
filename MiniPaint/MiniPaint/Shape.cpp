#include "Shapes.h"

int Shape::penWidth = 1;
int Shape::penStyle = PS_SOLID;
COLORREF Shape::penColor = RGB(0, 0, 0);

// Shape
Shape::Shape(int newX, int newY) : x(newX), y(newY){}

int Shape::getX()
{
	return x;
}

int Shape::getY()
{
	return y;
}

void Shape::setX(int newX)
{
	x = newX;
}

void Shape::setY(int newY)
{
	y = newY;
}

// Line
Line::Line(int newX, int newY) : Shape(newX, newY){}

void Line::draw(HDC &dc, int newX, int newY)
{
	MoveToEx(dc, x, y, NULL);
	LineTo(dc, newX, newY);
}

//Pencil
Pencil::Pencil(int newX, int newY) : Line(newX, newY){}

void Pencil::draw(HDC &dc, int newX, int newY)
{
	__super::draw(dc, newX, newY);
	x = newX;
	y = newY;
}


// Rectangle
CustomRectangle::CustomRectangle(int newX, int newY) : Shape(newX, newY){}

void CustomRectangle::draw(HDC &dc, int newX, int newY)
{
	Rectangle(dc, x, y, newX, newY);
}

// Ellipse
CustomEllipse::CustomEllipse(int newX, int newY) : Shape(newX, newY){}

void CustomEllipse::draw(HDC &dc, int newX, int newY)
{
	Ellipse(dc, x, y, newX, newY);
}
