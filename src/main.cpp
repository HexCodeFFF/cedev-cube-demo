#include <tice.h>
#include <graphx.h>
#include <keypadc.h>
#include "fasttrig.h"

float PI = 3.14159265;
float PIOVER180 = PI / 180;

float a = 0.0; // l-r rotation
float a_2 = 0.0; // u-d rotation
float d = 4.0; // distance i think?

float sindegrees(float degrees) {
    return fastsin(degrees * PIOVER180);
}

float cosdegrees(float degrees) {
    return fastcos(degrees * PIOVER180);
}

// function z determines the z position of a cylindrical coordinate
float z(float theta, float r, float h) {
    // \cos\left(a_{2}\right)r\sin\left(a+\theta\right)-\sin\left(a_{2}\right)h
    return (cosdegrees(a_2) * r * sindegrees(a + theta)) - (sindegrees(a_2) * h);
}

struct xyz {
    float x;
    float y;
    float z;
};

xyz ctoc(float x, float y, float z) {
    return {atan2f(y, x) * (180 / PI), sqrtf(x*x + y*y), z};
}

struct xy {
    float x;
    float y;
};

// function c plots cylindrical coordinates
xy c(float theta, float r, float h) {
    float zres = d - z(theta, r, h);
//    if (zres <= 0) {
//        return {NAN, NAN};
//    }

    float thing = (d / zres);
    float x = thing * r * cosdegrees(a + theta);
    float y = thing * ((r * sindegrees(a_2) * sindegrees(a + theta)) + (h * cosdegrees(a_2)));
    return {x, y};
}

struct cubeline {
    int8_t index0;
    int8_t index1;
};

int main() {
    // Start the graphics routines
    gfx_Begin();
    gfx_SetDrawBuffer();

    xyz cartesiancube[8] = {
            {-1, -1, -1},
            {-1, -1, 1},
            {-1, 1,  -1},
            {-1, 1,  1},
            {1,  -1, -1},
            {1,  -1, 1},
            {1,  1,  -1},
            {1,  1,  1}
    };
    xyz cylindricalcube[8] = {};

    cubeline cube[12] = {
            {0, 1},
            {1, 3},
            {3, 2},
            {2, 0},
            {0, 4},
            {1, 5},
            {3, 7},
            {2, 6},
            {4, 6},
            {6, 7},
            {7, 5},
            {5, 4},
    };

    for (int i = 0; i < 8; ++i) {
        cylindricalcube[i] = ctoc(cartesiancube[i].x, cartesiancube[i].y, cartesiancube[i].z);
    }

    // Loop until a key is pressed
    xy projected_points[8] = {};
    do {
        kb_Scan();
        // key control
        float rotspeed = 5;
        if (kb_IsDown(kb_KeyUp)) a_2 -= rotspeed;
        if (kb_IsDown(kb_KeyDown)) a_2 += rotspeed;
        if (kb_IsDown(kb_KeyLeft)) a += rotspeed;
        if (kb_IsDown(kb_KeyRight)) a -= rotspeed;
        if (kb_IsDown(kb_KeyAdd)) d -= 0.5;
        if (kb_IsDown(kb_KeySub)) d += 0.5;

        // bound properly
        if (a > 180) a -= 360;
        if (a < -180) a += 360;
        if (a_2 > 180) a_2 -= 360;
        if (a_2 < -180) a_2 += 360;
        if (d > 7) d = 7;
        if (d < 2) d = 2;

        gfx_FillScreen(255);
        for (int i = 0; i < 8; ++i) {
            projected_points[i] = c(cylindricalcube[i].x, cylindricalcube[i].y, cylindricalcube[i].z);
        }

        for (auto cl: cube) {
            gfx_Line(int(projected_points[cl.index0].x * (LCD_HEIGHT / 4)) + (LCD_WIDTH / 2),
                     int(projected_points[cl.index0].y * (LCD_HEIGHT / 4)) + (LCD_HEIGHT / 2),
                     int(projected_points[cl.index1].x * (LCD_HEIGHT / 4)) + (LCD_WIDTH / 2),
                     int(projected_points[cl.index1].y * (LCD_HEIGHT / 4)) + (LCD_HEIGHT / 2));
        }


        // Swap the buffer with the screen
        gfx_SwapDraw();

    } while (!kb_IsDown(kb_KeyEnter));

    // End graphics drawing
    gfx_End();

    return 0;
}