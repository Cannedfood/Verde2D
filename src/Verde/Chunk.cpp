#include "Chunk.hpp"

#include "graphics/Texture.hpp"
#include "Level.hpp"

#include <algorithm>

void Chunk::init(Level* l) {
	mLevel = l;
}

std::unique_ptr<Object> Chunk::remove(Object* o) {
	std::unique_ptr<Object> result;

	for(size_t i = 0; i < mObjects.size(); i++) {
		if(mObjects[i].get() == o) {
			result = std::move(mObjects[i]);
			mObjects.erase(mObjects.begin() + i);
			break;
		}
	}

	return result;
}
Object* Chunk::add(std::unique_ptr<Object>&& o, int type) {
	mLevel->addObject(o.get(), type);

	mObjects.emplace_back(std::move(o));
	return mObjects.back().get();
}

Chunk* Chunk::createChunk() {
	mChunks.emplace_back(new Chunk);
	mChunks.back()->mLevel       = mLevel;
	mChunks.back()->mParentChunk = this;
	return mChunks.back().get();
}
void   Chunk::_removeFromParent() {
	if(mParentChunk) {
		mParentChunk->mChunks.erase(
			std::find_if(
				mParentChunk->mChunks.begin(),
				mParentChunk->mChunks.end(),
				[this](auto& a) { return a.get() == this; }
			)
		);
	}
}
void   Chunk::removeChunk(Chunk* c) {
	assert(c);
	assert(c->mParentChunk == this);
	c->_removeFromParent();
}

void Chunk::offset(const glm::vec2& p) {
	for(auto& c : mChunks)
		c->offset(p);
	mOffset += p;
}

void Chunk::clear() {
	mChunks.clear();
	mObjects.clear();
}
void Chunk::clean() {
	for(auto& a : mChunks) {
		a->clean();
	}

	for (size_t i = 0; i < mChunks.size(); i++) {
		if(mChunks[i]->empty()) {
			mChunks.erase(mChunks.begin() + i);
			i--;
		}
	}
}
bool Chunk::empty() {
	if(!mObjects.empty()) return false;

	for(auto& a : mChunks) {
		if(!a->empty())
			return false;
	}

	return true;
}


using namespace YAML;

void Chunk::load(YAML::Node n) {
	// TODO: fix loading
	glm::vec2 offset_total;

	if(Node nn = n["offset"]) {
		offset_total.x = nn[0].as<float>();
		offset_total.y = nn[1].as<float>();
	}

	if(Node nn = n["file"]) {
		Node loaded = LoadFile(nn.as<std::string>());

	}

	if(Node nn = n["objects"]) {
		mObjects.clear();
		for(Node o : nn) {
			mObjects.emplace_back(new Object);
			mObjects.back()->load(o);
			mObjects.back()->mPosition += mOffset;
			mLevel->addObject(mObjects.back().get());
		}
	}
}
void Chunk::save(YAML::Emitter& e) {
	glm::vec2 old_offset = mOffset;

	offset(-mOffset);

	e << BeginMap;

	if(mOffset.x != 0 || mOffset.y != 0) {
		e << Key << "offset" << BeginSeq;
		e << mOffset.x << mOffset.y;
		e << EndSeq;
	}

	if(!mObjects.empty()) {
		e << Key << "objects" << BeginSeq;
		for(auto& a : mObjects)
			a->save(e);
		e << EndSeq;
	}

	e << EndMap;

	offset(old_offset);
}
