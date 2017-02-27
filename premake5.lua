newoption {
	trigger = "static-deps",
	description = "compile into binary"
}

newoption {
	trigger     = "native",
	description = "Use optimizations for current processor"
}

workspace "Verde"

 -----------------------------
--- General option defaults ---
 -----------------------------
	language "C++" flags "C++14"

	floatingpoint "Fast"
	vectorextensions "SSE2"

	startproject "verde"


 --------------------
--- Configurations ---
 --------------------

	if _OPTIONS.native then
		if os.is "linux" then
			buildoptions "-mtune=native"
		end
	end

	configurations { "Debug", "Release" }
	platforms      { "amd64", "i386" }


	filter "platforms:amd64" architecture "x86_64"
	filter "platforms:i386"  architecture "x86"

	filter "configurations:Debug"
		optimize "Debug"
		symbols "On"
	filter "configurations:Release"
		optimize "Speed"
		flags {
			"NoFramePointer",
			"NoRuntimeChecks"
		}
	filter "*"

 -----------------
-- Library stuff --
 -----------------

local function precompiled_lib(name)
	if os.is "windows" then
		return path.join("win/lib/%{cfg.shortname}/", name)
	end
	error "No precompiled libs included for this OS"
end

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
		"I haven't written the libraries for your system into premake5.lua yet." ..
		"Assuming you're on a *nix operating system, " ..
		"you can mostly copy the settings from linux"
	)
end

if not os.findlib(libs.glfw) then
	if os.is "windows" then
		libs.glfw = precompiled_lib "glfw3.lib"
		includedirs "win/include"
	else
		error "Couldn't find glfw. You seem to be on a unix system: install it."
	end
end

if not os.findlib(libs.openal) then
	if os.is "windows" then
		libs.openal = precompiled_lib "OpenAL32.lib"
		includedirs "win/include"
	else
		error "Couldn't find openal. You seem to be on a unix system: install it."
	end
end

if os.isdir "src/glm" then
	includedirs "src/glm"
end
defines "GLM_ENABLE_EXPERIMENTAL"

if _OPTIONS["static-deps"] or not os.findlib(libs.yamlcpp) then

libs.yamlcpp = "yamlcpp_static"

group "dependencies"
project(libs.yamlcpp)
	kind "StaticLib" group "dependencies"
	files "src/yaml-cpp/src/**"

	project "*"
		includedirs "src/yaml-cpp/include"

end

if _OPTIONS["static-deps"] or not os.findlib(libs.lua) then

libs.lua = "lua_static"

group "dependencies"
project(libs.lua) language "C"
	kind "StaticLib"
	files {
		"src/lua/**.c",
		"src/lua/**.h"
	}

	removefiles {
		"src/lua/lua.c", -- Lua cli interpreter
		"src/lua/luac.c" -- Lua compiler
	}

	optimize "Speed"
	symbols "Off"
	flags {
		"NoFramePointer"
	}

	if os.is "linux" then
		defines "LUA_USE_LINUX"
	end
end


 ----------------
-- Main project --
 ----------------

group ""
project "verde"
	-- Lying to build system in debug, because cl output is necessary, right VS??
	filter "configurations:Release" kind "WindowedApp"
	filter "configurations:Debug"   kind "ConsoleApp"
	filter "*"

	files "src/Verde/**"
	links {
		libs.gl,
		libs.glfw,
		libs.yamlcpp,
		libs.openal
	}

	if not os.is "windows" then
		links "pthread"
	end
