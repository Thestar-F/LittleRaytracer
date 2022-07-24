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
	//由box的“左下角”点和“右上角”点来构建一个box，方式是生成六个平面，两两平行
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

	//生成两两平行的xy平面
	sides.add(make_shared<xy_rec>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), mat));
	sides.add(make_shared<xy_rec>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), mat));

	//生成两两平行的xz平面
	sides.add(make_shared<xz_rec>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), mat));
	sides.add(make_shared<xz_rec>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), mat));

	//生成两两平行的yz平面
	sides.add(make_shared<yz_rec>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), mat));
	sides.add(make_shared<yz_rec>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), mat));

	//将上述六个平面放入sides中

}

bool box::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	//调用hittable_list的的hit函数，判断光线是否与box相交
	return sides.hit(r, t_min, t_max, rec);
}


#endif
