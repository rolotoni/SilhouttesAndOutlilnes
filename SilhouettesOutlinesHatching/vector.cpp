///////////////////////////////////////////////////////////////////////
// Provides the framework a raytracer.
//
// Gary Herron
//
// Copyright © 2012 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

#include "vector.h"

Q QuaternionAA(const double angle, const V& v)
{
    double l = v.length();
    if (l < 1e-4)
        return Q(1,0,0,0);
    else {
        double f = sin(angle/2.0)/l;
        return Q(cos(angle/2.0), v*f); }
}

Q QuaternionV2Z(const V& v)
{
    V z = V(0,0,1);
    V r = z.cross(v);
    double a = acos(v.dot(z)/v.length());
    return QuaternionAA(a, r);
}
