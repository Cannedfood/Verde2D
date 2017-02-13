#include "Event.hpp"

#include <list>
#include <map>

#include "graphics/Camera.hpp"

namespace {

	template<typename K, typename Hfn>
	class HandlerMap {
		struct _Handle : public internal::HandleInterface {
			std::multimap<K, _Handle>*          mContainer;
			typename
			std::multimap<K, _Handle>::iterator mLocation;
			std::function<Hfn>                  mCallback;

			void destroy() override {
				if(mContainer) {
					mContainer->erase(mLocation);
					mContainer = nullptr;
					mCallback = nullptr;
				}
			}
		};

		std::multimap<K, _Handle> mContainer;

		handle add(const K& k, std::function<Hfn>&& fn) {
			auto iter = mContainer.emplace(k, _Handle());
			iter->second.mContainer = mContainer;
			iter->second.mLocation = iter;
			iter->second.mCallback = std::move(fn);
			return handle(&iter->second);
		}
	};

	template<typename Hfn>
	class HandlerList {
		struct _Handle : public internal::HandleInterface {
			std::list<_Handle>*          mContainer;
			typename
			std::list<_Handle>::iterator mLocation;
			std::function<Hfn>           mCallback;

			void destroy() override {
				if(mContainer) {
					mContainer->erase(mLocation);
					mContainer = nullptr;
					mCallback = nullptr;
				}
			}
		};

		std::list<_Handle> mContainer;

		handle add(std::function<Hfn>&& fn) {
			mContainer.emplace_back(_Handle());
			auto iter = --mContainer.end();
			iter->mContainer = mContainer;
			iter->mLocation = iter;
			iter->mCallback = std::move(fn);
			return handle(&iter->second);
		}
	};

	struct EventManager {
		HandlerMap<int, KeyFn>   mKey;
		HandlerMap<int, ClickFn> mClick;
		HandlerMap<int, DragFn>  mDrag;

		HandlerList<KeyFn>  mKeys;
		HandlerList<DropFn> mDrop;
	};

	EventManager* pEvents;

	void viewportChangedCallback(GLFWwindow* win, int w, int h) {
		internal::SetViewport(glm::vec2{w, h});
	}

	void cursorCallback(GLFWwindow* win, double x, double y);
	void clickCallback(GLFWwindow* win, int btn, int action, int mods);
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void dropCallback(GLFWwindow* w, int count, const char** data);
}

namespace internal {
	void InitEvents(GLFWwindow* window) {
		pEvents = new EventManager;
		
		glfwSetWindowSizeCallback(window, viewportChangedCallback);
	}

	void FreeEvents() {
		delete pEvents;
	}
};
