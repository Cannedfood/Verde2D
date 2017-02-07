#pragma once

#include "../Box.hpp"

#include <yaml-cpp/yaml.h>

class Object;

class Graphics {
private:
	std::string mPrefab;

public:
	virtual ~Graphics() {}

	virtual void draw(const Object* onto) = 0;
	virtual void write(YAML::Emitter& to) = 0;
	void writeOrPrefab(YAML::Emitter& to);


	virtual void setAnimationOffset(float sec) {}
	virtual bool setWrapping(const glm::vec2& v) { return false; }


	static std::shared_ptr<Graphics> Load(const std::string& s);
	static std::shared_ptr<Graphics> Load(YAML::Node n);

	static void InitCache();
	static void FreeCache();
	static void CleanCache();
};
