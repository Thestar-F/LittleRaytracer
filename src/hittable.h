#pragma once

#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"
#include "aabb.h"

class material;
//定义一个结构体，存储光线触碰到的点的信息
struct hit_record {
	point3 p;
	vec3 normal;
	double t;
	bool front_face;
	shared_ptr<material> mat_ptr;
	//碰撞点对应的纹理坐标（u,v)
	double u;
	double v;

	//front_face为true表示光线从物体的外面射入，否则表示光线从物体的里面射入
	//设置光与物体碰撞点的法线方向总是与光线本身的方向相对

	//outward_normal表示指向物体外面的法线
	//该函数的作用为判断光线从物体哪面与物体碰撞，并将碰撞点的法线方向置为与光线相对的方向
	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

//定义一个纯虚函数的抽象类，抽象出可以被碰到的物体
class hittable {
public:
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
	//bounding_box()函数判断该hittable是否具有包围盒
	//若没有包围盒，则返回false。若有包围盒，将包围盒赋值到传出参数output_box中
	//t0、t1为考虑到运动物体的运动起始和终止时间
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const = 0;

	virtual double pdf_value(const point3& o, const vec3& v) const {
		return 0.0;
	}
	
	virtual vec3 random(const vec3& o) {
		return vec3(1, 0, 0);
	}

};


//实现物体的平移
class translate : public hittable {
public:
	shared_ptr<hittable> ptr;
	vec3 offset;

public:
	translate(shared_ptr<hittable> p, const vec3& displacement) : ptr(p), offset(displacement) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;
};


bool translate::bounding_box(double t0, double t1, aabb& output_box) const {
	if (!ptr->bounding_box(t0, t1, output_box))
		return false;
	//平移后物体的包围盒，为原来的包围盒的点再加一平移量
	output_box = aabb(output_box.min() + offset, output_box.max() + offset);
	
	return true;
}

bool translate::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	//在光线方向不变的情况下，将光线向物体的反方向移动，以此在物体不变的情况下，模拟移动的效果
	ray moved_r = ray(r.origin() - offset, r.direction(), r.time());

	if (!ptr->hit(moved_r, t_min, t_max, rec))
		return false;
	
	//rec.p为碰撞到的点的实际位置，需要将它加上偏移量，改变为移动后的点
	rec.p += offset;
	rec.set_face_normal(moved_r, rec.normal);

	return true;
}



class rotate_y : public hittable {
public:
	shared_ptr<hittable> ptr;
	double sin_theta;
	double cos_theta;
	bool hasbox;
	aabb bbox;

public:
	rotate_y(shared_ptr<hittable> p, double angle);

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override {
		output_box = bbox;
		return hasbox;
	}
};

//绕y轴逆时针旋转θ的公式为
//x` = xcosθ + zsinθ
//z` = -xsinθ + ycosθ
rotate_y::rotate_y(shared_ptr<hittable> p, double angle) {
	ptr = p;
	auto rad = degrees_to_radians(angle);
	sin_theta = sin(rad);
	cos_theta = cos(rad);

	hasbox = p->bounding_box(0, 1, bbox);

	point3 min(infinity, infinity, infinity);
	point3 max(-infinity, -infinity, -infinity);
	//将原本包围盒的点两两组合，计算新的最大值和最小值点
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
				auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
				auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

				double newx = x * cos_theta + z * sin_theta;
				double newz = -x * sin_theta + z * cos_theta;

				vec3 tmp(newx, y, newz);

				for (int c = 0; c < 3; c++) {
					min[c] = fmin(min[c], tmp[c]);
					max[c] = fmax(max[c], tmp[c]);
				}
			}
		}
	}

	bbox = aabb(min, max);
}


bool rotate_y::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	auto origin = r.origin();
	auto direction = r.direction();
	
	//与实现物体平移的方法相同
	//先将光线原点和方向旋转-θ，用于模拟物体旋转θ的效果

	origin[0] = r.origin()[0] * cos_theta - r.origin()[2] * sin_theta;
	origin[2] = r.origin()[0] * sin_theta + r.origin()[2] * cos_theta;

	direction[0] = r.direction()[0] * cos_theta - r.direction()[2] * sin_theta;
	direction[2] = r.direction()[0] * sin_theta + r.direction()[2] * cos_theta;

	ray rotate_r(origin, direction, r.time());

	if (!ptr->hit(rotate_r, t_min, t_max, rec))
		return false;
	
	auto p = rec.p;
	auto normal = rec.normal;

	//再讲求得的点和法线方向旋转θ，改变为旋转后的点和方向
	p[0] = rec.p[0] * cos_theta + rec.p[2] * sin_theta;
	p[2] = -rec.p[0] * sin_theta + rec.p[2] * cos_theta;

	normal[0] = rec.normal[0] * cos_theta + rec.normal[2] * sin_theta;
	normal[2] = -rec.normal[0] * sin_theta + rec.normal[2] * cos_theta;

	rec.p = p;
	rec.set_face_normal(rotate_r, normal);
	
	return true;
}


class flip_face : public hittable {
public:
	shared_ptr<hittable> ptr;

public:
	flip_face(shared_ptr<hittable> p) : ptr(p) {}


	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
		if (!ptr->hit(r, t_min, t_max, rec))
			return false;
		rec.front_face = !rec.front_face;
		return true;
	}
	
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
		return ptr->bounding_box(t0, t1, output_box);
	}




};





#endif