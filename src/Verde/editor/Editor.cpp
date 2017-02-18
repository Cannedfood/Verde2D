#include "Editor.hpp"

#include "../Level.hpp"
#include "../Object.hpp"
#include "../graphics/Camera.hpp"
#include "../audio/Audio.hpp"
#include "../Settings.hpp"

#include <GL/gl.h>

/* Insert code
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
*/

/* Save code
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
*/

/* Load code
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
*/

void Editor::bind(Level* level) {
	if(mLevel) unbind();

	mLevel = level;

	objectMode(); // Setup key bindings for object mode

	last_error = std::chrono::high_resolution_clock::now();
}

void Editor::unbind() {
	mLevel    = nullptr;
	mSelected = nullptr;
	mHandles.clear();
}

void Editor::chunkMode() {

}

void Editor::objectMode() {
	mHandles.clear();
	mHandles
	// Basic controls
	| OnKey(GLFW_KEY_TAB, [this]() { unbind(); })

	| OnKey(GLFW_KEY_X, [this]() { eraseCurrent(); })
	| OnKey(GLFW_KEY_K, [this]() { mSnap = !mSnap; })

	| OnKey(GLFW_KEY_L, [this]() { load(); }, GLFW_MOD_CONTROL)
	| OnKey(GLFW_KEY_S, [this]() { save(); }, GLFW_MOD_CONTROL)

	| OnKey(GLFW_KEY_T, [this]() { selectUnder(CursorWorld()); })
	| OnKey(GLFW_KEY_R, [this]() {
		if(Object* o = mLevel->get("player")) {
			o->mPosition = CursorWorld();
			o->mMotion = glm::vec2(0);
		}
	})
	| OnKey(GLFW_KEY_G, [this]() {
		mDrawGrid = !mDrawGrid;
	})
	// Mouse controls
	| OnClick(GLFW_MOUSE_BUTTON_MIDDLE, [this] (auto& a) { selectUnder(a); })
	| HookDrag(GLFW_MOUSE_BUTTON_MIDDLE,  [this](auto& a, auto& b) { return dragMove(a, b); })
	| HookDrag(GLFW_MOUSE_BUTTON_LEFT,  [this](auto& a, auto& b) { return dragMove(a, b); })
	| HookDrag(GLFW_MOUSE_BUTTON_RIGHT, [this](auto& a, auto& b) { return dragResize(a, b); })
	// On drop
	| OnDrop([this](auto* c) { return this->onDrop(c); })

	// Random stuff
	| OnKey(GLFW_KEY_0, []() { PlaySfx("res/prefab/HoverCam/hover.wav", CursorWorld(), 1, 1); })
	| OnKey(GLFW_KEY_9, []() {
		if(YAML::Node audio = GetSettings()["audio"]) {
			if(YAML::Node bgm = audio["bgm"]) {
				PlayBgm(bgm.as<std::string>("").c_str(), 2, false);
			}
		}
	})
	;
}

void Editor::update(float dt) {
	if(!mLevel) { return; }
}

void Editor::draw() {
	if(!mLevel) return;

	glDisable(GL_TEXTURE_2D);

	if(mDrawGrid) {
		glLineWidth(1);
		auto& b = CamBounds();

		glm::vec2 start = b.min - glm::mod(b.min, glm::vec2(mSnapDistance));

		glColor3f(.5f, .5f, .5f);
		glBegin(GL_LINES);
		for(float x = start.x; x <= b.max.x; x += mSnapDistance) {
			glVertex2f(x, b.min.y);
			glVertex2f(x, b.max.y);
		}

		for(float y = start.y; y <= b.max.y; y += mSnapDistance) {
			glVertex2f(b.min.x, y);
			glVertex2f(b.max.x, y);
		}
		glEnd();
	}


	glLineWidth(3);
	mLevel->debugDraw(CamBounds());

	if(mSelected) {
		glLineWidth(5);
		Box& b = mSelected->mBounds;
		glColor3f(1, 1, 0);
		glBegin(GL_LINE_LOOP);
			glVertex2fv(&b.min[0]);
			glVertex2f (b.max.x, b.min.y);
			glVertex2fv(&b.max[0]);
			glVertex2f (b.min.x, b.max.y);
		glEnd();

		glPointSize(16);
		glBegin(GL_POINTS);
			glColor3f(1, 0, 0);
			glVertex2fv(&mSelected->mPosition[0]);
		glEnd();
	}
}

void Editor::selectUnder(const glm::vec2& p) {
	std::vector<Object*> o;
	mLevel->at(p, -1, o, -1);

	if(o.empty()) {
		mSelected = nullptr;
		editorError("No object under cursor!");
		return;
	}

	// Selecting the next after the currently selected (with loop-back)
	for (size_t i = 0; i < o.size(); i++) {
		if(o[i] == mSelected) {
			mSelected = o[(i + 1) % o.size()];
			return;
		}
	}

	mSelected = o[0];
}

void Editor::eraseCurrent() {
	if(mSelected) {
		mLevel->removeObject(mSelected);
		mSelected = nullptr;
	}
	else
		editorError("No object selected!");
}

bool Editor::dragResize(const glm::vec2 &at, const glm::vec2 &dist) {
	glm::vec2 mouse = ViewportToWorld(at);

	if(!mSelected) selectUnder(mouse);

	if(mSelected) {
		Box&      b     = mSelected->mRelativeBounds;

		bool movable = (mSelected->mType & (Object::DYNAMIC | Object::PARTICLE)) != 0;

		if(mSnap) {
			if(movable) mouse -= mSelected->mPosition;
			mouse = glm::round(mouse / mSnapDistance) * mSnapDistance;
			if(movable) mouse += mSelected->mPosition;
		}

		glm::vec2 mdist = mouse - mSelected->mBounds.middle();

		mouse -= mSelected->mPosition;

		if(mdist.x < 0) b.min.x = mouse.x;
		else            b.max.x = mouse.x;

		if(mdist.y < 0) b.min.y = mouse.y;
		else            b.max.y = mouse.y;

		b.fix();
	}

	return true;
}

bool Editor::dragMove(const glm::vec2 &at, const glm::vec2 &dist) {
	glm::vec2 mouse = ViewportToWorld(at);

	if(!mSelected) selectUnder(mouse);

	if(mSelected) {
		glm::vec2 world_scale {
			CamTransform()[0],
			CamTransform()[5]
		};

		glm::vec2 move_amount = dist / world_scale;

		bool movable = (mSelected->mType & (Object::DYNAMIC | Object::PARTICLE)) != 0;

		if(movable) {
			mSelected->mPosition += move_amount;
		}
		else {
			mSelected->mRelativeBounds.min += move_amount;
			mSelected->mRelativeBounds.max += move_amount;
		}

		mSelected->mMotion = glm::vec2(0);
	}

	return true;
}

bool Editor::onDrop(const char *c) {
	return false;
}

#include <fstream>

void Editor::load() {
	using namespace std::chrono;
	auto begin = high_resolution_clock::now();

	std::ifstream file("res/level.yml", std::ios::binary);
	if(!file) return;

	YAML::Node data = YAML::Load(file);
	if(YAML::Node n = data["player"]) {
		Object* player = mLevel->get("player");
		if(!player) {
			return; // TODO: generate new player
		}
		player->read(n);
	}

	if(YAML::Node objects = data["objects"]) {
		mLevel->clear();

		for(YAML::Node nn : objects) {
			std::unique_ptr<Object> o(new Object);
			o->read(nn);
			mLevel->addOwned(std::move(o));
		}
	}

	auto end = high_resolution_clock::now();
	printf("Loading level took %fms\n", duration_cast<microseconds>(end - begin).count() * 0.001f);
}

void Editor::save() {

}

void Editor::editorError(const char *msg) {
	auto now = std::chrono::high_resolution_clock::now();

	if(now - last_error > std::chrono::milliseconds(300)) {
		printf("\a[Editor] %s\n", msg);
		last_error = now;
	}
}
