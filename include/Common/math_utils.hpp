#ifndef MATH_UTILS_H
#define MATH_UTILS_H

template <typename T>
inline T HermitianSmoothing(T cor) {
    return cor * cor * (3 - 2 * cor);
}

template <typename T>
inline T lerp(T a, T b, T t) {
    return a + t * (b - a);
}

#endif