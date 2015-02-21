///////////////////////////////////////////////////////////////////////
// Provides the framework a raytracer.
//
// Gary Herron
//
// Copyright © 2012 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

#include <math.h>


class V {
public:
    double x, y, z;
    V(double x_=0, double y_=0, double z_=0){ x=x_; y=y_; z=z_; }
    V operator+(const V& b) const { return V(x+b.x, y+b.y, z+b.z); }
    V operator-() const { return V(-x, -y, -z); }
    V operator-(const V& b) const { return V(x-b.x, y-b.y, z-b.z); }
    V operator*(const double b) const { return V(x*b, y*b, z*b); }
    V operator/(const double b) const { return V(x/b, y/b, z/b); }
    double length() const { return sqrt(x*x+y*y+z*z); }
    double lum() const { return (x+y+z)/3.0; }
    V unit() const { return *this / this->length(); }
    double dot(const V& b) const { return x*b.x+y*b.y+z*b.z; }
    double dotP(const V& b) const { double d=this->dot(b); return d>0.0?d:0.0; }
    V cross(const V& b) const {return V(y*b.z-z*b.y, z*b.x-x*b.z, x*b.y-y*b.x);}
    V operator*(const V& b) const { return V(x*b.x, y*b.y, z*b.z); } // Beware
    bool operator==(const V& b) const { return x==b.x && y==b.y && z==b.z; }
    bool operator!=(const V& b) const { return !(*this==b); }
};

class Q {
public:
    double w, x, y, z;
    Q(double w_=0, double x_=0, double y_=0, double z_=0){ w=w_; x=x_; y=y_; z=z_; }
    Q(double w_, const V& v){ w=w_; x=v.x; y=v.y; z=v.z; }
    Q operator+(const Q& b) const { return Q(w+b.w, x+b.x, y+b.y, z+b.z); }
    Q operator-(const Q& b) const { return Q(w-b.w, x-b.x, y-b.y, z-b.z); }
    Q operator-() const { return Q(-w, -x, -y, -z); }
    Q operator*(const double b) const { return Q(w*b, x*b, y*b, z*b); }
    Q operator*(const Q& b) const { return Q(w*b.w - x*b.x - y*b.y - z*b.z,
                                             w*b.x + x*b.w + y*b.z - z*b.y,
                                             w*b.y + y*b.w + z*b.x - x*b.z,
                                             w*b.z + z*b.w + x*b.y - y*b.x); }
    Q operator/(const double b) const { return Q(w/b, x/b, y/b, z/b); }
    double length() const { return sqrt(w*w+x*x+y*y+z*z); }
    double dot(const Q& b) const { return w*b.w+x*b.x+y*b.y+z*b.z; }
    Q conj() const { return Q(w, -x, -y, -z); }
    V axis() const { return V(x,y,z); }
    V rotate(const V& b) const { return (conj()* Q(0.0, b.x, b.y, b.z) * *this).axis(); }
};


Q QuaternionAA(const double angle, const V& v);
Q QuaternionV2Z(const V& v);
