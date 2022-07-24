#pragma once

#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <vector>
#include <memory>
#include "hittable.h"

using std::shared_ptr;
using std::make_shared;

//����һ���࣬����Ϊ����ײ����ļ��ϣ��ǿ���ײ��hittable������
//��Ա����Ϊ��ſ���ײ����ָ��ļ���
//��Ա����Ϊ��д����ײ��⺯��hit
class hittable_list : public hittable {
public:
	//��Ĺ��캯��
	hittable_list(){ }
	//���ָ�룬��Ҫ�ø����ָ��ָ������Ķ����Դ�ʹ�ö�̬
	hittable_list(shared_ptr<hittable> object) {
		add(object);
	}
	//add��һ������ײ������ӵ�����ļ�����
	void add(shared_ptr<hittable> object) {
		objects.push_back(object);
	}
	//�Ѵ�ſ���ײ������������
	void clear() {
		objects.clear();
	}
	//��д����ײ��⺯��
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	//output_box�������ײ���ϵİ�Χ��
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;


	virtual double pdf_value(const point3& o, const vec3& v) const override;

	virtual vec3 random(const vec3& o) const;


public:
	std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
	//tmp_rec������¼ÿһ����ײʱ�Ĳ�����Ϣ
	hit_record tmp_rec;
	//closest_so_far������¼����������������t,��ʼΪt_max
	double closest_so_far = t_max;
	//����Ƿ���ײ������
	bool hit_anything = false;
	
	for (const auto& object : objects) {
		if (object->hit(r, t_min, closest_so_far, tmp_rec)) {
			//����objects�е�ÿһ�����壬���������ײ��������ײ����rec,ͬʱ��������ײ�����t
			hit_anything = true;
			closest_so_far = tmp_rec.t;
			rec = tmp_rec;
		}
	}

	return hit_anything;
}

//����ײ�б�İ�Χ�У������б�����������İ�Χ�еİ�Χ��
bool hittable_list::bounding_box(double t0, double t1, aabb& output_box) const {
	//�����ǰ�б�Ϊ�գ��򲻴��ڰ�Χ��
	if (objects.empty()) { return false; }

	aabb tmp_box;
	//�ж��Ƿ��ǵ�һ����Χ��
	bool first_box = true;

	for (const auto& object : objects) {
		//����б�����һ�����岻���ڰ�Χ�У������岻���ڰ�Χ��
		if (!object->bounding_box(t0, t1, tmp_box)) { return false; }
		output_box = first_box ? tmp_box : surrounding_box(tmp_box, output_box);
		//��һ����Χ�и�ֵ�����
		first_box = false;
	}

	return true;
}


double hittable_list::pdf_value(const point3& o, const vec3& v) const {
	double ret = 0.0;
	double weight = 1.0 / objects.size();
	for (int i = 0; i < objects.size(); i++)
		ret += (weight * objects[i]->pdf_value(o, v));

	return ret;
}


vec3 hittable_list::random(const vec3& o) const{
	int count = static_cast<int>(objects.size());
	return objects[random_int(0, count - 1)]->random(o);
}


#endif