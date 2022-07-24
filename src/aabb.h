#pragma once

#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

class aabb {
public:
	//用两个点，表述一个三维轴对齐包围盒
	point3 mininum;
	point3 maxinum;
public:
	aabb(){}
	aabb(const point3& a, const point3& b) : mininum(a), maxinum(b) {}
	
	//min()和max()用来返回用于构建该包围盒的最小和最大值点
	point3 min() const { return mininum; }
	point3 max() const { return maxinum; }
	bool hit(const ray& r, double t_min, double t_max) const;
};

//判断光线是否与包围盒相交
inline bool aabb::hit(const ray& r, double t_min, double t_max) const{
	for (int a = 0; a < 3; a++) {
		double invD = 1.0f / r.direction()[a];
		//计算光线r与x、y、z三个平面相交时的t
		auto t0 = (min()[a] - r.origin()[a]) * invD;
		auto t1 = (max()[a] - r.origin()[a]) * invD;
		//如果invD小于0，则交换t0,t1
		if (invD < 0.0f)
			std::swap(t0, t1);
		//t_min 取t0的最大值， t_max 取t1的最大值
		t_min = t0 > t_min ? t0 : t_min;
		t_max = t1 < t_max ? t1 : t_max;
		if (t_max <= t_min)
			return false;
	}
	return true;
}

//计算两个包围盒的包围盒
aabb surrounding_box(aabb box0, aabb box1) {
	point3 small(fmin(box0.min().x(), box1.min().x()),
			fmin(box0.min().y(), box1.min().y()),
			fmin(box0.min().z(), box1.min().z()));
	point3 big(fmax(box0.max().x(), box1.max().x()),
			fmax(box0.max().y(), box1.max().y()),
			fmax(box0.max().z(), box1.max().z()));

	return aabb(small, big);
}

#endif