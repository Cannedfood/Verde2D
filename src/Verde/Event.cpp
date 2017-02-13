#include "Event.hpp"

#include <list>
#include <map>

#include "graphics/Camera.hpp"

namespace {

	template<typename Hfn>
	struct HandlerList {
		struct _Handle : public internal::HandleInterface {
			std::list<_Handle>*          mContainer;
			typename
			std::list<_Handle>::iterator mLocation;
			std::function<Hfn>           mCallback;

			void destroy() override {
				if(mContainer) {
					mCallback = nullptr;
					auto* c = mContainer;
					mContainer = nullptr;
					c->erase(mLocation);
				}
			}
		};

		std::list<_Handle> mContainer;

	public:
		~HandlerList() {
			for(auto& a : mContainer)
				a.mContainer = nullptr;
			mContainer.clear();
		}

		EvtHandle add(std::function<Hfn>&& fn) {
			mContainer.emplace_front(_Handle());
			auto iter = mContainer.begin();
			iter->mContainer = &mContainer;
			iter->mLocation = iter;
			iter->mCallback = std::move(fn);
			return EvtHandle(&mContainer.front());
		}

		using iterator = typename std::list<_Handle>::iterator;

		iterator begin() {
			return mContainer.begin();
		}

		iterator end() {
			return mContainer.end();
		}
	};

	struct EventManager {
		std::map<int, HandlerList<KeyFn>>   mKey;
		std::map<int, HandlerList<ClickFn>> mClick;
		std::map<int, HandlerList<MoveFn>>  mDrag;

		HandlerList<DropFn> mDrop;

		glm::vec2 mMouseLast;

		GLFWwindow* mWindow;
	};

	EventManager* pEvents;

	void viewportChangedCallback(GLFWwindow* win, int w, int h) {
		internal::SetViewport(glm::vec2{w, h});
	}

	void cursorCallback(GLFWwindow* win, double x, double y) {
		const glm::vec2& v =
			internal::GetViewport();

		glm::vec2 p;
		p.x = ((float)x / v.x) * 2 - 1;
		p.y = -(((float)y / v.y) * 2 - 1);

		glm::vec2 dif = p - pEvents->mMouseLast;

		pEvents->mMouseLast = p;

		for(int i = GLFW_MOUSE_BUTTON_1; i < GLFW_MOUSE_BUTTON_4; i++) {
			if(glfwGetMouseButton(win, i)) {
				for(auto& fn : pEvents->mDrag[i]) {
					if(fn.mCallback(p, dif)) return;
				}
			}
		}

		for(auto& fn : pEvents->mDrag[-1])
			if(fn.mCallback(p, dif)) return;
	}

	void clickCallback(GLFWwindow* win, int btn, int action, int mods) {
		for(auto& fn : pEvents->mClick[btn]) {
			if(fn.mCallback(pEvents->mMouseLast, action, mods))
				return;
		}
	}

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		for(auto& fn : pEvents->mKey[key]) {
			if(fn.mCallback(action, mods)) return;
		}
	}

	void dropCallback(GLFWwindow* w, int count, const char** data) {
		for(int i = 0; i < count; i++) {
			for(auto& f : pEvents->mDrop) {
				if(f.mCallback(data[i])) return;
			}
		}
	}
}

namespace internal {
	void InitEvents(GLFWwindow* window) {
		pEvents = new EventManager;

		pEvents->mWindow = window;

		glfwSetWindowSizeCallback(window, viewportChangedCallback);
		glfwSetKeyCallback(window, keyCallback);
		glfwSetMouseButtonCallback(window, clickCallback);
		glfwSetCursorPosCallback(window, cursorCallback);
		glfwSetDropCallback(window, dropCallback);
	}

	void FreeEvents() {
		delete pEvents;
	}
};

EvtHandle HookKey  (int key, const std::function<KeyFn>& fn) {
	return pEvents->mKey[key].add(std::function<KeyFn>(fn));
}

EvtHandle HookClick(int key, const std::function<ClickFn>& fn) {
	return pEvents->mClick[key].add(std::function<ClickFn>(fn));
}

EvtHandle HookDrag (int key, const std::function<MoveFn>& fn) {
	return pEvents->mDrag[key].add(std::function<MoveFn>(fn));
}

EvtHandle HookMove (const std::function<MoveFn>& fn) {
	return pEvents->mDrag[-1].add(std::function<MoveFn>(fn));
}

EvtHandle OnDrop(const std::function<DropFn>& fn) {
	return pEvents->mDrop.add(std::function<DropFn>(fn));
}

glm::vec2 CursorWorld() {
	return ViewportToWorld(pEvents->mMouseLast);
}

glm::vec2 CursorViewport() {
	return pEvents->mMouseLast;
}
