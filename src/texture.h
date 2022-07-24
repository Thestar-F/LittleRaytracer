#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.h"
#include "perlin.h"
#include "rtw_stb_image.h"

//����:��Ա����value������������u, v���ض�Ӧ�����ɫֵ
class texture {
public:
	virtual color value(double u, double v, const point3& p) const = 0;
};


//��ɫ����
class solid_color : public texture{
private:
    //��ɫ����ĳ�Աcolor_value���൱��һ����ɫ��
	color color_value;

public:
	solid_color(){}
    //����ֱ�Ӵ���һ����ɫcolor��solid_color���й���
	solid_color(color c) : color_value(c) {}
    //���Դ���һ����ɫ��RGBֵ��solid_color���й���
	solid_color(double r, double g, double b) : color_value(color(r, g, b)) {}

    //��ɫ�����ֵ����������u,v�޹أ�ֱ�ӷ���color_value����
	virtual color value(double u, double v, const point3& p) const override {
		return color_value;
	}

};


//���̸�����
class checker_texture : public texture {
public:
	shared_ptr<texture> even;
	shared_ptr<texture> odd;

public:
	checker_texture() {}

    //���̸��������ֲ�ͬ��texture�����ǽ������
	checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd) : even(_even), odd(_odd) {}
    //�������ִ�ɫ����
	checker_texture(color a , color b) : even(make_shared<solid_color>(a)), odd(make_shared<solid_color>(b)) {}

	virtual color value(double u, double v, const point3& p) const override {
        //һ��sin�������������ɽ����Ч��
		auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}

};


//perlin noise����

class noise_texture : public texture {
public:
	perlin noise;
	double scale;
public:
	noise_texture() {}
	noise_texture(double sc) : scale(sc) {}

    //scale�ɸı�������Ƶ��
	virtual color value(double u, double v, const point3& p) const override {
		return color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
	}

};


//������ͼ
class image_texture : public texture {
public:
    const static int bytes_per_pixel = 3;

    image_texture()
        : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

    image_texture(const char* filename) {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(
            filename, &width, &height, &components_per_pixel, components_per_pixel);

        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    ~image_texture() {
        delete data;
    }

    virtual color value(double u, double v, const vec3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (data == nullptr)
            return color(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width)  i = width - 1;
        if (j >= height) j = height - 1;

        const auto color_scale = 1.0 / 255.0;
        auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

        return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    unsigned char* data;
    int width, height;
    int bytes_per_scanline;
};

#endif