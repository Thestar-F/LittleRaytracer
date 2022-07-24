#pragma once

#ifndef RAY_H
#define RAY_H

#include "vec3.h"

//�������ͣ�����
//���ߵı��ʽΪ p = o + td
//oΪ���ߵ�ԭ�㣬tΪ���շ����ƶ��Ĵ�С��dΪ��ķ���
//��Ա������orig��ԭ�㣬dir������
//��Ա������origin������direction������at����
class ray {

public:
	ray(){ }
	//�����˴�Ĭ�ϲ����Ĺ��캯�����������ʱ���Բ�ָ���ò���ray(ori, dir) timeĬ��Ϊ0.0
	ray(const point3& origin, const point3& direction, double time = 0.0)
		: orig(origin), dir(direction), tm(time) { }

	//���ع��ԭ��
	point3 origin() const{ return orig; }
	//���ع�ķ���
	vec3 direction() const { return dir; }
	//���ع���ڵ�ʱ��
	double time() const { return tm; }

	//���ع��߾���t����λ��
	point3 at(double t) const { 
		return orig + t * dir; 
	}

public:
	point3 orig;
	vec3 dir;
	//���ߴ��ڣ���������ʱ��
	double tm;
};




#endif