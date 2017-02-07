#include "AtlasAnimation.hpp"

#include "../Object.hpp"

#ifdef _WIN32
#	include <Windows.h> // Dependency of include <GL/gl.h>
#endif

#include <GL/gl.h>
#include <chrono>

AtlasAnimation::AtlasAnimation() {}
AtlasAnimation::~AtlasAnimation() {}

void AtlasAnimation::draw(const Object* o) {
	using namespace std::chrono;

	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	bind();

	// TODO: create global time variable instead of getting it here via chrono
	double   animation_time = o->mAnimationTime - duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count() * 0.001;
	unsigned frame = ((unsigned)std::floor(animation_time / mFrameTime)) % mNumFrames;
	unsigned t = frame % mTiles; // OPTIMIZE: use the division we do anyways for manually calculating modulo
	unsigned s = frame / mTiles;

	float tile_size = 1.f / mTiles;
	glm::vec2 p = glm::vec2(s, t) / (float) mTiles;

	glm::vec2 p0 = glm::vec2(p.x, p.y + tile_size);
	glm::vec2 p1 = glm::vec2(p.x + tile_size, p.y);

	if(o->mFlipOrientation)
		std::swap(p0.x, p1.x);

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(p0.x, p0.y); glVertex2fv(&o->mBounds.min[0]);
		glTexCoord2f(p1.x, p0.y); glVertex2f (o->mBounds.max.x, o->mBounds.min.y);
		glTexCoord2f(p0.x, p1.y); glVertex2f (o->mBounds.min.x, o->mBounds.max.y);
		glTexCoord2f(p1.x, p1.y); glVertex2fv(&o->mBounds.max[0]);
	glEnd();
}

bool AtlasAnimation::load(YAML::Node& n) {
	if(YAML::Node nn = n["frames"]) {
		mNumFrames = nn.as<unsigned>();
		if(mNumFrames < 2)
			puts("An animation needs at least 2 frames!");
	}
	else
		mNumFrames = 4;

	{
		float tiles = std::ceil(std::sqrt((float) mNumFrames));
		mTiles = (unsigned) tiles;
	}

	if(YAML::Node nn = n["fps"])
		mFrameTime = 1 / nn.as<float>();
	else
		mFrameTime = 1 / 15.f;

	if(YAML::Node nn = n["texture"])
		return Texture::load(mFile = nn.as<std::string>());
	else {
		puts("atlas-animation needs a texture file to be declared.");
		return false;
	}

	if(YAML::Node nn = n["loaded-from"]) {
		mFile = nn.as<std::string>();
	}
}

void AtlasAnimation::write(YAML::Emitter& e) {
	using namespace YAML;
	e << BeginMap;
	e << Key << "type"    << "atlas-animation";
	e << Key << "frames"  << mNumFrames;
	e << Key << "fps"     << 1.f / mFrameTime;
	e << Key << "file"    << mFile;
	e  << EndMap;
}
