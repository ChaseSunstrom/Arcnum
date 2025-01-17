workspace "Arcnum"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Arcnum"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Spark"
    location "Spark"
    kind "StaticLib"
    language "C++"
    cdialect "C17"
    cppdialect "C++20"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files {
        "Spark/src/**.c",
        "Spark/src/**.cpp"
    }


    includedirs {
        "Spark/include",
    }
    
    pchheader "spark_pch.hpp"
    pchsource "Spark/src/spark_pch.cpp"

    defines { 
        "AL_LIBTYPE_STATIC",
        "NOMINMAX",  -- Prevents Windows.h from defining min/max macros
        "WIN32_LEAN_AND_MEAN"  -- Reduces Windows header bulk
    }

    links {
        "d3d11.lib",
        "d3dcompiler.lib",
        "dxgi.lib",
        "opengl32.lib",
        "winmm.lib",
        "imm32.lib",
        "version.lib",
        "setupapi.lib",
        "ws2_32.lib",
        "Avrt.lib"
    }

    filter "configurations:Debug"
        defines { "DEBUG", "_ITERATOR_DEBUG_LEVEL=2" }
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG", "_ITERATOR_DEBUG_LEVEL=0" }
        runtime "Release"
        optimize "on"

project "Arcnum"
    location "Arcnum"
    kind "ConsoleApp"
    language "C++"
    cdialect "C17"
    cppdialect "C++20"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files {
        "Arcnum/src/**.hpp",
        "Arcnum/src/**.cpp",
        "Arcnum/src/**.c"
    }

    includedirs {
        "Spark/include",
    }

    links {
        "Spark"
    }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "NDEBUG"
        runtime "Release"
        optimize "on"