#pragma once

#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <memory>
#include <cmath>
#include <limits>
#include <cstdlib>

//�����Ŀ�ж����õ���ͷ�ļ�

//��Ҫ�õ�����
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//�����ض��������µĺ���
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//���ú���
inline double degrees_to_radians(double degrees) {
	return (degrees * pi) / 180.0;
}

//����һ��[0,1��������ֵ
inline double random_double() {
	return rand() / (RAND_MAX + 1.0);
}
//����һ��[min,max)֮������ֵ
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

//����һ�����͵�[min,max]�������
inline int random_int(int min, int max) {
	return static_cast<int>(random_double(min, max + 1));
}

#include "vec3.h"
#include "ray.h"

#endif