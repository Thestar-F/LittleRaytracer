#pragma once

#ifndef BVH_H
#define BVH_H


//BVH调用流程
//1.传入hittable_list和运动物体的运动起始和停止时间t0,t1

//2.包含上述三个参数的构造函数会调用更复杂参数列表的构造函数
//该构造函数参数列表包括：存储着若干hittable的容器，需要构建BVH的物体在容器中的起始和结束位置[start, end)，以及时间t0,t1

//3.进入构造函数后，随机生成0,1,2中任意一个随机数，分别对应x、y、z轴，即选择哪一个轴进行构建BVH

//4.判断物体一共有几个。若只有一个，则无需构建BVH，左右孩子均为这个hittable
//若有两个物体，也无需构建BVH，左孩子为包围盒较小的物体，右孩子为包围盒较大的物体
//若有三个及以上的物体，则按照包围盒从小到的顺序进行排序，前一半构建左孩子BVH，后一半构建右孩子BVH（递归的过程）

//5.调用bounding_box()函数计算左右孩子的包围盒，然后将它们传入surrounding_box()函数，构建父节点的包围盒

//6.将bvh_node传入获取颜色的函数用于计算光线是否与物体发生碰撞，即调用hit函数

//7.bvh_node的hit会首先调用包围盒aabb的hit，若与包围盒相交，则去递归的调用左右孩子的hit函数
//当左右孩子不为BVH，而是具体的hittable时，则将hittable->hit结果返回，返回递归结果



#include "hittable_list.h"
#include <algorithm>	//为了使用std::sort函数



//bvh_node的成员：左右孩子和包围盒
//不确定左右孩子是bvh_node还是具体的碰撞物，所以用父类hittable
class bvh_node : public hittable {
public:
	shared_ptr<hittable> left;
	shared_ptr<hittable> right;
	aabb box;
public:
	bvh_node() {}
	//传入时只需传入hittable_list、t0、t1，然后再调用参数列表更丰富的构造函数去构造
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



//一系列比较函数，根据所选轴axis进行比较
inline bool box_compare(const shared_ptr<hittable>& a, const shared_ptr<hittable>& b, int axis) {
	aabb box0;
	aabb box1;

	if (!a->bounding_box(0, 0, box0) || !b->bounding_box(0, 0, box1)) {
		std::cerr << "No bounding box in bvh_node constructor .\n";
	}

	//按包围盒最小边界从小到大的顺序排列
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


//bvh_node的构造函数
bvh_node::bvh_node(
	const std::vector<shared_ptr<hittable>>& src_objects,
	size_t start, size_t end, double t0, double t1) {
	//复制一个可供修改的碰撞表的数组
	auto objects = src_objects;
	//随机选择一个轴
	int axis = random_int(0, 2);
	auto comparator = (axis == 0) ? box_x_compare
					: (axis == 1) ? box_y_compare
					: box_z_compare;

	//所需分类的hittable的个数
	size_t object_span = end - start;

	//如果list中只有一个物体，则左右孩子均是它自己
	if (object_span == 1) {
		left = objects[start];
		right = objects[start];
	}	//如果list中有两个物体，则选取边界小的当左孩子，另一个当右孩子
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
	else {	//当list中不止两个物体时，按照包围盒边界大小进行排序，划分左右孩子，构造两个bvh_node
		std::sort(objects.begin() + start, objects.begin() + end, comparator);
		size_t mid = start + (object_span >> 1);
		left = make_shared<bvh_node>(objects, start, mid, t0, t1);
		right = make_shared<bvh_node>(objects, mid, end, t0, t1);
	}
	//构造左孩子和右孩子的包围盒
	aabb box0, box1;
	if (!left->bounding_box(t0, t1, box0) || !right->bounding_box(t0, t1, box1)) {
		std::cerr << "No bounding box in bvh_node constructor.\n";
	}
	//由左孩子和右孩子的包围盒构造父节点的包围盒
	box = surrounding_box(box0, box1);
}


#endif
