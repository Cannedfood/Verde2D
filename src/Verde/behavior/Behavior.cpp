#include "Behavior.hpp"
#include "../Object.hpp"

#include "HoverCamFollow.hpp"

#include <string.h>

static bool ends_in(const std::string& s, const char* end) {
	size_t end_len = strlen(end);
	if(s.size() < end_len) return false;
	return strcmp(s.c_str() + s.size() - end_len, end) == 0;
}

bool Behavior::LoadBehavior(Object *o, YAML::Node data) {
	if(!data) return false;

	if(YAML::Node type_n = data["type"]) {
		std::string type_s = type_n.as<std::string>();
		if(ends_in(type_s, ".lua")) {
			printf("Cannot load behavior from %s: Lua scripts not yet supported.\n", type_s.c_str());
			return false;
		}
		if(type_s == "hovercam-follow") {
			o->mBehavior.reset(new HoverCamFollow);
			o->mBehavior->load(o, data);
			if(o->mLevel) // Hot swap behavior
				o->mBehavior->start(o);
			return true;
		}
	}
	else {
		puts("Behavior does not contain a type!");
		return false;
	}

	return false;
}
