#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "aabb.h"
#include "bvh.h"
#include "aarec.h"
#include "box.h"

#include <iostream>



hittable_list final_scene() {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list objects;

    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    objects.add(make_shared<xz_rec>(123, 423, 147, 412, 554, light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
        ));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("images/earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));


    auto smat = make_shared<diffuse_light>(make_shared<image_texture>("images/sun.jpg"));
    objects.add(make_shared<sphere>(point3(100, 400, 150), 80, smat));

    return objects;
}


//创建场景：cornell_box
hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rec>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rec>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(make_shared<xz_rec>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rec>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rec>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rec>(0, 555, 0, 555, 555, white));

    /*shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), aluminum);*/



    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    objects.add(box1);

    /*shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    objects.add(box2);*/

    auto glass = make_shared<dielectric>(1.5);
    objects.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));


    return objects;
}




//最大深度递归
color ray_color(const ray& r, const color& background, const hittable& world, 
    hittable_list& lights, int depth) {
    hit_record  rec;

    //如果递归已达到最大深度，则返回color（0， 0， 0）
    if (depth <= 0)
        return color(0.0, 0.0, 0.0);

    //如果光线未击中任何物体，则返回背景色
    if (!world.hit(r, 0.001, infinity, rec)) 
        return background;
    
    scatter_record srec;
    color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
    if (!rec.mat_ptr->scatter(r, rec, srec))
        return emitted;

    if (srec.is_specular) {
        return srec.attenuation *
            ray_color(srec.specular_ray, background, world, lights, depth - 1);
    }
    
    auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
    mixture_pdf p(light_ptr, srec.pdf_ptr);

    ray scattered = ray(rec.p, p.generate(), r.time());
    auto pdf_val = p.value(scattered.direction());



    return emitted
        + srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered)
        * ray_color(scattered, background, world, lights, depth - 1) / pdf_val;

}




int main() {

    //test();


    //图片大小
    auto aspect_ratio = 1.0 / 1.0;
    int image_width = 600;
    //每个像素采样个数
    int samples_per_pixel = 100;
    
    //光线最大反射次数
    const int max_depth = 50;
    //光线反射概率
    const double P = 0.95;


    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;
    color background(0, 0, 0);

    //选择一个场景，并设置相机参数
    hittable_list world;
    switch (1) {
    case 1:
        world = cornell_box();
        aspect_ratio = 1.0;
        image_width = 600;
        samples_per_pixel = 10;
        background = color(0, 0, 0);
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;
    default:
    case 2:
        world = final_scene();
        aspect_ratio = 1.0;
        image_width = 800;
        samples_per_pixel = 10000;
        background = color(0, 0, 0);
        lookfrom = point3(478, 278, -600);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;
    }

    //根据场景生成BVH
    auto world_f = bvh_node(world, 0.0, 1.0);
    auto light = make_shared<diffuse_light>(color(15, 15, 15));
    //shared_ptr<hittable> lights = make_shared<xz_rec>(213, 343, 227, 332, 554, light);
    hittable_list lights;
    lights.add(make_shared<xz_rec>(213, 343, 227, 332, 554, light));
    lights.add(make_shared<sphere>(point3(190, 90, 190), 90, light));
    
    

    //设置摄像机
    vec3 up(0, 1, 0);
    auto focus_dist = 10.0;
    int image_height = static_cast<int>(image_width / aspect_ratio);
    camera cam(lookfrom, lookat, up, vfov, aspect_ratio, aperture, focus_dist, 0.0, 1.0);



    /**********************************************************************************************/


    //render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color = { 0.0, 0.0, 0.0 };
            for (int s = 0; s < samples_per_pixel; s++) {
                //每个像素随机取samples_per_pixel个采样点，将每个采样点的值加起来。
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world_f, lights, max_depth);
            }

            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";


}
