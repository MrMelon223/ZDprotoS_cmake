    // Level.cpp
#include "../include/Level.h"
#include "../include/App.h"
#include "../include/Model.h"

void Level::load_from(std::string path) {
    std::ifstream in;
	in.open(path, std::ios::in);
	if (!in) {
		std::cout << "Cannot find Level: " << path << std::endl;
		return;
	}

	std::cout << "Loading Level: " << path << std::endl;

	this->objects = *new std::vector<Object>();

	std::string line;
	std::getline(in, line);
	std::istringstream parse(line);
	size_t leng = 0;

	parse >> leng;
	std::cout << leng << " static models detected!" << std::endl;
	std::string model;

	uint32_t model_count = 0;

	for (size_t i = 0; i < leng; i++) {
		std::getline(in, line);
		std::istringstream in0(line);

		float x, y, z, x_r, y_r, z_r, scale;

		in0 >> x >> y >> z >> x_r >> y_r >> z_r >> scale >> model;
		//std::cout << model << std::endl;

		vec3_t position = vec3_t{x, y, z};
		vec3_t rotation = vec3_t{x_r, y_r, z_r};

		glModel* h_model = &HOST_MODELS[Runtime::find_model(model)];

		h_model_instances.push_back(create_instance(Runtime::find_model(model), position, rotation, h_model->get_triangle_count(), true, scale));

		std::cout << "d_model = " << this->h_model_instances.back().model_index << std::endl;

		//d_Model d_model2 = Runtime::find_host_model(model)->to_gpu();

		//d_models.push_back(DEVICE_MODELS.at(Runtime::find_host_model_index(model)));	// -> Dis piece o shite

	}
	//thrust::copy(DEVICE_MODELS.begin(), DEVICE_MODELS.end(), d_models.begin());
	size_t light_leng;
	std::string line2;
	std::getline(in, line2);
	std::istringstream parse2(line2);
	parse2 >> light_leng;
	std::cout << light_leng << " lights detected!" << std::endl;
	this->point_lights = *new std::vector<point_light_t>();
	for (size_t i = 0; i < light_leng; i++) {
		std::getline(in, line);
		std::istringstream in1(line);

		float x, y, z, r, g, b, a, s_r, s_g, s_b, s_a, intensity, falloff, range;

		in1 >> x >> y >> z >> r >> g >> b >> a >> s_r >> s_g >> s_b >> s_a >> intensity >> falloff >> range;
		//std::cout << model << std::endl;

		vec3_t position = vec3_t{x, y, z};
		color_t color = color_t{r, g, b, a};
		color_t s_color = color_t{s_r, s_g, s_b, s_a};

		point_lights.push_back(point_light_t{ position, color, s_color, intensity, falloff, range });
	}

	std::getline(in, line);
	std::istringstream in2(line);

	float r, g, b, a, s_r, s_g, s_b, s_a, intensity;

	in2 >> r >> g >> b >> a >> s_r >> s_g >> s_b >> s_a >> intensity;

	this->ambient_light = ambient_light_t{ color_t{r, g, b, a}, color_t{s_r, s_g, s_b, s_a}, intensity };

	this->model_instance_count = static_cast<uint32_t>(this->h_model_instances.size());

	this->point_light_count = static_cast<uint32_t>(this->point_lights.size());

	std::getline(in, line);
	std::istringstream in3(line);
	size_t object_count;
	in3 >> object_count;
	std::cout << std::setw(10) << object_count << " objects detected!" << std::endl;
	bool has_player = false;
	this->objects = *new std::vector<Object>();
	for (size_t i = 0; i < object_count; i++) {
		std::getline(in, line);
		std::istringstream in_obj(line);
		uint32_t type = 0;
		float x2, y2, z2, x_d, y_d, z_d, scale;
		std::string visual_model, rigid_model;

		in_obj >> x2 >> y2 >> z2 >> x_d >> y_d >> z_d >> scale >> visual_model;

		ObjectType obj_type = ObjectType::AI;
		//std::cout << "Type " << type << std::endl;

		d_ModelInstance instance;
		uint32_t model_idx = 0, instance_idx = 0, hitbox_index = 0;

		this->objects.push_back(OBJECTS[Runtime::find_object(visual_model)]);
		this->objects.back().set_position(vec3_t{x2, y2, z2});
		this->objects.back().set_direction(vec3_t{x_d, y_d, z_d});

		this->add_model_instance(create_instance(this->objects.back().get_model_index(), this->objects.back().get_position(), this->objects.back().get_direction(), HOST_MODELS[this->objects.back().get_model_index()].get_triangle_count(), false, scale));
		h_model_instances[this->objects.back().get_hitbox_instance_index()].scale = scale;
		this->objects.back().set_instance_index(static_cast<uint32_t>(this->h_model_instances.size() - 1));
		this->objects.back().set_spawn_point(vec3_t{x2, y2, z2});

		if (this->objects.back().get_object_type() == ObjectType::Player) {
			this->objects.back().attach_camera(this->camera_ptr);
		}
		std::cout << "Object added: " << &this->objects.back() << " @ " << this->objects.back().get_model_index() << " index with model " << visual_model << " of type " << this->objects.back().get_object_type() << std::endl;
	}
}

Level::Level(std::string path, Camera* cam, sycl::queue* q) {
    this->queue = q;

    this->camera_ptr = cam;

    this->filepath = path;

    this->load_from(this->filepath);
}

void Level::upload_instances() {
    if (this->d_model_instances != nullptr) {
        sycl::free(this->d_model_instances, *this->queue);
    }

    this->d_model_instances = sycl::malloc_device<d_ModelInstance>(this->h_model_instances.size(), *this->queue);

    this->queue->memcpy(this->d_model_instances, this->h_model_instances.data(), sizeof(d_ModelInstance) * this->h_model_instances.size());
    this->queue->wait();

    this->model_instance_count = static_cast<unsigned int>(this->h_model_instances.size());
}

void Level::add_model_instance(d_ModelInstance instance) {
    this->h_model_instances.push_back(instance);
}