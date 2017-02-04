workspace "Verde"
	language "C++" flags "C++14"

	configurations "Release"
	optimize "Debug"
	symbols "On"


local libs

if os.is "linux" then
	libs = {
		lua     = "lua",
		gl      = "GL",
		glfw    = "glfw",
		yamlcpp = "yaml-cpp",
		openal  = "openal"
	}
elseif os.is "windows" then
	libs = {
		lua     = "lua32",
		gl      = "OpenGL32",
		glfw    = "glfw32",
		yamlcpp = "yamlcpp32",
		openal  = "OpenAL32"
	}
end


project "verde"
	kind "WindowedApp"
	files "src/Verde/**"
	links {
		libs.gl,
		libs.glfw,
		libs.yamlcpp,
		libs.openal
	}

if not os.findlib "yaml-cpp" then

project(libs.yamlcpp)
	kind "SharedLib"
	files "src/yaml-cpp/src/**"

	project "*"
		includedirs "src/yaml-cpp/include"

end

if not os.findlib "lua" then

project(libs.lua)
	kind "SharedLib"
	files {
		"src/lua/**.c",
		"src/lua/**.h"
	}

	exclude {
		"src/lua/lua.c", -- Lua cli interpreter
		"src/lua/luac.c" -- Lua cli compiler
	}

end
