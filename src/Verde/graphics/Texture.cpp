#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "3rd_party/stb_image.h"

#ifdef _WIN32
#	include <Windows.h> // Dependency of include <GL/gl.h>
#endif

#include <GL/gl.h>

#include <chrono>

Texture::Texture() :
	mHandle(0)
{}

Texture::~Texture() {
	free();
}

using namespace std::chrono;
using namespace std::literals::chrono_literals;

bool Texture::load(const std::string& s) {
	this->free();

	auto beg = duration_cast<microseconds>(steady_clock::now().time_since_epoch());

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

	mFile = s;
	printf("Loaded %s (%.2fms)\n", s.c_str(), (duration_cast<microseconds>(steady_clock::now().time_since_epoch()) - beg).count() * 0.001f);
	return true;
}

void Texture::free() {
	if(mHandle) {
		glDeleteTextures(1, &mHandle);
		mHandle = 0;
		mFile.clear();
	}
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, mHandle);
}
