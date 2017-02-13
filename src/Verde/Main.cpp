#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <list>

#include <cstdio>
#include <cstdlib>

#include "Level.hpp"
#include "Chunk.hpp"
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

	std::unique_ptr<Editor> mEditor;

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

		mEditor.reset(new Editor);

		// Test objects
		mPlayer.mRelativeBounds = {
			glm::vec2 { -0.5f },
			glm::vec2 {  0.5f }
		};
		mPlayer.mPosition = { 1, 4 };
		mLevel.addObject(&mPlayer, Object::DYNAMIC);
		mLevel.alias(&mPlayer, "player");

		{
			std::unique_ptr<Object> ground(new Object);
			ground->mRelativeBounds = {
				glm::vec2 { -2, 0.f },
				glm::vec2 {  2, 0.5f }
			};

			auto* o = mLevel.addOwned(std::move(ground), Object::STATIC);
			mLevel.alias(o, "default_platform"); // For removal
			o->setGraphics("res/ground.yml");
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mHandles | OnKey(GLFW_KEY_TAB, [this] { if(mEditor) mEditor->bind(&mLevel); });
	}

	void Quit() {
		if(mEditor)
			mEditor.reset();

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

			if(mEditor)
				mEditor->update(dt);

			glLineWidth(1);
			mLevel.draw(CamBounds());

			if(mEditor)
				mEditor->draw();

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
