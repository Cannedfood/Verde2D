#pragma once

#include <string>

std::string FileDialogue(const char* title, const char* folder = nullptr, const char* filter = nullptr);
int EditDialogue(const char* file);
