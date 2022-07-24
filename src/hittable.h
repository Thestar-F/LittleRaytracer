#pragma once

#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"
#include "aabb.h"

class material;
//����һ���ṹ�壬�洢���ߴ������ĵ����Ϣ
struct hit_record {
	point3 p;
	vec3 normal;
	double t;
	bool front_face;
	shared_ptr<material> mat_ptr;
	//��ײ���Ӧ���������꣨u,v)
	double u;
	double v;

	//front_faceΪtrue��ʾ���ߴ�������������룬�����ʾ���ߴ��������������
	//���ù���������ײ��ķ��߷�����������߱���ķ������

	//outward_normal��ʾָ����������ķ���
	//�ú���������Ϊ�жϹ��ߴ�����������������ײ��������ײ��ķ��߷�����Ϊ�������Եķ���
	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

//����һ�����麯���ĳ����࣬��������Ա�����������
class hittable {
public:
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
	//bounding_box()�����жϸ�hittable�Ƿ���а�Χ��
	//��û�а�Χ�У��򷵻�false�����а�Χ�У�����Χ�и�ֵ����������output_box��
	//t0��t1Ϊ���ǵ��˶�������˶���ʼ����ֹʱ��
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const = 0;

	virtual double pdf_value(const point3& o, const vec3& v) const {
		return 0.0;
	}
	
	virtual vec3 random(const vec3& o) {
		return vec3(1, 0, 0);
	}

};


//ʵ�������ƽ��
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
	//ƽ�ƺ�����İ�Χ�У�Ϊԭ���İ�Χ�еĵ��ټ�һƽ����
	output_box = aabb(output_box.min() + offset, output_box.max() + offset);
	
	return true;
}

bool translate::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	//�ڹ��߷��򲻱������£�������������ķ������ƶ����Դ������岻�������£�ģ���ƶ���Ч��
	ray moved_r = ray(r.origin() - offset, r.direction(), r.time());

	if (!ptr->hit(moved_r, t_min, t_max, rec))
		return false;
	
	//rec.pΪ��ײ���ĵ��ʵ��λ�ã���Ҫ��������ƫ�������ı�Ϊ�ƶ���ĵ�
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

//��y����ʱ����ת�ȵĹ�ʽΪ
//x` = xcos�� + zsin��
//z` = -xsin�� + ycos��
rotate_y::rotate_y(shared_ptr<hittable> p, double angle) {
	ptr = p;
	auto rad = degrees_to_radians(angle);
	sin_theta = sin(rad);
	cos_theta = cos(rad);

	hasbox = p->bounding_box(0, 1, bbox);

	point3 min(infinity, infinity, infinity);
	point3 max(-infinity, -infinity, -infinity);
	//��ԭ����Χ�еĵ�������ϣ������µ����ֵ����Сֵ��
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
	
	//��ʵ������ƽ�Ƶķ�����ͬ
	//�Ƚ�����ԭ��ͷ�����ת-�ȣ�����ģ��������ת�ȵ�Ч��

	origin[0] = r.origin()[0] * cos_theta - r.origin()[2] * sin_theta;
	origin[2] = r.origin()[0] * sin_theta + r.origin()[2] * cos_theta;

	direction[0] = r.direction()[0] * cos_theta - r.direction()[2] * sin_theta;
	direction[2] = r.direction()[0] * sin_theta + r.direction()[2] * cos_theta;

	ray rotate_r(origin, direction, r.time());

	if (!ptr->hit(rotate_r, t_min, t_max, rec))
		return false;
	
	auto p = rec.p;
	auto normal = rec.normal;

	//�ٽ���õĵ�ͷ��߷�����ת�ȣ��ı�Ϊ��ת��ĵ�ͷ���
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