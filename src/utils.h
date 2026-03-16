#pragma once
#include <Arduino.h>

struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;
    
    Vector2() = default;
    Vector2(float x, float y) : x(x), y(y) {}

    //TODO: implement addition, subtraction, multiplication, division operators

    Vector2 rotate(float angle) {
        float sine = sinf(angle);
        float cosine = cosf(angle);
        return Vector2(
            x * cosine - y * sine,
            x * sine + y * cosine
        );
    }
};