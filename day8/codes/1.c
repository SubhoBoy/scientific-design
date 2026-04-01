#include <stdio.h>

typedef float (*deriv_f)(float x, float y);

float rk4(float x, float y, float h, deriv_f df) {
    float k1 = h * df(x, y);
    float k2 = h * df(x + 0.5 * h, y + 0.5 * k1);
    float k3 = h * df(x + 0.5 * h, y + 0.5 * k2);
    float k4 = h * df(x + h, y + k3);
    return y + (k1 + 2*k2 + 2*k3 + k4) / 6.0;
}

float solve(float x0, float y0, float x_target, deriv_f df) {
    float x = x0;
    float y = y0;
    const float h = 0.01;
    int n_steps = (int)((x_target - x0) / h);

    for (int i = 0; i < n_steps; i++) {
        y = rk4(x, y, h, df);
        x += h;
    }

    float rem_h = x_target - x;
    if (rem_h > 1e-6) { // Use a small epsilon
        y = rk4(x, y, rem_h, df);
    }

    return y;
}

float deriv_exp(float x, float y) {
    return y;
}

int main(void) {
    // float e = solve(0.0, 1.0, 1.0, deriv_exp);
    return 0;
}
