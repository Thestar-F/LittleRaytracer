#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "texture.h"
#include "pdf.h"

struct hit_record;



struct scatter_record {
	ray specular_ray;
	bool is_specular;
	color attenuation;
	shared_ptr<pdf> pdf_ptr;
};



class material {
public:
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const {
		return false;
	}
	//每个材质都能发射光，但如果不重写该函数的话，默认材质发光值为color（0, 0, 0）
	virtual color emitted(const ray& r_in, const hit_record& rec,
		double u, double v, const point3& p) const{
		return color(0, 0, 0);
	}

	virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
	{
		return 0;
	}
};


class lambertian :public material {

public:
	//将原本color的成员换为texture，可以用漫反射材质实现各种纹理
	lambertian(const color& a) : albedo(make_shared<solid_color>(a)){ }
	lambertian(shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const  override {
		srec.is_specular = false;
		srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = make_shared<cos_pdf>(rec.normal);
		return true;
	}

	double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
		auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
		return cosine < 0 ? 0 : cosine / pi;
	}

public:
	shared_ptr<texture> albedo;
};

class metal :public material {

public:
	metal(const color& a, double f) : albedo(a), fuzz(f) { }

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const  override {
		//反射光线的方向为镜面反射方向
		auto reflected = reflect(unit_vector(r_in.direction()), rec.normal);

		//模糊的反射方向
		srec.specular_ray = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
		srec.attenuation = albedo;
		srec.is_specular = true;
		srec.pdf_ptr = 0;
		
		return true;
	}

public:
	color albedo;
	double fuzz;	//模糊参数
};


class dielectric :public material {

public:
	dielectric(double index_of_refraction) : ir(index_of_refraction) { }

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const  override {
		srec.is_specular = true;
		srec.pdf_ptr = nullptr;
		srec.attenuation = color(1.0, 1.0, 1.0);


		double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;
		vec3 r_in_unit = unit_vector(r_in.direction());

		double cos_theta = fmin(dot(-r_in_unit, rec.normal), 1.0);
		double sin_theta = sqrt(1 - cos_theta * cos_theta);
		//如果大于0，则不能发生折射，光线全部被反射
		bool cannot_refract = refraction_ratio * sin_theta > 1.0;

		vec3 direction;
		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
			direction = reflect(r_in_unit, rec.normal);
		}
		else {
			direction = refract(r_in_unit, rec.normal, refraction_ratio);
		}
		
		srec.specular_ray = ray(rec.p, direction, r_in.time());
		
		return true;
	}


public:
	//color albedo;
	double ir;	//折射率

private:
	//Schlick Approximation
	static double reflectance(double cosine, double ref_idx) {
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};


//光源
class diffuse_light : public material {
public:
	//光源的发光纹理
	shared_ptr<texture> emit;
public:
	diffuse_light(shared_ptr<texture> a) : emit(a) {}
	diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

	//光源不反射光
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const  override {
		return false;
	}

	//光源发出的光，返回发光纹理的value
	virtual color emitted(const ray& r_in, const hit_record& rec,
		double u, double v, const point3& p) const {
		if (rec.front_face)
			return emit->value(rec.u, rec.v, rec.p);
		else
			return color(0, 0, 0);
	}

};



//雾状材质
class isotropic : public material {
public:
	shared_ptr<texture> albedo;

public:
	//若传入参数为color，则将材质赋值为纯色纹理
	isotropic(color c) : albedo(make_shared<solid_color>(c)) {}

	isotropic(shared_ptr<texture> a) : albedo(a) {}


	virtual bool scatter(
		const ray& r_in, const hit_record& rec, scatter_record& srec
	) const  override {
		////雾状材质，散射方向是随机的
		//scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
		//attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}
};



#endif