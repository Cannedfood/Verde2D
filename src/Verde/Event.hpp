#pragma once

#include <memory>
#include <deque>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>

namespace internal {
	struct HandleInterface {
		virtual void destroy() = 0;

		struct FakeDeleter {
			inline
			void operator()(HandleInterface* i) const { i->destroy(); }
		};
	};

	void InitEvents(GLFWwindow* window);
	void FreeEvents();
};

using EvtHandle = std::unique_ptr<internal::HandleInterface, internal::HandleInterface::FakeDeleter>;

class EvtHandles {
	std::deque<EvtHandle> mHandles;

public:
	EvtHandles& operator|(EvtHandle&& h) {
		mHandles.emplace_back(std::move(h));
		return *this;
	}

	void clear() {
		mHandles.clear();
	}
};

using KeyFn   = bool(int action, int mods);
using ClickFn = bool(const glm::vec2& pos, int action, int mods);
using MoveFn  = bool(const glm::vec2& new_pos, const glm::vec2& dif_to_last);
using DropFn  = bool(const char* c);

EvtHandle HookKey  (int key, const std::function<KeyFn>& fn);
EvtHandle HookClick(int key, const std::function<ClickFn>& fn);
EvtHandle HookMove (const std::function<MoveFn>& fn);
EvtHandle HookDrag (int key, const std::function<MoveFn>& fn);

EvtHandle OnDrop(const std::function<DropFn>&);

// Simplifications

template<typename C>
EvtHandle OnKey(int key, const C& c, int req_mods = 0) {
	return HookKey(key, std::function<KeyFn>([c, req_mods](int action, int mods) {
		if(action > 0 && (mods == 0 || (req_mods & mods) == req_mods)) {
			c();
			return true;
		}
		else
			return false;
	}));
}

template<typename C>
EvtHandle OnClick(int key, const C& c, int req_mods = 0) {
	return HookClick(key, [c, req_mods] (auto& pos, int action, int mods) {
		if(action > 0 && (mods == 0 || req_mods == mods)) {
			c(pos);
			return true;
		}
		else
			return false;
	});
}


glm::vec2 CursorWorld();
glm::vec2 CursorViewport();
