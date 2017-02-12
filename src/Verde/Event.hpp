#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>

namespace internal {
	class HandleInterface {
		virtual ~HandleInterface() {}
	};

	void InitEvents(GLFWwindow* window);
	void FreeEvents();

	void SendKey   (int key, int action);
	void SendMouse (int key, int action);
	void SendCursor(float x, float y);
};

using handle = std::unique_ptr<internal::HandleInterface>;

using KeyFn   = bool(int action);
using ClickFn = bool(const glm::vec2& v, int action);
using DragFn  = bool(const glm::vec2& v, const glm::vec2& dif);
using MoveFn  = bool(const glm::vec2& v, const glm::vec2& dif);

handle HookKey   (int key, const std::function<KeyFn>& fn);
handle HookClick (int key, const std::function<ClickFn>& fn);
handle HookDrag  (int key, const std::function<DragFn>& fn);
handle HookMove  (const std::function<MoveFn>& fn);
