#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <list>
#include <thread>
#include <chrono>

#include "Level.hpp"
#include "Object.hpp"
#include "Texture.hpp"

using namespace std;
using namespace std::chrono;
using namespace std::literals::chrono_literals;

void viewportChangedCallback(GLFWwindow* win, int w, int h);
void cursorCallback(GLFWwindow* win, double x, double y);
void clickCallback(GLFWwindow* win, int btn, int action, int mods);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

class Game {
public:
	glm::vec2   mMouse;

	GLFWwindow* mWindow;

	unsigned    mWidth, mHeight;
	float       mAspect;

	glm::vec2   mCamPosition;
	float       mCamZoom = 0.1f;

	Object mPlayer;
	glm::vec2 mPlayerSpeed = { 6.f, 6.f };

	std::vector<std::unique_ptr<Object>> mGround;

	Level       mLevel;

	struct {
		bool  active    = false;
		bool  snap      = true;
		float snap_dist = 2.f;
		Object* selected = nullptr;
	} mEditor;

	glm::vec2 MouseInLevel() {
		float yscale = 2 * mCamZoom;
		float xscale = yscale * mAspect;

		return {
			mCamPosition.x + mMouse.x / xscale,
			mCamPosition.y + mMouse.y / yscale
		};
	}

	Object* Active() {
		if(mEditor.selected)
			return mEditor.selected;
		else if(mGround.empty())
			return nullptr;
		else
			return mEditor.selected = mGround.back().get();
	}

	void AddGround(const Box& bounds) {
		mGround.emplace_back(new Object);
		mGround.back()->mRelativeBounds = bounds;
		mGround.back()->mPosition = glm::vec2{0};
		mLevel.addObject(mGround.back().get(), Object::STATIC);
		mGround.back()->setTexture(
			"res/rock.png"
		);
		mGround.back()->mTexture->setWrapping(glm::vec2{1.f});
		mEditor.selected = mGround.back().get();
	}

	void Init() {
		glfwInit();
		glfwWindowHint(GLFW_SAMPLES, 16);
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		mWindow = glfwCreateWindow(800, 600, "Verde", NULL, NULL);
		if(!mWindow) {
			printf("Failed creating window\n");
			exit(-1);
		}
		glfwMakeContextCurrent(mWindow);
		glfwSetWindowSizeCallback(mWindow, viewportChangedCallback);
		glfwSetCursorPosCallback(mWindow, cursorCallback);
		glfwSetMouseButtonCallback(mWindow, clickCallback);
		glfwSetKeyCallback(mWindow, keyCallback);
		glfwSwapInterval(1);

		viewportChangedCallback(mWindow, 800, 600);

		Texture::Init();


		// Test objects
		mPlayer.mRelativeBounds = {
			glm::vec2 { -0.5f },
			glm::vec2 {  0.5f }
		};
		mPlayer.mPosition = { 1, 4 };
		mLevel.addObject(&mPlayer, Object::DYNAMIC);

		mPlayer.setTexture("/home/benno/Downloads/danks.jpg");

		AddGround({
			{ 0,  0 },
			{ 10, 1 }
		});
	}

	void Quit() {
		Texture::Quit();
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}

	void Run() {
		Init();

		microseconds last = duration_cast<microseconds>(steady_clock::now().time_since_epoch());
		size_t frames = 0;
		float  accum_time = 0;
		float print_timer = 0;

		while(!glfwWindowShouldClose(mWindow)) {
			float dt;
			{
				microseconds now = duration_cast<microseconds>(steady_clock::now().time_since_epoch());
				dt = (now - last).count() * 0.000001;
				last = now;

				print_timer += dt;
				accum_time  += dt;
				frames++;
			}

			glfwPollEvents();

			{
				glMatrixMode(GL_PROJECTION);

				float xscale = 2 * mCamZoom * mAspect;
				float yscale = 2 * mCamZoom;

				float projection_mat[] = {
					xscale,                        0, 0, 0,
					0,                   yscale, 0, 0,
					0,                        0, 1, 0,
					-mCamPosition.x * xscale, -mCamPosition.y * yscale, 0, 1
				};

				glLoadMatrixf(projection_mat);
			}

			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			if(glfwGetKey(mWindow, GLFW_KEY_D)) {
				mPlayer.mMotion.x = mPlayerSpeed.x;
				mPlayer.setTexture("res/sprite-r.png");
			}
			else if(glfwGetKey(mWindow, GLFW_KEY_A)) {
				mPlayer.mMotion.x = -mPlayerSpeed.x;
				mPlayer.setTexture("res/sprite-l.png");
			}
			else
				mPlayer.mMotion.x = 0;

			if(glfwGetKey(mWindow, GLFW_KEY_SPACE) || glfwGetKey(mWindow, GLFW_KEY_W)) {
				if(mPlayer.mMotion.y < 0)
					mPlayer.mMotion.y = mPlayerSpeed.y;
				else
					mPlayer.mMotion.y += mPlayerSpeed.y * 1.2f * dt;
			}

			mCamPosition = glm::mix(mCamPosition, mPlayer.mPosition, 0.01f);

			glLineWidth(4);
			mLevel.update(dt);

			glLineWidth(1);
			mLevel.draw();

			if(mEditor.active) {
				glLineWidth(3);
				glDisable(GL_TEXTURE_2D);
				mLevel.debugDraw();

				if(mEditor.selected) {
					glColor3f(1, 1, 0);
					Box& b = mEditor.selected->mBounds;
					glBegin(GL_LINE_LOOP);
						glVertex2fv(&b.min[0]);
						glVertex2f (b.max.x, b.min.y);
						glVertex2fv(&b.max[0]);
						glVertex2f (b.min.x, b.max.y);
					glEnd();
				}
			}

			glfwSwapBuffers(mWindow);

			if(print_timer > 0.5f) {
				print_timer = 0;
				//printf("dt (secs): %f\n", accum_time / frames);
			}
		}

		Quit();
	}
} *game;

void viewportChangedCallback(GLFWwindow* win, int w, int h) {
	glViewport(0, 0, w, h);
	game->mWidth  = w;
	game->mHeight = h;
	game->mAspect = h / (float)w;
}

void onCursorUpdate() {
	if(glfwGetMouseButton(game->mWindow, GLFW_MOUSE_BUTTON_LEFT)) {
		if(!game->mGround.empty()) {
			glm::vec2 mouse = game->MouseInLevel();
			Box&      b     = game->Active()->mRelativeBounds;

			glm::vec2 mdist = mouse - b.middle();

			if(game->mEditor.snap) mouse = glm::round(mouse * game->mEditor.snap_dist) / game->mEditor.snap_dist;

			if(mdist.x < 0) b.min.x = mouse.x;
			else            b.max.x = mouse.x;

			if(mdist.y < 0) b.min.y = mouse.y;
			else            b.max.y = mouse.y;

			printf("Set %u %s.x = %.2f %s.y = %.2f\n",
				(unsigned) game->mGround.size(),
				mdist.x < 0 ? "min" : "max",
				mouse.x,
				mdist.y < 0 ? "min" : "max",
				mouse.y
			);
		}
	}
}

void cursorCallback(GLFWwindow* win, double x, double y) {
	game->mMouse = glm::vec2(((float)x / game->mWidth) * 2 - 1, - (((float)y / game->mHeight) * 2 - 1)) ;
	onCursorUpdate();
}

void clickCallback(GLFWwindow* win, int btn, int action, int mods) {
	if(action < 1) return;
	switch (btn) {
		case GLFW_MOUSE_BUTTON_RIGHT: {
			game->mEditor.selected = nullptr;
			if(!game->mGround.empty()) {
				glm::vec2 mouse = game->MouseInLevel();
				for (int i = game->mGround.size() - 1; i >= 0; i--) {
					if(game->mGround[i]->mBounds.contains(mouse)) {
						game->mEditor.selected = game->mGround[i].get();
						break;
					}
				}
			}
		} break;
		case GLFW_MOUSE_BUTTON_MIDDLE: {
			glm::vec2 p = game->MouseInLevel();
			if(game->mEditor.snap) {
				glm::vec2 pp = glm::round(p * game->mEditor.snap_dist) / game->mEditor.snap_dist;
				glm::vec2 size = p - pp;
				size.x = size.x < 0 ? -1 : 1;
				size.y = size.y < 0 ? -1 : 1;

				game->AddGround({
					pp,
					pp + size / game->mEditor.snap_dist
				});
			}
			else {
				glm::vec2 size { 0.05f };
				game->AddGround({p - size, p + size});
			}
		} break;
		default: break;
	}
}

void setTexture(int i) {
	if(!game->mGround.empty()) {
		i -= 1;

		struct TexInfo {
			std::string path;
			bool wraps;
			glm::vec2 wrapping;
		};

		TexInfo textures[] = {
			{ "res/rock.png", true, { 1, 1 } },
			{ "res/darkrock.png", true, { 1, 1 } },
			{ "res/ground.png", true, { 1, 1 } }
		};

		game->Active()->setTexture(textures[i].path);
		if(textures[i].wraps && game->Active()->mTexture)
			game->Active()->mTexture->setWrapping(textures[i].wrapping);
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(action > 0) {
		switch (key) {
			case GLFW_KEY_X:
				if(game->mEditor.selected) {
					for (size_t i = 0; i < game->mGround.size(); i++) {
						if(game->mGround[i].get() == game->mEditor.selected) {
							game->mGround.erase(game->mGround.begin() + i);
							break;
						}
					}
					game->mEditor.selected = nullptr;
				}
				break;
			case GLFW_KEY_TAB:
					game->mEditor.active = !game->mEditor.active;
				break;
			case GLFW_KEY_1: setTexture(1); break;
			case GLFW_KEY_2: setTexture(2); break;
			case GLFW_KEY_3: setTexture(3); break;
			// case GLFW_KEY_4: setTexture(4); break;
			case GLFW_KEY_K:
				game->mEditor.snap = !game->mEditor.snap;
				break;
		}

	}
}

int main(int argc, char** argv) {
	game = new Game();
	game->Run();
	delete game;
}
