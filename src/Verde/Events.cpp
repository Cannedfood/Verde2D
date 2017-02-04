#include "Events.hpp"

#include <unordered_map>
#include <list>
#include <string>
#include <functional>

static std::unordered_map<std::string, std::list<std::function<void()>>>* pButtonCallbacks;
static std::unordered_map<std::string, std::list<std::function<void()>>>* pAxisCallbacks;
static std::list<std::function<void(const char**, size_t)>>*               pDropCallbacks;
