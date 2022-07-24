#pragma once

#ifndef AAREC_H
#define AAREC_H

#include "rtweekend.h"
#include "hittable.h"

class xy_rec : public hittable{
public:
	double x0, x1, y0, y1, k;
	shared_ptr<material> mp;

public:
	xy_rec() {}
	//构造矩形的xy平面，x的范围为x0到x1，y的范围为y0到y1，z的值为k
	xy_rec(double _x0, double _x1, double _y0, double _y1, double _k, shared_ptr<material> mat):
		x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

	//将一个平面拓展成一个很窄的立方体，作为该平面的包围盒
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override {
		output_box = aabb(point3(x0, y0, k - 0.0001), point3(x1, y1, k + 0.0001));
		return true;
	};

};

class xz_rec : public hittable {
public:
	double x0, x1, z0, z1, k;
	shared_ptr<material> mp;

public:
	xz_rec() {}
	xz_rec(double _x0, double _x1, double _z0, double _z1, double _k, shared_ptr<material> mat) :
		x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override {
		output_box = aabb(point3(x0, k - 0.0001, z0), point3(x1, k + 0.0001, z1));
		return true;
	};

	virtual double pdf_value(const point3& o, const vec3& v) const override{
		hit_record rec;
		if (!this->hit(ray(o, v), 0.001, infinity, rec))
			return 0;

		double area = (x1 - x0) * (z1 - z0);
		double distance = rec.t * rec.t * v.length_squared();
		double cosine = fabs(dot(rec.normal, v) / v.length());

		return distance / (cosine * area);

	}

	virtual vec3 random(const vec3& o) {
		point3 random_point(random_double(x0, x1), k, random_double(z0, z1));
		return random_point - o;
	}

};

class yz_rec : public hittable {
public:
	double y0, y1, z0, z1, k;
	shared_ptr<material> mp;

public:
	yz_rec() {}
	yz_rec(double _y0, double _y1, double _z0, double _z1, double _k, shared_ptr<material> mat) :
		y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override {
		output_box = aabb(point3(k - 0.0001, y0, z0), point3(k + 0.0001, y1, z1));
		return true;
	};

};


bool xy_rec::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	//计算光线在z轴分量为k时,t的值是多少，是否在范围内
	auto t = (k - r.origin().z()) / r.direction().z();
	if (t < t_min || t > t_max)
		return false;

	//计算t时，光线的x，y值分别为多少，是否在矩形范围内
	auto x = r.origin().x() + t * r.direction().x();
	auto y = r.origin().y() + t * r.direction().y();
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;

	//由x,y计算它们的u，v分量
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.mat_ptr = mp;
	rec.t = t;
	auto outward_normal = vec3(0, 0, 1);
	rec.set_face_normal(r, outward_normal);
	rec.p = r.at(t);

	return true;
}


bool xz_rec::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	auto t = (k - r.origin().y()) / r.direction().y();
	if (t < t_min || t > t_max)
		return false;

	auto x = r.origin().x() + t * r.direction().x();
	auto z = r.origin().z() + t * r.direction().z();
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;

	rec.u = (x - x0) / (x1 - x0);
	rec.v = (z - z0) / (z1 - z0);
	rec.mat_ptr = mp;
	rec.t = t;
	auto outward_normal = vec3(0, 1, 0);
	rec.set_face_normal(r, outward_normal);
	rec.p = r.at(t);

	return true;
}



bool yz_rec::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	auto t = (k - r.origin().x()) / r.direction().x();
	if (t < t_min || t > t_max)
		return false;

	auto z = r.origin().z() + t * r.direction().z();
	auto y = r.origin().y() + t * r.direction().y();
	if (z < z0 || z > z1 || y < y0 || y > y1)
		return false;

	rec.u = (y - y0) / (y1 - y0);
	rec.v = (z - z0) / (z1 - z0);
	rec.mat_ptr = mp;
	rec.t = t;
	auto outward_normal = vec3(1, 0, 0);
	rec.set_face_normal(r, outward_normal);
	rec.p = r.at(t);

	return true;
}



#endif