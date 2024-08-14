workspace "Arcnum"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Arcnum"

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "NDEBUG"
        runtime "Release"
        optimize "on"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Arcnum"
    location "Arcnum"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files {
        "Arcnum/src/**.hpp",
        "Arcnum/src/**.cpp"
    }

    includedirs {
        "Spark",
        "Spark/include/**"
    }

    links {
        "Spark"
    }

project "Spark"
    location "Spark"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    pchheader "Spark/pch.hpp"
    pchsource "Spark/pch.cpp"

    files {
        "Spark/**.hpp",
        "Spark/**.cpp",
        -- Need these to compile them during the build proccess
        "Spark/lib/%{cfg.system}/glad/glad.c",
        {
            "Spark/lib/%{cfg.system}/glad/glad.c",
            flags { "NoPCH" }
        },
        "Spark/include/glad/**.h"
    }

    includedirs {
        "Spark",
        "Spark/include"
    }

    cfg = {}
    cfg.system = iif(os.target() == "macosx", "macos", os.target())
    cfg.architecture = iif(os.is64bit(), "x64", "x86")

    filter "system:windows"
        systemversion "latest"
        links {
            "opengl32.lib",
            "Spark/lib/windows/glfw/glfw3.lib"
        }
        defines { "_CRT_SECURE_NO_WARNINGS" }
        libdirs {
            "Spark/lib/windows/glfw/**",
            "Spark/lib/windows/glad/**"
        }
