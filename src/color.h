#pragma once

#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel) {
	//把每个颜色分量转换到[0,255]的对应值
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();


	if (r != r) r = 0.0;
	if (g != g) g = 0.0;
	if (b != b) b = 0.0;


	//根据每个像素点采样的个数，对RGB分量进行缩放
	auto scale = 1.0 / samples_per_pixel;

	//gamma2.0校正
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	//clamp函数确保每个分量都在0到1之间
	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

#endif 
