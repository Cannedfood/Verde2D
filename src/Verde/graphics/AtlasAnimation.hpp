#pragma once

#include "Texture.hpp"
#include "Graphics.hpp"

class AtlasAnimation : public Graphics, public Texture {
	unsigned mNumFrames;
	unsigned mTiles;
	float    mFrameTime;

public:
	AtlasAnimation();
	~AtlasAnimation() override;

	bool load(YAML::Node& n);

	void draw(const Object* onto) override;
	void write(YAML::Emitter& e) override;
};
