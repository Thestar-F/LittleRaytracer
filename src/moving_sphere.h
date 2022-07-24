#pragma once

#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "rtweekend.h"
#include "hittable.h"

//�˶���������
class moving_sphere : public hittable {
public:
    //����center���ֱ�Ϊ��ʼ���center���յ��center
    //����time0ʱ������ʼ�㣬time1ʱ�����յ�
	point3 center0;
	point3 center1;
	double time0;
	double time1;
	double radius;
	shared_ptr<material> mat_ptr;

public:
	moving_sphere() {}
	moving_sphere(point3 cen0, point3 cen1, double tm0, double tm1, double r, shared_ptr<material> material) :
		center0(cen0), center1(cen1), time0(tm0), time1(tm1), radius(r), mat_ptr(material) { }

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;

    //center�������������ڻ��������Ŵ�ʱ�����ĵ�λ��
	point3 center(double time) const;
};

point3 moving_sphere::center(double time) const{
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center(r.time());
    double a = dot(r.direction(), r.direction());
    double half_b = dot(r.direction(), oc);
    double c = oc.length_squared() - radius * radius;
    double discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
        return false;
    double sqrtd = std::sqrt(discriminant);

    double t = (-half_b - sqrtd) / a;
    if (t < t_min || t > t_max) {
        t = (-half_b + sqrtd) / a;
        if (t < t_min || t > t_max) {
            return false;
        }
    }
    rec.p = r.at(t);
    rec.t = t;
    //����Ϊ������Ŵ�ʱ���ĵ�λ��
    vec3 outward_normal = (rec.p - center(r.time())) / radius; //��������һ��
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

//�˶�����İ�Χ�У����˶���ʼ����յ�ʱ�����嶼������ȥ
bool moving_sphere::bounding_box(double t0, double t1, aabb& output_box) const {
    //���������յ�İ�Χ�У��������ǵİ�Χ��
    aabb box0 = aabb(center(t0) - point3(radius, radius, radius),
                    center(t0) + point3(radius, radius, radius));
    aabb box1 = aabb(center(t1) - point3(radius, radius, radius),
                    center(t1) + point3(radius, radius, radius));
    output_box = surrounding_box(box0, box1);
    return true;
}


#endif