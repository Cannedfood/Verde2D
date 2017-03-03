#pragma once

#include "Texture.hpp"
#include "Graphics.hpp"

class StaticGraphics : public Graphics, public Texture {
private:
	bool      mDoesWrap = false;
	glm::vec2 mWrapping;

public:
	StaticGraphics();
	~StaticGraphics();

	bool load(YAML::Node n);

	void draw(const Object* o) override;
	void writeImpl(YAML::Emitter& e, int flags) override;
	bool setWrapping(const glm::vec2& v) override;
};
