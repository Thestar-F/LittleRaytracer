#pragma once

#ifndef BOX_H
#define BOX_H

#include "rtweekend.h"
#include "hittable_list.h"
#include "material.h"
#include "aarec.h"

class box : public hittable {
public:
	point3 box_min;
	point3 box_max;
	hittable_list sides;
public:
	box() {}
	//��box�ġ����½ǡ���͡����Ͻǡ���������һ��box����ʽ����������ƽ�棬����ƽ��
	box(const point3& p0, const point3& p1, shared_ptr<material> mat);


	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override{
		output_box = aabb(box_min, box_max);
		return true;
	}

};

box::box(const point3& p0, const point3& p1, shared_ptr<material> mat) {
	box_min = p0;
	box_max = p1;

	//��������ƽ�е�xyƽ��
	sides.add(make_shared<xy_rec>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), mat));
	sides.add(make_shared<xy_rec>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), mat));

	//��������ƽ�е�xzƽ��
	sides.add(make_shared<xz_rec>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), mat));
	sides.add(make_shared<xz_rec>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), mat));

	//��������ƽ�е�yzƽ��
	sides.add(make_shared<yz_rec>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), mat));
	sides.add(make_shared<yz_rec>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), mat));

	//����������ƽ�����sides��

}

bool box::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	//����hittable_list�ĵ�hit�������жϹ����Ƿ���box�ཻ
	return sides.hit(r, t_min, t_max, rec);
}


#endif
