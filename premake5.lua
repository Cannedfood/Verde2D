workspace "Verde"
	language "C++" flags "C++14"

	configurations { "Debug", "Release" }

	filter "configurations:Debug"
		optimize "Debug"
		symbols "On"
	filter "configurations:Release"
		optimize "Speed"
	filter "*"

	startproject "verde"

-- Library stuff

local libs -- table containing all library names/locations

if os.is "linux" then
	libs = {
		lua     = "lua",
		gl      = "GL",
		glfw    = "glfw",
		yamlcpp = "yaml-cpp",
		openal  = "openal"
	}
elseif os.is "windows" then
	flags { "NoMinimalRebuild", "MultiProcessorCompile" }

	libs = {
		lua     = "lua32",
		gl      = "OpenGL32.lib",
		glfw    = "glfw32.lib",
		yamlcpp = "yamlcpp32",
		openal  = "OpenAL32.lib"
	}
else
	error(
		"I haven't written the libraries for your system yet." ..
		"Assuming you're on a *nix operating system, " ..
		"you can mostly copy the settings from linux"
	)
end

if not os.findlib(libs.glfw) then
	if os.is "windows" then
		libs.glfw = "win/lib/%{cfg.shortname}/glfw3.lib"
		includedirs "win/lib/include"
	else
		error "Couldn't find glfw. You seem to be on a unix system: install it."
	end
end

if not os.findlib(libs.openal) then
	if os.is "windows" then
		libs.openal = "win/lib/%{cfg.shortname}/OpenAL32.lib"
		includedirs "win/lib/include"
	else
		error "Couldn't find openal. You seem to be on a unix system: install it."
	end
end

if os.isdir "src/glm" then
	includedirs "src/glm"
end
defines "GLM_ENABLE_EXPERIMENTAL"

if not os.findlib(libs.yamlcpp) then

project(libs.yamlcpp)
	kind "StaticLib"
	files "src/yaml-cpp/src/**"

	project "*"
		includedirs "src/yaml-cpp/include"

end

if not os.findlib(libs.lua) then

project(libs.lua)
	kind "StaticLib"
	files {
		"src/lua/**.c",
		"src/lua/**.h"
	}

	removefiles {
		"src/lua/lua.c", -- Lua cli interpreter
		"src/lua/luac.c" -- Lua cli compiler
	}
end


project "verde"
	kind "WindowedApp"
	files "src/Verde/**"
	removefiles "src/Verde/Audio.*"
	links {
		libs.gl,
		libs.glfw,
		libs.yamlcpp,
		libs.openal
	}
