#include "Graphics.hpp"

#include "StaticGraphics.hpp"

#include <unordered_map>

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
			p = std::make_shared<StaticGraphics>();
			if(!((StaticGraphics*)p.get())->load(s)) // Failed loading texture
				p.reset();
		}
		return p;
	}
	else {
		auto p = std::make_shared<StaticGraphics>();
		if(((StaticGraphics*)p.get())->load(s))
			return p;
		else
			return nullptr;
	}
}

std::shared_ptr<Graphics> Graphics::Load(YAML::Node n) {
	if(!n) return nullptr;

	auto p = Load(n["path"].as<std::string>());

	if(YAML::Node nn = n["wrapping"]) {
		p->setWrapping({
			nn[0].as<float>(),
			nn[1].as<float>()
		});
	}

	return p;
}
