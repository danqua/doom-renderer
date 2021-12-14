workspace "Harm"
	architecture "x86_64"
	language "C++"
	cppdialect "C++17"
	debugdir "data"
	flags { "MultiProcessorCompile" }
	configurations { "Debug", "Release" }
	targetdir( path.join("build", "%{wks.name}"))
	objdir( path.join("build", "%{wks.name}"))

project "Harm"
	kind "ConsoleApp"
	includedirs {
		"src",
		"extern/glm-0.9.9.8",
		"extern/SDL2-2.0.18/include",
		"extern/glad/include",
		"extern/stb"
	}
	files {
		"src/**.h",
		"src/**.cpp",
		"extern/glad/src/glad.c"
	}
	libdirs {
		"extern/SDL2-2.0.18/lib/x64"
	}
	links {
		"SDL2"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "Full"