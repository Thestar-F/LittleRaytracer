#pragma once
#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

#include "rtweekend.h"

using std::sqrt;
//定义常用的类型：向量
class vec3 {

public:
    vec3() : e{ 0, 0, 0 } {}
    vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }


    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }    
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(const double t) {
        return *this *= 1 / t;
    }

    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    double length() const {
        return std::sqrt(length_squared());
    }

    //产生一个随机向量，向量的每一个分量的值都在[0,1)之间
    inline static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }
    //产生一个随机向量，向量的每一个分量的值都在[min, max)之间
    inline static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }

    //判断该向量是否十分接近0
    bool near_zero() const{
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

public:
    double e[3];

};

//定义vec3的别名 
using point3 = vec3; //3D的点
using color = vec3; //RGB

#endif

//常用的向量运算
inline std::ostream& operator<< (std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(const double t, const vec3& v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, const double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, const double t) {
    return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

//产生一个单位圆中的随机点，如果点到球心的距离大于1，则判定不在球中将其舍弃，直到满足条件为止。
inline point3 random_in_unit_sphere() {
    while (1) {
        point3 p = vec3::random(-1, 1);
        if (p.length_squared() >= 1)
            continue;
        return p;
    }
}

//产生单位圆上的随机点
point3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

//产生半球内的随机方向向量
vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0 )
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

//镜面反射方向的计算
vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

//折射方向计算
//计算公式：R_per = (eta/etap) * (R + con(theta) * n)
// R_par = - sqrt(fabs(1 - R_per ^ 2))) * n
vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    double cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_per = etai_over_etat * (uv + cos_theta * n);
    vec3 r_par = -sqrt(fabs(1.0 - r_per.length_squared())) * n;
    return r_per + r_par;
}

//产生光圈内的随机偏置
vec3 random_in_unit_disk() {
    while (1) {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1)
            continue;
        return p;
    }
}
