#pragma once

#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <memory>
#include <cmath>
#include <limits>
#include <cstdlib>

//这个项目中都会用到的头文件

//需要用到参数
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//常用特定作用域下的函数
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//常用函数
inline double degrees_to_radians(double degrees) {
	return (degrees * pi) / 180.0;
}

//产生一个[0,1）间的随机值
inline double random_double() {
	return rand() / (RAND_MAX + 1.0);
}
//产生一个[min,max)之间的随机值
inline double random_double(double min, double max) {
	return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max) {
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

//返回一个整型的[min,max]的随机数
inline int random_int(int min, int max) {
	return static_cast<int>(random_double(min, max + 1));
}

#include "vec3.h"
#include "ray.h"

#endif