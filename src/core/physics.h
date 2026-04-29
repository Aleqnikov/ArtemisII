#ifndef ARTEMISII_PHYSICS_H
#define ARTEMISII_PHYSICS_H


/**
 * В данном файле находятся всевозможные физические контстанты,
 * функции, описывающие положение луны или Земли.
 */

// Ситуация аналогична Луне.
double Earth_Angle(double t);


// Не могу сказать является это хардкодом, но да, изначальное положение луны
// заданно, согласно pdf документу, и исходя из этого и получаем текущее положение Луны
std::pair<double, double> moon_position (double t);

#endif //ARTEMISII_PHYSICS_H