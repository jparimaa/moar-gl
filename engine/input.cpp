#include "input.h"

namespace moar
{

Input::Input() :
    x(0.0),
    y(0.0),
    deltaX(0.0),
    deltaY(0.0),
    sensitivity(1.0),
    movementSpeed(1.0)
{
}

Input::~Input()
{
}

void Input::setCursorPosition(double x, double y)
{
    deltaX = x - this->x;
    deltaY = y - this->y;
    this->x = x;
    this->y = y;
}

double Input::getCursorDeltaX() const
{
    return sensitivity * deltaX;
}

double Input::getCursorDeltaY() const
{
    return sensitivity * deltaY;
}

} // moar