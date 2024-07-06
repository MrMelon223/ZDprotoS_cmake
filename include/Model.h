#ifndef MODEL_H
#define MODEL_H

#include "Helper.h"

typedef struct d_Model {
	sycl::queue* queue;

	vec3_t* vertex_positions;
	vec3_t* vertex_normals;
	color_t* vertex_colors;
	uv_t* vertex_uvs;
	unsigned int* vertex_count;

	tri_t* triangle_indices;
	vec3_t* triangle_normals;
	unsigned int* triangle_count;

	unsigned int* material_index;
};

class glModel {
private:
	void load_from(std::string);
protected:
	std::string filepath;
	std::string name;

	std::vector<vec3_t> vertex_positions;
	std::vector<vec3_t> vertex_normals;
	std::vector<color_t> vertex_colors;
	std::vector<uv_t> vertex_uvs;

	std::vector<tri_t> triangle_indices;
	std::vector<vec3_t> triangle_normals;

public:
	glModel(std::string, std::string);

	d_Model to_gpu(sycl::queue*);

	std::string get_name() { return this->name; }

	unsigned int get_triangle_count() { return static_cast<unsigned int>(this->triangle_indices.size()); }
};

typedef struct d_ModelInstance {
	unsigned int model_index;
	vec3_t position;
	vec3_t rotation;
	unsigned int triangle_count;
	bool show;
	float scale;
};

d_ModelInstance create_instance(unsigned int, vec3_t, vec3_t, unsigned int, bool, float);

#endif
