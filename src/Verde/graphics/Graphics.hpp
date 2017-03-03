#pragma once

#include "../Box.hpp"

#include <yaml-cpp/yaml.h>

class Object;

class Graphics {
private:
	std::string mPrefab;

public:
	virtual ~Graphics() {}

	enum WriteFlags {
		WRITE_NO_PREFAB = 1
	};

	virtual void draw(const Object* onto) = 0;
	virtual void writeImpl(YAML::Emitter& to, int flags = 0) = 0;
	void         write(YAML::Emitter& to, int flags = 0);


	virtual void setAnimationOffset(float sec) {}
	virtual bool setWrapping(const glm::vec2& v) { return false; }


	static std::shared_ptr<Graphics> Load(const std::string& s);
	static std::shared_ptr<Graphics> Load(YAML::Node n);

	static void InitCache();
	static void FreeCache();
	static void CleanCache();
};
