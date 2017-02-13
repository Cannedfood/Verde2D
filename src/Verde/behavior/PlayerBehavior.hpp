#pragma once

class PlayerBehavior {
	PlayerBehavior();
	~PlayerBehavior();

	virtual void start (Object* o);
	virtual void update(Object* o, float dt);

	virtual void load  (Object* o, YAML::Node n);
	virtual void write (Object* o, YAML::Emitter& e);
};
