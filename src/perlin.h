#pragma once

#ifndef PERLIN_H
#define PERLIN_H

#include "rtweekend.h"



//柏林噪声生成步骤
//1.生成随机的排列表和梯度表
//排列表用于获取点在梯度表中的索引，梯度表中对应点生成的随机梯度向量
//2.对于点p，找到它周围用于插值的几个整数点，对于3维点p，则找到8个点
//利用c[2][2][2]记录这8个点梯度向量，然后插值计算p的值
//3.生成插值系数t, t = 3*t^2 - 2*t^3, t为点p到插值点的距离
//4.计算p到每个插值点的方向向量，再与对应点的梯度进行点乘
//5.利用插值公式，计算得到p点的返回值
//6.可利用turb函数，获取多个尺度的噪声值的混杂
//noise函数可能返回赋值，直接使用的话需要加0.5 * （1.0 + noise(p))，将输出值调整为0到1
//turb函数最后进行了abs操作，返回值可以直接使用


class perlin {
private:
	static const int point_count = 256;
	vec3* randvec;
	int* perm_x;
	int* perm_y;
	int* perm_z;


	//生成随机的排列表
	static int* perlin_generate_perm() {
		auto p = new int[point_count];

		for (int i = 0; i < point_count; i++) {
			p[i] = i;
		}

		permute(p, point_count);

		return p;
	}
	//将数组p中顺序进行随机打乱
	static void permute(int* p, int n) {
		for (int i = n - 1; i >= 0; i--) {
			int tar = random_int(0, i);
			int tmp = p[i];
			p[i] = p[tar];
			p[tar] = tmp;
		}
	}

	static double perlin_interp(vec3 c[2][2][2], double u, double v, double w) {
		//为了使噪声更加平滑，改进的插值系数
		//t = 3*t^2 - 2*t^3
		auto uu = u * u * (3 - 2 * u);
		auto vv = v * v * (3 - 2 * v);
		auto ww = w * w * (3 - 2 * w);
		auto accum = 0.0;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					//weight_v为点p与周围8个点的方向向量
					vec3 weight_v(u - i, v - j, w - k);
					//插值公式
					accum +=
						(i * uu + (1 - i) * (1 - uu)) *
						(j * vv + (1 - j) * (1 - vv)) *
						(k * ww + (1 - k) * (1 - ww)) *
						dot(c[i][j][k], weight_v);
					//dot(c[i][j][k], weight_v)对应点的梯度与对应点方向向量的点积
				}
			}
		}
		return accum;
	}


public:
	perlin() {
		//生成随机的梯度表
		randvec = new vec3[point_count];
		for (int i = 0; i < point_count; i++) {
			randvec[i] = unit_vector(vec3::random(-1, 1));
		}
		//获取x、y、z三个维度的梯度表
		perm_x = perlin_generate_perm();
		perm_y = perlin_generate_perm();
		perm_z = perlin_generate_perm();
	}
	~perlin() {
		delete[] randvec;
		delete[] perm_x;
		delete[] perm_y;
		delete[] perm_z;
	}

	double noise(const point3& p) const{
		//获取x、y、z三个维度上的值与距它们最近的且小于它们的坐标的最大值的距离
		auto u = p.x() - floor(p.x());
		auto v = p.y() - floor(p.y());
		auto w = p.z() - floor(p.z());

		//获取距离p最近的最大坐标
		auto i = static_cast<int>(floor(p.x()));
		auto j = static_cast<int>(floor(p.y()));
		auto k = static_cast<int>(floor(p.z()));
		vec3 c[2][2][2];

		//使用p附近的8个点，获取8个点在排序表中的值perm，然后用该值去获取相应的梯度值c
		for (int di = 0; di < 2; di++) {
			for (int dj = 0; dj < 2; dj++) {
				for (int dk = 0; dk < 2; dk++) {
					c[di][dj][dk] = randvec[
						perm_x[(i + di) & 255] ^
						perm_y[(j + dj) & 255] ^
						perm_z[(k + dk) & 255] 
					];
				}
			}
		}
		return perlin_interp(c, u, v, w);
	}

	//将多个尺度的噪声进行混杂
	double turb(const point3& p, int depth = 7) const {
		auto accum = 0.0;
		auto temp_p = p;
		auto weight = 1.0;

		for (int i = 0; i < depth; i++) {
			accum += weight * noise(temp_p);
			temp_p *= 2;
			weight *= 0.5;
		}
		return abs(accum);
	}
};



#endif