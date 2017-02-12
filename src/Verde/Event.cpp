#include "Event.hpp"

#include <list>
#include <map>

namespace {

	template<typename T>
	class HandlerContainer {
		T mContainer;

		struct HandlerHandle : public internal::HandleInterface {
			T*                   mContainer;
			typename T::iterator mLocation;

			~HandlerHandle() {
				if(mContainer)
					mContainer->erase(mLocation);
			}
		};
	};

	class EventManager {
		HandlerContainer<std::multimap<int, std::function<KeyFn>>> mKey;
		HandlerContainer<std::multimap<int, std::function<KeyFn>>> mClick;
		HandlerContainer<std::multimap<int, std::function<KeyFn>>> mDrag;
		HandlerContainer<std::list<std::function<KeyFn>>>          mKeys;
	};

	EventManager* pEvents;
}

namespace internal {
	void InitEvents(GLFWwindow* window) {
		pEvents = new EventManager;
	}

	void FreeEvents() {
		delete pEvents;
	}

	void SendKey   (int key, int action);
	void SendMouse (int key, int action);
	void SendCursor(float x, float y);
};
