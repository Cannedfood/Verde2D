#pragma once

#include "Texture.hpp"
#include "Graphics.hpp"

class AtlasAnimation : public Graphics, public Texture {
	unsigned mTilesS, mTilesT;

	AtlasAnimation();
	~AtlasAnimation();

	bool load(YAML::Node& n);

	void draw(const Object* onto) override;
	void write(YAML::Emitter& e) override;
};
