#ifndef ROTATETEXTURE_H
#define ROTATETEXTURE_H
#include "Math.h"

class rotateTexture
{
    public:
        rotateTexture();
        void rotateBitmap(int mposx,int mposy,int x, int y);
        double getAngle();
        double delta_x,delta_y;
        double angle;
        int x,y;
        virtual ~rotateTexture();
    protected:
    private:
};

#endif // ROTATETEXTURE_H
