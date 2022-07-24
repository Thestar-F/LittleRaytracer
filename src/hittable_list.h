#pragma once

#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <vector>
#include <memory>
#include "hittable.h"

using std::shared_ptr;
using std::make_shared;

//定义一个类，该类为可碰撞物体的集合，是可碰撞类hittable的子类
//成员变量为存放可碰撞物体指针的集合
//成员函数为重写的碰撞检测函数hit
class hittable_list : public hittable {
public:
	//类的构造函数
	hittable_list(){ }
	//存放指针，是要用父类的指针指向子类的对象以此使用多态
	hittable_list(shared_ptr<hittable> object) {
		add(object);
	}
	//add将一个可碰撞物体添加到该类的集合中
	void add(shared_ptr<hittable> object) {
		objects.push_back(object);
	}
	//把存放可碰撞物体的容器清空
	void clear() {
		objects.clear();
	}
	//重写的碰撞检测函数
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	//output_box输出可碰撞集合的包围盒
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const override;


	virtual double pdf_value(const point3& o, const vec3& v) const override;

	virtual vec3 random(const vec3& o) const;


public:
	std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
	//tmp_rec用来记录每一次碰撞时的参数信息
	hit_record tmp_rec;
	//closest_so_far用来记录碰到的最近的物体的t,初始为t_max
	double closest_so_far = t_max;
	//检测是否碰撞到物体
	bool hit_anything = false;
	
	for (const auto& object : objects) {
		if (object->hit(r, t_min, closest_so_far, tmp_rec)) {
			//遍历objects中的每一个物体，如果发生碰撞，更新碰撞参数rec,同时更新所碰撞物体的t
			hit_anything = true;
			closest_so_far = tmp_rec.t;
			rec = tmp_rec;
		}
	}

	return hit_anything;
}

//可碰撞列表的包围盒：构建列表中所有物体的包围盒的包围盒
bool hittable_list::bounding_box(double t0, double t1, aabb& output_box) const {
	//如果当前列表为空，则不存在包围盒
	if (objects.empty()) { return false; }

	aabb tmp_box;
	//判断是否是第一个包围盒
	bool first_box = true;

	for (const auto& object : objects) {
		//如果列表中有一个物体不存在包围盒，则整体不存在包围盒
		if (!object->bounding_box(t0, t1, tmp_box)) { return false; }
		output_box = first_box ? tmp_box : surrounding_box(tmp_box, output_box);
		//第一个包围盒赋值已完成
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