#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <list>

#include <cstdio>
#include <cstdlib>

#include "Level.hpp"
#include "Object.hpp"
#include "graphics/Graphics.hpp"
#include "graphics/Camera.hpp"
#include "Settings.hpp"
#include "Event.hpp"

#include "audio/Audio.hpp"

#include "editor/Editor.hpp"

using namespace std;
using namespace std::chrono;
using namespace std::literals::chrono_literals;

class Game {
public:
	EvtHandles  mHandles;
	glm::vec2   mMouse;

	GLFWwindow* mWindow;

	Object      mPlayer;
	glm::vec2   mPlayerSpeed = { 6.f, 6.f };

	std::vector<std::unique_ptr<Object>> mObjects;

	Level       mLevel;

	Editor*     pEditor;

	struct {
		bool  active    = false;
		bool  snap      = true;
		float snap_dist = 2.f;
		Object* selected = nullptr;
	} mEditor;

	glm::vec2 MouseInLevel() {
		return ViewportToWorld(mMouse);
	}

	void AddGround(const Box& bounds) {
		mObjects.emplace_back(new Object);
		mObjects.back()->mRelativeBounds = bounds;
		mObjects.back()->mPosition = glm::vec2{0};
		mLevel.addObject(mObjects.back().get(), Object::STATIC);
		mObjects.back()->setGraphics(
			"res/rock.png"
		);
		mObjects.back()->mGraphics->setWrapping(glm::vec2{1.f});
		mEditor.selected = mObjects.back().get();
	}

	void Init() {
		glfwInit();

		int width = 800, height = 600;

		if(YAML::Node n = GetSettings()["graphics"]["resolution"]) {
			width = n[0].as<int>();
			height = n[1].as<int>();
		}
		else {
			n[0] = width;
			n[1] = height;
		}

		if(YAML::Node n = GetSettings()["graphics"]["samples"])
			glfwWindowHint(GLFW_SAMPLES, glm::max(1, n.as<int>()));
		else n = 1;

		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		mWindow = glfwCreateWindow(width, height, "Verde", NULL, NULL);
		if(!mWindow) {
			printf("Failed creating window\n");
			exit(-1);
		}
		glfwMakeContextCurrent(mWindow);
		
		if(YAML::Node n = GetSettings()["graphics"]["vsync"])
			glfwSwapInterval(n.as<bool>() ? 1 : 0);
		else {
			n = true;
			glfwSwapInterval(1);
		}

		internal::SetViewport(glm::vec2{width, height});

		internal::InitEvents(mWindow);
		internal::InitAudio();

		Graphics::InitCache();
		AudioData::InitCache();

		pEditor = new Editor;

		// Test objects
		mPlayer.mRelativeBounds = {
			glm::vec2 { -0.5f },
			glm::vec2 {  0.5f }
		};
		mPlayer.mPosition = { 1, 4 };
		mLevel.addObject(&mPlayer, Object::DYNAMIC);
		mLevel.alias(&mPlayer, "player");

		AddGround({
			{ 0,  0 },
			{ 10, 1 }
		});

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mHandles | OnKey(GLFW_KEY_TAB, [this] { if(pEditor) pEditor->bind(&mLevel); });
	}

	void Quit() {
		if(pEditor)
			delete pEditor;

		mHandles.clear();

		internal::FreeAudio();

		Graphics::FreeCache();
		AudioData::FreeCache();
		glfwDestroyWindow(mWindow);
		internal::FreeEvents();
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
				dt = (float)((now - last).count() * 0.000001);
				last = now;

				print_timer += dt;
				accum_time  += dt;
				frames++;
			}

			glfwPollEvents();
			// Camera
			internal::UpdateCam();
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(CamTransform());

			internal::UpdateAudio();

			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			if(glfwGetKey(mWindow, GLFW_KEY_D)) {
				mPlayer.mMotion.x = mPlayerSpeed.x;
				mPlayer.setGraphics("res/sprite-r.png");
			}
			else if(glfwGetKey(mWindow, GLFW_KEY_A)) {
				mPlayer.mMotion.x = -mPlayerSpeed.x;
				mPlayer.setGraphics("res/sprite-l.png");
			}
			else
				mPlayer.mMotion.x = 0;

			if(glfwGetKey(mWindow, GLFW_KEY_SPACE) || glfwGetKey(mWindow, GLFW_KEY_W)) {
				if (mPlayer.mMotion.y < 0) {
					bool on_ground = mLevel.hitTest({
						{ mPlayer.mBounds.min.x, mPlayer.mBounds.min.y - 0.1f },
						{ mPlayer.mBounds.max.x, mPlayer.mBounds.min.y - 0.01f }
					});

					if (on_ground) {
						mPlayer.mMotion.y = mPlayerSpeed.y;
					}
				}
				else
					mPlayer.mMotion.y += mPlayerSpeed.y * 1.2f * dt;
			}

			CamPosition(glm::mix(CamPosition(), mPlayer.mPosition, 0.01f));

			glLineWidth(4);
			mLevel.update(dt);

			glLineWidth(1);
			mLevel.draw(CamBounds());

			if(pEditor) {
				pEditor->draw();
			}

			/*
			if(mEditor.active) {
				glDisable(GL_TEXTURE_2D);

				if(mEditor.selected) {
					glLineWidth(5);
					Box& b = mEditor.selected->mBounds;
					glColor3f(1, 1, 0);
					glBegin(GL_LINE_LOOP);
						glVertex2fv(&b.min[0]);
						glVertex2f (b.max.x, b.min.y);
						glVertex2fv(&b.max[0]);
						glVertex2f (b.min.x, b.max.y);
					glEnd();
					glBegin(GL_POINTS);
						glColor3f(1, 0, 0);
						glVertex2fv(&mEditor.selected->mPosition[0]);
					glEnd();
				}

				glLineWidth(3);
				mLevel.debugDraw(CamBounds());
			}
			*/

			glfwSwapBuffers(mWindow);

			if(print_timer > 5) {
				print_timer = 0;
				float avg_dt = accum_time / frames;
				printf("dt (secs): %.2f (%.1fFPS)\n", avg_dt, 1 / avg_dt);
			}
		}

		Quit();
	}
} *game;

void clickCallback(GLFWwindow* win, int btn, int action, int mods) {
	if(action < 1) return;
	switch (btn) {
		case GLFW_MOUSE_BUTTON_RIGHT: {
			if(Object* o = game->mLevel.at(game->MouseInLevel(), Object::STATIC | Object::DYNAMIC | Object::PARTICLE))
				game->mEditor.selected = o;
		} break;
		case GLFW_MOUSE_BUTTON_MIDDLE: {
			glm::vec2 p = game->MouseInLevel();
			if(game->mEditor.snap) {
				glm::vec2 pp = glm::round(p * game->mEditor.snap_dist) / game->mEditor.snap_dist;
				glm::vec2 size = p - pp;
				size.x = size.x < 0 ? -1.f : 1.f;
				size.y = size.y < 0 ? -1.f : 1.f;

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

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(action > 0) {
		switch (key) {
			case GLFW_KEY_K:
				game->mEditor.snap = !game->mEditor.snap;
				break;
			case GLFW_KEY_S: {
				if(mods & GLFW_MOD_CONTROL) {
					// TODO: select file
					std::string save_path = "res/level.yml";

					YAML::Emitter savedata;

					savedata << YAML::BeginMap;

					savedata << YAML::Key << "player";
					game->mPlayer.write(savedata);

					savedata << YAML::Key << "objects";
					savedata << YAML::BeginSeq;
					for(size_t i = 0; i < game->mObjects.size(); i++) {
						savedata << YAML::Value;
						game->mObjects[i]->write(savedata);
					}
					savedata << YAML::EndSeq;

					savedata << YAML::EndMap;

					std::ofstream file(save_path);
					if(file)
						file << savedata.c_str();
					else
						printf("Failed writing save to %s: Failed opening file.\n", save_path.c_str());
				}
			} break;
			case GLFW_KEY_O: {
				auto begin = high_resolution_clock::now();

				std::ifstream file("res/level.yml", std::ios::binary);
				if(!file) break;

				YAML::Node data = YAML::Load(file);
				if(YAML::Node n = data["player"])
					game->mPlayer.read(n);

				if(YAML::Node objects = data["objects"]) {
					game->mObjects.clear();

					for(YAML::Node nn : objects) {
						game->mObjects.emplace_back(new Object);
						game->mObjects.back()->read(nn);
						game->mLevel.addObject(game->mObjects.back().get());
					}
				}

				auto end = high_resolution_clock::now();
				printf("Loading level took %fms\n", duration_cast<microseconds>(end - begin).count() * 0.001f);
			} break;
			case GLFW_KEY_UP:   if(game->mEditor.selected) game->mEditor.selected->mHeight += 0.1f; break;
			case GLFW_KEY_DOWN: if(game->mEditor.selected) game->mEditor.selected->mHeight -= 0.1f; break;
		}
	}
}

int main(int argc, char** argv) {
	const std::string settings_path = "res/settings.yml";

	{
		std::ifstream settings_file("res/settings.yml", std::ios::binary);
		if(settings_file)
			internal::Settings = YAML::Load(settings_file);
		else
			printf("No configuration file at %s\n", settings_path.c_str());
	}

	game = new Game();
	game->Run();
	delete game;

	{
		std::ofstream settings_file("res/settings.yml");
		if(settings_file)
			settings_file << internal::Settings;
		else
			printf("Failed writing settings to %s\n", settings_path.c_str());
	}
}
