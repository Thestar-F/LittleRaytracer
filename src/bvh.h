#pragma once

#ifndef BVH_H
#define BVH_H


//BVH��������
//1.����hittable_list���˶�������˶���ʼ��ֹͣʱ��t0,t1

//2.�����������������Ĺ��캯������ø����Ӳ����б�Ĺ��캯��
//�ù��캯�������б�������洢������hittable����������Ҫ����BVH�������������е���ʼ�ͽ���λ��[start, end)���Լ�ʱ��t0,t1

//3.���빹�캯�����������0,1,2������һ����������ֱ��Ӧx��y��z�ᣬ��ѡ����һ������й���BVH

//4.�ж�����һ���м�������ֻ��һ���������蹹��BVH�����Һ��Ӿ�Ϊ���hittable
//�����������壬Ҳ���蹹��BVH������Ϊ��Χ�н�С�����壬�Һ���Ϊ��Χ�нϴ������
//�������������ϵ����壬���հ�Χ�д�С����˳���������ǰһ�빹������BVH����һ�빹���Һ���BVH���ݹ�Ĺ��̣�

//5.����bounding_box()�����������Һ��ӵİ�Χ�У�Ȼ�����Ǵ���surrounding_box()�������������ڵ�İ�Χ��

//6.��bvh_node�����ȡ��ɫ�ĺ������ڼ�������Ƿ������巢����ײ��������hit����

//7.bvh_node��hit�����ȵ��ð�Χ��aabb��hit�������Χ���ཻ����ȥ�ݹ�ĵ������Һ��ӵ�hit����
//�����Һ��Ӳ�ΪBVH�����Ǿ����hittableʱ����hittable->hit������أ����صݹ���



#include "hittable_list.h"
#include <algorithm>	//Ϊ��ʹ��std::sort����



//bvh_node�ĳ�Ա�����Һ��ӺͰ�Χ��
//��ȷ�����Һ�����bvh_node���Ǿ������ײ������ø���hittable
class bvh_node : public hittable {
public:
	shared_ptr<hittable> left;
	shared_ptr<hittable> right;
	aabb box;
public:
	bvh_node() {}
	//����ʱֻ�贫��hittable_list��t0��t1��Ȼ���ٵ��ò����б���ḻ�Ĺ��캯��ȥ����
	bvh_node(
		const hittable_list& list, double t0, double t1) :
		bvh_node(list.objects, 0, list.objects.size(), t0, t1) 
		{}

		bvh_node(
			const std::vector<shared_ptr<hittable>>& src_objects,
			size_t start, size_t end, double t0, double t1);

		virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
		virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;
};


bool bvh_node::bounding_box(double t0, double t1, aabb& output_box) const {
	output_box = box;
	return true;
}

bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	if (!box.hit(r, t_min, t_max)) { return false; }

	bool hit_left = left->hit(r, t_min, t_max, rec);
	bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

	return hit_left || hit_right;
}



//һϵ�бȽϺ�����������ѡ��axis���бȽ�
inline bool box_compare(const shared_ptr<hittable>& a, const shared_ptr<hittable>& b, int axis) {
	aabb box0;
	aabb box1;

	if (!a->bounding_box(0, 0, box0) || !b->bounding_box(0, 0, box1)) {
		std::cerr << "No bounding box in bvh_node constructor .\n";
	}

	//����Χ����С�߽��С�����˳������
	return box0.min().e[axis] < box1.min().e[axis];
}

bool box_x_compare(const shared_ptr<hittable>& a, const shared_ptr<hittable>& b) {
	return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<hittable>& a, const shared_ptr<hittable>& b) {
	return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<hittable>& a, const shared_ptr<hittable>& b) {
	return box_compare(a, b, 2);
}


//bvh_node�Ĺ��캯��
bvh_node::bvh_node(
	const std::vector<shared_ptr<hittable>>& src_objects,
	size_t start, size_t end, double t0, double t1) {
	//����һ���ɹ��޸ĵ���ײ�������
	auto objects = src_objects;
	//���ѡ��һ����
	int axis = random_int(0, 2);
	auto comparator = (axis == 0) ? box_x_compare
					: (axis == 1) ? box_y_compare
					: box_z_compare;

	//��������hittable�ĸ���
	size_t object_span = end - start;

	//���list��ֻ��һ�����壬�����Һ��Ӿ������Լ�
	if (object_span == 1) {
		left = objects[start];
		right = objects[start];
	}	//���list�����������壬��ѡȡ�߽�С�ĵ����ӣ���һ�����Һ���
	else if (object_span == 2) {
		if (comparator(objects[start], objects[start + 1])) {
			left = objects[start];
			right = objects[start + 1];
		}
		else {
			left = objects[start + 1];
			right = objects[start];
		}
	}
	else {	//��list�в�ֹ��������ʱ�����հ�Χ�б߽��С�������򣬻������Һ��ӣ���������bvh_node
		std::sort(objects.begin() + start, objects.begin() + end, comparator);
		size_t mid = start + (object_span >> 1);
		left = make_shared<bvh_node>(objects, start, mid, t0, t1);
		right = make_shared<bvh_node>(objects, mid, end, t0, t1);
	}
	//�������Ӻ��Һ��ӵİ�Χ��
	aabb box0, box1;
	if (!left->bounding_box(t0, t1, box0) || !right->bounding_box(t0, t1, box1)) {
		std::cerr << "No bounding box in bvh_node constructor.\n";
	}
	//�����Ӻ��Һ��ӵİ�Χ�й��츸�ڵ�İ�Χ��
	box = surrounding_box(box0, box1);
}


#endif
