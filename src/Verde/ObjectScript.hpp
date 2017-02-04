#pragma once

#include <yaml-cpp/yaml.h>

class Object;

class ObjectScript {
public:
	virtual ~ObjectScript() {}
	virtual void update(Object* o) = 0;
	virtual void write(YAML::Emitter& e) = 0;
};
