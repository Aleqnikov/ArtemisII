#ifndef ARTEMISII_RK45_H
#define ARTEMISII_RK45_H


Vector RK45(double t, const Vector& y, double h,
       Vector (*function)(double t, const Vector y, void* ctx),
       void* ctx);


#endif //ARTEMISII_RK45_H