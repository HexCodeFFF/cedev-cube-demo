#include <cmath>
#include <tice.h>
#include <graphx.h>
#include <keypadc.h>
#include <debug.h>

double PI = 3.14159265;

double a = 0.0; // l-r rotation
double a_2 = 0.0; // u-d rotation
double d = 4.0; // distance i think?

double sindegrees(double degrees) {
    return sin(degrees * (PI / 180));
}

double cosdegrees(double degrees) {
    return cos(degrees * (PI / 180));
}

// function z determines the z position of a cylindrical coordinate
double z(double theta, double r, double h) {
    // \cos\left(a_{2}\right)r\sin\left(a+\theta\right)-\sin\left(a_{2}\right)h
    return (cosdegrees(a_2) * r * sindegrees(a + theta)) - (sindegrees(a_2) * h);
}

struct xyz {
    double x;
    double y;
    double z;
};

xyz ctoc(double x, double y, double z) {
    return {atan2(y, x) * (180 / PI), sqrt(pow(x, 2) + pow(y, 2)), z};
}

struct xy {
    double x;
    double y;
};

// function c plots cylindrical coordinates
xy c(double theta, double r, double h) {
    double zres = d - z(theta, r, h);
//    if (zres <= 0) {
//        return {NAN, NAN};
//    }

    double thing = (d / zres);
    double x = thing * r * cosdegrees(a + theta);
    double y = thing * ((r * sindegrees(a_2) * sindegrees(a + theta)) + (h * cosdegrees(a_2)));
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
        if (kb_IsDown(kb_KeyUp)) a_2 -= 10;
        if (kb_IsDown(kb_KeyDown)) a_2 += 10;
        if (kb_IsDown(kb_KeyLeft)) a += 10;
        if (kb_IsDown(kb_KeyRight)) a -= 10;
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
            gfx_Line(int(projected_points[cl.index0].x * LCD_HEIGHT / 4) + LCD_WIDTH / 2,
                     int(projected_points[cl.index0].y * LCD_HEIGHT / 4) + LCD_HEIGHT / 2,
                     int(projected_points[cl.index1].x * LCD_HEIGHT / 4) + LCD_WIDTH / 2,
                     int(projected_points[cl.index1].y * LCD_HEIGHT / 4) + LCD_HEIGHT / 2);
        }


        // Swap the buffer with the screen
        gfx_SwapDraw();

    } while (!kb_IsDown(kb_KeyEnter));

    // End graphics drawing
    gfx_End();

    return 0;
}