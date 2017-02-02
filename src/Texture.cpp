#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "3rd_party/stb_image.h"

#include <GL/gl.h>

Texture::Texture() :
	mHandle(0),
	mWorldSize(1),
	mDoesWrap(false)
{}

Texture::~Texture() {
	free();
}

bool Texture::load(const std::string& s) {
	int x, y, comp;
	unsigned char* data = stbi_load(s.c_str(), &x, &y, &comp, 0);
	if(!data) {
		printf("Failed loading '%s': %s\n", s.c_str(), stbi_failure_reason());
		return false;
	}

	GLenum fmt_internal;
	GLenum fmt_data;

	switch (comp) {
		case 3:
			fmt_internal = GL_RGB;
			fmt_data     = GL_RGB;
			break;
		case 4:
			fmt_internal = GL_RGBA;
			fmt_data     = GL_RGBA;
			break;
		default:
			stbi_image_free(data);
			printf("Failed loading '%s': Unsupported number of channels: %i\n", s.c_str(), comp);
			return false;
	}

	glGenTextures  (1, &mHandle);
	glBindTexture  (GL_TEXTURE_2D, mHandle);
	glTexImage2D   (GL_TEXTURE_2D, 0, fmt_internal, x, y, 0, fmt_data, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
	return true;
}

void Texture::free() {
	if(mHandle) {
		glDeleteTextures(1, &mHandle);
		mHandle = 0;
	}
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, mHandle);
}

#include <unordered_map>

std::unordered_map<std::string, std::shared_ptr<Texture>>* pTextureCache;

void Texture::Init() {
	pTextureCache = new std::unordered_map<std::string, std::shared_ptr<Texture>>;
}

void Texture::Quit() {
	delete pTextureCache;
}

std::shared_ptr<Texture> Texture::Load(const std::string &s) {
	if(pTextureCache) {
		auto& p = (*pTextureCache)[s];
		if(!p) {
			p = std::make_shared<Texture>();
			if(!p->load(s)) // Failed loading texture
				p.reset();
		}
		return p;
	}
	else {
		auto p = std::make_shared<Texture>();
		if(p->load(s))
			return p;
		else
			return nullptr;
	}
}
