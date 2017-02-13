#pragma once

#include "../Event.hpp"

#include <chrono>

class Level;
class Chunk;
class Object;

class Editor {
	EvtHandles mHandles;
	Level*     mLevel    = nullptr;
	Chunk*     mChunk    = nullptr;
	Object*    mSelected = nullptr;

	bool  mSnap         = true;
	float mSnapDistance = .5f;
	bool  mDrawGrid     = false;

	std::chrono::high_resolution_clock::time_point last_error;
public:
	void bind(Level* level);
	void unbind();

	void chunkMode();
	void objectMode();

	void update(float dt);
	void draw();


	void eraseCurrent();

	void selectUnder(const glm::vec2& p);
	void middleClick();
	bool dragResize(const glm::vec2& at, const glm::vec2& dist);
	bool dragMove(const glm::vec2& at, const glm::vec2& dist);

	bool onDrop(const char* c);

	void load();
	void save();

	void editorError(const char* msg);
};
