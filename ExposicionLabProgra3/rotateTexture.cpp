#include "rotateTexture.h"

rotateTexture::rotateTexture()
{

}

rotateTexture::~rotateTexture()
{
    //dtor
}

void rotateTexture::rotateBitmap(int mposx,int mposy,int x,int y)
{
    delta_x = mposx - x;
    delta_y = mposy - y;

    angle = (atan2(delta_y,delta_x));
    angle*=(180.0000/3.1416);

    if (angle < 0)
        angle = 360 -(-angle);
}

double rotateTexture::getAngle()
{
    return angle;
}
