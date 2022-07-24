#pragma once

#ifndef PERLIN_H
#define PERLIN_H

#include "rtweekend.h"



//�����������ɲ���
//1.������������б���ݶȱ�
//���б����ڻ�ȡ�����ݶȱ��е��������ݶȱ��ж�Ӧ�����ɵ�����ݶ�����
//2.���ڵ�p���ҵ�����Χ���ڲ�ֵ�ļ��������㣬����3ά��p�����ҵ�8����
//����c[2][2][2]��¼��8�����ݶ�������Ȼ���ֵ����p��ֵ
//3.���ɲ�ֵϵ��t, t = 3*t^2 - 2*t^3, tΪ��p����ֵ��ľ���
//4.����p��ÿ����ֵ��ķ��������������Ӧ����ݶȽ��е��
//5.���ò�ֵ��ʽ������õ�p��ķ���ֵ
//6.������turb��������ȡ����߶ȵ�����ֵ�Ļ���
//noise�������ܷ��ظ�ֵ��ֱ��ʹ�õĻ���Ҫ��0.5 * ��1.0 + noise(p))�������ֵ����Ϊ0��1
//turb������������abs����������ֵ����ֱ��ʹ��


class perlin {
private:
	static const int point_count = 256;
	vec3* randvec;
	int* perm_x;
	int* perm_y;
	int* perm_z;


	//������������б�
	static int* perlin_generate_perm() {
		auto p = new int[point_count];

		for (int i = 0; i < point_count; i++) {
			p[i] = i;
		}

		permute(p, point_count);

		return p;
	}
	//������p��˳������������
	static void permute(int* p, int n) {
		for (int i = n - 1; i >= 0; i--) {
			int tar = random_int(0, i);
			int tmp = p[i];
			p[i] = p[tar];
			p[tar] = tmp;
		}
	}

	static double perlin_interp(vec3 c[2][2][2], double u, double v, double w) {
		//Ϊ��ʹ��������ƽ�����Ľ��Ĳ�ֵϵ��
		//t = 3*t^2 - 2*t^3
		auto uu = u * u * (3 - 2 * u);
		auto vv = v * v * (3 - 2 * v);
		auto ww = w * w * (3 - 2 * w);
		auto accum = 0.0;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					//weight_vΪ��p����Χ8����ķ�������
					vec3 weight_v(u - i, v - j, w - k);
					//��ֵ��ʽ
					accum +=
						(i * uu + (1 - i) * (1 - uu)) *
						(j * vv + (1 - j) * (1 - vv)) *
						(k * ww + (1 - k) * (1 - ww)) *
						dot(c[i][j][k], weight_v);
					//dot(c[i][j][k], weight_v)��Ӧ����ݶ����Ӧ�㷽�������ĵ��
				}
			}
		}
		return accum;
	}


public:
	perlin() {
		//����������ݶȱ�
		randvec = new vec3[point_count];
		for (int i = 0; i < point_count; i++) {
			randvec[i] = unit_vector(vec3::random(-1, 1));
		}
		//��ȡx��y��z����ά�ȵ��ݶȱ�
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
		//��ȡx��y��z����ά���ϵ�ֵ��������������С�����ǵ���������ֵ�ľ���
		auto u = p.x() - floor(p.x());
		auto v = p.y() - floor(p.y());
		auto w = p.z() - floor(p.z());

		//��ȡ����p������������
		auto i = static_cast<int>(floor(p.x()));
		auto j = static_cast<int>(floor(p.y()));
		auto k = static_cast<int>(floor(p.z()));
		vec3 c[2][2][2];

		//ʹ��p������8���㣬��ȡ8������������е�ֵperm��Ȼ���ø�ֵȥ��ȡ��Ӧ���ݶ�ֵc
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

	//������߶ȵ��������л���
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