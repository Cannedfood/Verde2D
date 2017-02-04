#include <yaml-cpp/yaml.h>

namespace internal {
	extern YAML::Node Settings;
}

static inline
YAML::Node& GetSettings() { return internal::Settings; }
