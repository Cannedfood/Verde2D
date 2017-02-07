#include "Graphics.hpp"

#include "StaticGraphics.hpp"
#include "AtlasAnimation.hpp"

#include <unordered_map>
#include <cstring>
#include <fstream>

void Graphics::writeOrPrefab(YAML::Emitter &to) {
	if(mPrefab.size() != 0)
		to << mPrefab;
	else
		write(to);
}

std::unordered_map<std::string, std::shared_ptr<Graphics>>* pGraphicsCache = nullptr;

void Graphics::InitCache() {
	pGraphicsCache = new std::unordered_map<std::string, std::shared_ptr<Graphics>>;
}

void Graphics::FreeCache() {
	delete pGraphicsCache;
}

void Graphics::CleanCache() {
	// TODO: remove unique shared_ptr from cache
}

std::shared_ptr<Graphics> Graphics::Load(const std::string &s) {
	if(pGraphicsCache) {
		auto& p = (*pGraphicsCache)[s];
		if(!p) {
			if(s.size() > 4 && strcmp(s.c_str() + s.size() - 4, ".yml") == 0) {
				std::ifstream file(s, std::ios::binary);
				if(!file)
					printf("Failed loading description file %s\n", s.c_str());
				else {
					YAML::Node data;
					try {
						data = YAML::Load(file);
						p = Graphics::Load(data);
						p->mPrefab = s;
					}
					catch(std::exception& e) {
						printf("Failed loading %s: %s\n", s.c_str(), e.what());
						return nullptr;
					}
				}
			}
			else {
				p = std::make_shared<StaticGraphics>();

				YAML::Node node;
				node = s;

				try {
					if(!((StaticGraphics*)p.get())->load(node)) // Failed loading texture
						p.reset();
				}
				catch(std::exception& e) {
					printf("Failed loading %s: %s\n", s.c_str(), e.what());
					return nullptr;
				}
			}
		}
		return p;
	}
	else {
		auto p = std::make_shared<StaticGraphics>();
		YAML::Node tmp;
		tmp = s;
		if(((StaticGraphics*)p.get())->load(tmp))
			return p;
		else
			return nullptr;
	}
}

std::shared_ptr<Graphics> Graphics::Load(YAML::Node n) {
	if(!n) return nullptr;

	if(n.IsScalar())
		return Load(n.as<std::string>());

	std::shared_ptr<Graphics> p;

	YAML::Node type_n = n["type"];
	if(type_n) {
		std::string type_s = type_n.as<std::string>();
		if(type_s != "default") {
			if(type_s == "atlas-animation") {
				p = std::make_shared<AtlasAnimation>();
				if(((AtlasAnimation*) p.get())->load(n))
					return p;
				else {
					printf("Failed loading atlas-animation\n");
					return nullptr;
				}
			}

			printf("Graphics: Type %s not supported\n", type_s.c_str());
			return nullptr;
		}
	}

	p = Load(n["texture"].as<std::string>());

	if(YAML::Node nn = n["wrapping"]) {
		if(nn.IsSequence()) {
			p->setWrapping({
				nn[0].as<float>(),
				nn[1].as<float>()
			});
		}
		else {
			p->setWrapping(glm::vec2(nn.as<float>()));
		}
	}

	return p;
}
