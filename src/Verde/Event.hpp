#pragma once

#include <memory>
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

using handle = std::unique_ptr<internal::HandleInterface, internal::HandleInterface::FakeDeleter>;

using KeyFn   = bool(int action);
using ClickFn = bool(const glm::vec2& v, int action);
using DragFn  = bool(const glm::vec2& v, const glm::vec2& dif);
using MoveFn  = bool(const glm::vec2& v, const glm::vec2& dif);
using DropFn  = bool(const char* c);

handle HookKey   (int key, const std::function<KeyFn>& fn);
handle HookClick (int key, const std::function<ClickFn>& fn);
handle HookDrag  (int key, const std::function<DragFn>& fn);
handle HookMove  (const std::function<MoveFn>& fn);
