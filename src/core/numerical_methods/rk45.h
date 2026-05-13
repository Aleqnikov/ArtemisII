#ifndef ARTEMISII_RK45_H
#define ARTEMISII_RK45_H


Vector RK4(double t, const Vector& y, double h,
          Vector (*function)(double t, const Vector y, void* ctx),
          void* ctx1)
{
    auto k_1 = function(t,          y, 			ctx1);
    auto k_2 = function(t + h/2.0,  y + (k_1*h)/2.0, 	ctx1);
    auto k_3 = function(t + h/2.0,  y + (k_2*h)/2.0, 	ctx1);
    auto k_4 = function(t + h,      y + (k_3*h), 		ctx1);

    auto p = k_1 + k_2*2 + k_3*2 + k_4;
    auto p2 = (k_1 + k_2*2 + k_3*2 + k_4)*h;

    auto res = y + ((k_1 + k_2*2 + k_3*2 + k_4)*h)/6.0;


    return res;
}

#endif //ARTEMISII_RK45_H