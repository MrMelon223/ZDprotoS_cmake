#ifndef OBJECT_H
#define OBJECT_H

#include "Model.h"
#include "Camera.h"

enum ObjectType {
	AI,
	Physics,
	Player,
	Weapon,
	Static
};

class Object {
private:
	void load_from(std::string);
protected:
	std::string name;
	std::string filepath;

	vec3_t spawn_position;

	vec3_t position;
	vec3_t rotation;
	vec3_t direction;
	vec3_t velocity;
	float mass_kg;

	unsigned int model_index;
	unsigned int hitbox_model_index;

	bool is_visible;

	ObjectType object_type;

	Camera* player_camera;
public:
	Object(std::string);

	void set_visibility(bool b) { this->is_visible = b; }

	vec3_t get_position() { return this->position; }
	void set_position(vec3_t p) { this->position = p; }

	std::string get_name() { return this->name; }

	void set_spawn_point(vec3_t v) { this->spawn_position = v;}

	void set_direction(vec3_t v) { this->direction = v; }
	vec3_t get_direction() { return this->direction; }

	vec3_t get_rotation() { return this->rotation; }
	void set_rotation(vec3_t r) { this->rotation = r; } 

	unsigned int get_model_index() { return this->model_index; }
	unsigned int get_hitbox_instance_index() { return this->hitbox_model_index; }

	ObjectType get_object_type() { return this->object_type; }

	void attach_camera(Camera*);

	void set_instance_index(unsigned int idx) { this->model_index = idx; }

};

#endif
