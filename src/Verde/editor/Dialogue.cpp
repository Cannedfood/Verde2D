#include "Dialogue.hpp"

#ifdef __GNUC__
#	include <memory.h>
#	include <cstdio>

	std::string FileDialogue(const char* title, const char* folder, const char* filter) {
		char path[2048];

		std::string zenity_command = "zenity --file-selection";

		if(title) {
			zenity_command += " --title ";
			zenity_command += title;
		}

		if(folder) {
			zenity_command += " \"--filename=";
			zenity_command += folder;
			zenity_command += '"';
		}

		if(filter) {
			zenity_command += " --file-filter ";
			zenity_command += filter;
		}

		if(FILE* f = popen(zenity_command.c_str(), "r")) {
			memset(path, 0, sizeof(path));
			fgets(path, sizeof(path) - 1, f);

			pclose(f);

			return path;
		}

		return "";
	}

	int EditDialogue(const char* file) {
		std::string command = "xdg-open ";
		command += file;
		return system(command.c_str());
	}
#else // _GNUC_
	std::string FileDialogue(const char* title, const char* folder, const char* filter) { return ""; }
	int EditDialogue(const char *file) { return -1; }
#endif
