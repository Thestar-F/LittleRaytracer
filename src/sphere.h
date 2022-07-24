#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "pdf.h"
#include <cmath>

class sphere : public hittable {
public:
	sphere() { }
	sphere(point3 cen, double r,shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) { }

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;

    virtual double pdf_value(const point3& o, const vec3& v) const override;

    virtual vec3 random(const vec3& o) override;
    

public:
	point3 center;
	double radius;
    shared_ptr<material> mat_ptr;

private:
    //u,vΪ��������
    static void get_sphere_uv(const point3& p, double& u, double& v) {
        //����������������
        //����������һ�㣨x,y,z������Ϊ��-Y��Y�����Ϊ�е�ֵ����Ϊ��-X��+Z��+X��-Z�ٵ�-X�����Ϊ2�е�ֵ
        //y = -cos(�ȣ�
        //x = -cos(��)sin(��)
        //z = sin(��)sin(��)
        //��atan2��acos����ʵ�ֵķ��������ʽΪ
        //�� = atan2(-z, x) + ��
        //�� = acos��-y)
        //�������Ƕ���u,v��ӳ���ϵΪ
        //u = �� / 2�У� v = �� / ��

        auto phi = atan2(-p.z(), p.x()) + pi;
        auto theta = acos(-p.y());

        u = phi / (2 * pi);
        v = theta / pi;

    }
};

//һ��ԭ��Ϊcenter���뾶Ϊradius���򣬼������r�Ƿ�������ཻ
//��O+td-C����O+td-C��- r^2 = 0
bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    vec3 oc = r.origin() - center;
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
    vec3 outward_normal = (rec.p - center) / radius; //��������һ��
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;
    //��get_sphere_uv����������¼��ײ���u,v����
    get_sphere_uv(outward_normal, rec.u, rec.v);

    return true;   
}

//��ͨ����İ�Χ�У����������½ǵĵ���������Ͻǵĵ��ʾ
bool sphere::bounding_box(double t0, double t1, aabb& output_box) const {
    output_box = aabb(center - point3(radius, radius, radius),
                    center + point3(radius, radius, radius));
    return true;
}



double sphere::pdf_value(const point3& o, const vec3& v) const {
    hit_record rec;
    if (!this->hit(ray(o, v), 0.001, infinity, rec))
        return 0;

    double cos_theta_max = sqrt(1 - radius * radius / (center - o).length_squared());
    double solid_angle = 2 * pi * (1 - cos_theta_max);

    return 1 / solid_angle;
}

vec3 sphere::random(const vec3& o) {
    vec3 direction = center - o;
    auto distance_square = direction.length_squared();
    onb uvw;
    uvw.build(direction);
    return uvw.local(random_to_sphere(radius, distance_square));
}


#endif