#ifndef LEVEL_H
#define LEVEL_H

#include "Object.h"
#include "Light.h"

class Level {
protected:
    sycl::queue* queue;

    std::string filepath, name;

    std::vector<Object> objects;
    std::vector<point_light_t> point_lights;
    unsigned int point_light_count;
    ambient_light_t ambient_light;

    std::vector<d_ModelInstance> h_model_instances;
    unsigned int model_instance_count;

    d_ModelInstance* d_model_instances;

    Camera* camera_ptr;

    void load_from(std::string);
public:
    Level(std::string, Camera*, sycl::queue*);

    void upload_instances();

    d_ModelInstance* get_d_model_instances() { return this->d_model_instances; }
    unsigned int get_instance_count() { return this->model_instance_count; }

    void add_model_instance(d_ModelInstance);
};

#endif