workspace "Verde"
	language "C++" flags "C++14"

	configurations "Release"
	optimize "Debug"
	symbols "On"


project "verde"
	kind "WindowedApp"
	files "src/**"
	links { "glfw", "GL" }
