#pragma once

#ifndef PDF_H
#define PDF_H

#include "rtweekend.h"

inline vec3 random_cos_dir() {
	double r1 = random_double();
	double r2 = random_double();

	double z = sqrt(1 - r2);

	double phi = 2 * pi * r1;
	double x = cos(phi) * sqrt(r2);
	double y = sin(phi) * sqrt(r2);

	return vec3(x, y, z);
}



inline vec3 random_to_sphere(double radius, double distance_squared) {
	double r1 = random_double();
	double r2 = random_double();
	double z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

	double phi = 2 * pi * r1;
	double x = cos(phi) * sqrt(1 - z * z);
	double y = sin(phi) * sqrt(1 - z * z);

	return vec3(x, y, z);
}




class onb {
public:
	vec3 axis[3];

public:
	onb() {};

	inline vec3 operator[](int i) const { return axis[i]; }

	vec3 u() const { return axis[0]; }
	vec3 v() const { return axis[1]; }
	vec3 w() const { return axis[2]; }

	vec3 local(double a, double b, double c) const {
		return a * u() + b * v() + c * w();
	}

	vec3 local(const vec3& a) const {
		return a.x() * u() + a.y() * v() + a.z() * w();
	}

	void build(const vec3&);
};


void onb::build(const vec3& n) {
	axis[2] = n;
	vec3 a = (fabs(n.x()) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);

	axis[1] = unit_vector(cross(n, a));
	axis[0] = unit_vector(cross(axis[1], n));
}







class pdf {
public:

	virtual ~pdf() {}

	virtual double value(const vec3& direction) const = 0;
	virtual vec3 generate() const = 0;

};


class cos_pdf : public pdf {
public:
	onb uvw;

public:
	cos_pdf(const vec3& w) { uvw.build(w); }

	virtual double value(const vec3& direction) const override{
		double cosine = dot(unit_vector(direction), uvw.w());
		return (cosine < 0) ? 0 : cosine / pi;
	}

	virtual vec3 generate() const override{
		return uvw.local(random_cos_dir());
	}

};


class hittable_pdf : public pdf {
public:
	point3 o;
	hittable_list ptr;

public:
	hittable_pdf(hittable_list p, const vec3& origin) : ptr(p), o(origin) {}

	virtual double value(const vec3& direction) const {
		return ptr.pdf_value(o, direction);
	}
	virtual vec3 generate() const {
		return ptr.random(o);
	}
};



class mixture_pdf : public pdf {
public:
	shared_ptr<pdf> p[2];

public:
	mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1) {
		p[0] = p0;
		p[1] = p1;
	}

	virtual double value(const vec3& direction) const {
		return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
	}
	virtual vec3 generate() const {
		if (random_double() < 0.5)
			return p[0]->generate();
		else
			return p[1]->generate();
	}

};


#endif