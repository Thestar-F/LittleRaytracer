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
        //vfov为垂直fov(角度制)
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);

        //摄像机视图大小
        auto viewport_height = 2 * h;
        auto viewport_width = aspect_ratio * viewport_height;
        auto focal_length = 1.0;

       
        //定义相机坐标系
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        //观察点位置
        origin = lookfrom;

        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        //成像平面的坐标原点在相机坐标系下的位置
        //用于计算成像平面下的点在相机坐标系下的位置，以计算光线方向
        left_lower_corner = origin - horizontal / 2 - vertical / 2 - w * focus_dist;

        lens_radius = aperture / 2;
        
        time0 = _time0;
        time1 = _time1;
	}
    //景深模糊
    ray get_ray(double s, double t) const{
        vec3 roff = lens_radius * random_in_unit_disk();
        vec3 offset = u * roff.x() + v * roff.y();
        //在产生一个随机的time0到time1之间的光线，用于模拟相机快门打开的瞬间，用于生成运动模糊
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
    vec3 u, v, w; //相机坐标系
    double lens_radius;
    double time0, time1;    //相机快门开启、关闭的时间；
};

#endif