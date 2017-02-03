#include "Chunk.hpp"

#include "Texture.hpp"
#include "Level.hpp"

bool Chunk::remove(Object* o) {
	for(size_t i = 0; i < mObjects.size(); i++) {
		if(mObjects[i].get() == o) {
			mObjects.erase(mObjects.begin() + i);
			return true;
		}
	}

	return false;
}

void Chunk::add(std::unique_ptr<Object>&& o) {
	mObjects.emplace_back(std::move(o));
}



using namespace YAML;

void Chunk::read(YAML::Node n) {
	if(Node nn = n["offset"]) {
		mOffset.x = nn[0].as<float>();
		mOffset.y = nn[1].as<float>();
	}

	if(Node nn = n["objects"]) {
		mObjects.clear();
		for(Node o : nn) {
			mObjects.emplace_back(new Object);
			mObjects.back()->read(o);
			mObjects.back()->mPosition += mOffset;
			mLevel->addObject(mObjects.back().get());
		}
	}

	mBackground = Texture::Load(n["texture"]);
}

void Chunk::write(YAML::Emitter& e) {
	e << BeginMap;

	if(mOffset.x != 0 || mOffset.y != 0) {
		e << Key << "offset" << BeginSeq;
		e << mOffset.x << mOffset.y;
		e << EndSeq;
	}

	if(!mObjects.empty()) {
		e << Key << "objects" << BeginSeq;
		for(auto& a : mObjects)
			a->write(e);
		e << EndSeq;
	}

	if(mBackground) {
		e << Key << "background";
		mBackground->write(e);
	}

	e << EndMap;
}
