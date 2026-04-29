#ifndef ARTEMISII_ADAMS_H
#define ARTEMISII_ADAMS_H


Vector Adams(double t, const Vector& y, double h,
       Vector (*function)(double t, const Vector y, void* ctx),
       void* ctx);


#endif //ARTEMISII_ADAMS_H