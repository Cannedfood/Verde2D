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
Box Chunk::bounds(bool recursive) {
	Box b = Box::ExpandNull();
	
	for(auto& o : mObjects)
		b.expand(o->mBounds);

	if(recursive) {
		for(auto& c : mChunks)
			b.expand(c->bounds());
	}

	return b;
}

using namespace YAML;

void Chunk::load(YAML::Node n) {
	if(Node nn = n["file"]) {
		mFile = nn.as<std::string>();
		Node loaded = LoadFile(mFile);

		this->load(loaded);
	}
	else {
		if(Node nn = n["chunks"]) {
			mChunks.clear();
			for(Node data : nn) {
				auto* c = createChunk();
				c->load(data);
			}
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

	if(Node nn = n["offset"]) {
		offset({
			nn[0].as<float>(),
			nn[1].as<float>()
		});
	}
}
void Chunk::save(YAML::Emitter& e) {
	e << BeginMap;

	if(mOffset.x != 0 || mOffset.y != 0) {
		e << Key << "offset" << BeginSeq;
		e << mOffset.x << mOffset.y;
		e << EndSeq;
	}

	glm::vec2 old_offset = mOffset;
	offset(-mOffset);

	if(!mObjects.empty()) {
		e << Key << "objects" << BeginSeq;
		for(auto& a : mObjects)
			a->save(e);
		e << EndSeq;
		e << Key << "chunks" << BeginSeq;
		for(auto& a : mChunks)
			a->save(e);
		e << EndSeq;
	}

	offset(old_offset); // Reset offset

	e << EndMap;
}
