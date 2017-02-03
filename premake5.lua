workspace "Verde"
	language "C++" flags "C++14"

	configurations "Release"
	optimize "Debug"
	symbols "On"


project "verde"
	kind "WindowedApp"
	files "src/Verde/**"
	links { "glfw", "GL", "yaml-cpp" }

project "yaml-cpp"
	kind "StaticLib"
	files "src/yaml-cpp/src/**"

	project "*"
		includedirs "src/yaml-cpp/include"
