#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera{
public:
	camera(point3 lookfrom, point3 lookat, vec3 vup,
        double vfov, double aspect_ratio,
        double aperture,
        double focus_dist,
        double _time0 = 0.0,
        double _time1 = 0.0
        ) {
        //vfovΪ��ֱfov(�Ƕ���)
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);

        //�������ͼ��С
        auto viewport_height = 2 * h;
        auto viewport_width = aspect_ratio * viewport_height;
        auto focal_length = 1.0;

       
        //�����������ϵ
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        //�۲��λ��
        origin = lookfrom;

        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        //����ƽ�������ԭ�����������ϵ�µ�λ��
        //���ڼ������ƽ���µĵ����������ϵ�µ�λ�ã��Լ�����߷���
        left_lower_corner = origin - horizontal / 2 - vertical / 2 - w * focus_dist;

        lens_radius = aperture / 2;
        
        time0 = _time0;
        time1 = _time1;
	}
    //����ģ��
    ray get_ray(double s, double t) const{
        vec3 roff = lens_radius * random_in_unit_disk();
        vec3 offset = u * roff.x() + v * roff.y();
        //�ڲ���һ�������time0��time1֮��Ĺ��ߣ�����ģ��������Ŵ򿪵�˲�䣬���������˶�ģ��
        return ray(origin + offset, 
                    left_lower_corner + s * horizontal + t * vertical - origin - offset,
                    random_double(time0, time1)
        );
    }
private:
    point3 origin;
    vec3 horizontal;
    vec3 vertical;
    point3 left_lower_corner;
    vec3 u, v, w; //�������ϵ
    double lens_radius;
    double time0, time1;    //������ſ������رյ�ʱ�䣻
};

#endif