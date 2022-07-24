#pragma once

#ifndef RAY_H
#define RAY_H

#include "vec3.h"

//定义类型：光线
//光线的表达式为 p = o + td
//o为光线的原点，t为按照方向移动的大小，d为光的方向
//成员变量：orig：原点，dir：方向
//成员函数：origin（），direction（），at（）
class ray {

public:
	ray(){ }
	//定义了带默认参数的构造函数，则定义变量时可以不指定该参数ray(ori, dir) time默认为0.0
	ray(const point3& origin, const point3& direction, double time = 0.0)
		: orig(origin), dir(direction), tm(time) { }

	//返回光的原点
	point3 origin() const{ return orig; }
	//返回光的方向
	vec3 direction() const { return dir; }
	//返回光存在的时间
	double time() const { return tm; }

	//返回光线经过t所在位置
	point3 at(double t) const { 
		return orig + t * dir; 
	}

public:
	point3 orig;
	vec3 dir;
	//光线存在（产生）的时间
	double tm;
};




#endif