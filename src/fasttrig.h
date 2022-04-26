#include <cmath>
#include <debug.h>

const float invtwopi = 0.1591549f;
const float twopi = 6.283185f;
const float threehalfpi = 4.7123889f;
const float pi = 3.141593f;
const float halfpi = 1.570796f;
const float quarterpi = 0.7853982f;

float cos_32s(float x) {
    const float c1 = 0.99940307f;
    const float c2 = -0.49558072f;
    const float c3 = 0.03679168f;
    float x2 = x * x;
    return (c1 + x2 * (c2 + c3 * x2));
}

float fastcos(float angle) {
    //clamp to the range 0..2pi
    angle = angle - floorf(angle * invtwopi) * twopi;
    angle = angle > 0.f ? angle : -angle;
    if (angle < halfpi) return cos_32s(angle);
    if (angle < pi) return -cos_32s(pi - angle);
    if (angle < threehalfpi) return -cos_32s(angle - pi);
    return cos_32s(twopi - angle);
}

float fastsin(float angle) {
    return fastcos(halfpi - angle);
}