#pragma once

#include "Texture.hpp"
#include "Graphics.hpp"

class AtlasAnimation : public Graphics, public Texture {
	unsigned    mNumFrames;
	unsigned    mTiles;
	float       mFrameTime;
	std::string mFile;

public:
	AtlasAnimation();
	~AtlasAnimation() override;

	bool load(YAML::Node& n);

	void draw(const Object* onto) override;
	void writeImpl(YAML::Emitter& e, int flags) override;
};
