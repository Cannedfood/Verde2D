#pragma once

#include <yaml-cpp/yaml.h>

class Object;

class Behavior {
public:
	virtual void load  (Object* o, YAML::Node n) = 0;
	virtual void write (Object* o, YAML::Emitter& e) = 0;
	virtual void start (Object* o) = 0;
	virtual void update(Object* o, float dt) = 0;
	virtual ~Behavior() {}

	static bool LoadBehavior(Object* o, YAML::Node data);
};
