workspace "Arcnum"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Arcnum"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Shared Library Project
project "Spark"
    location "Spark"
    kind "SharedLib"
    language "C++"
    cdialect "C17"
    cppdialect "C++20"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files {
        "Spark/src/**.cpp",
        "Spark/include/**.hpp"
    }

    includedirs {
        "Spark/include"
    }
    
    pchheader "spark_pch.hpp"
    pchsource "Spark/src/spark_pch.cpp"

    defines { 
        "SPARK_EXPORTS",  -- Export symbols for shared library
        "NOMINMAX",       -- Prevent Windows macros
        "WIN32_LEAN_AND_MEAN" -- Minimize Windows headers
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

project "TestMod"
    location "TestMod"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files {
        "TestMod/src/**.cpp",
        "TestMod/include/**.hpp"
    }

    includedirs {
        "Spark/include",
        "TestMod/include"
    }
    
    links {
        "Spark"
    }
    
    defines { 
        "SPARK_EXPORTS",  -- Export symbols for shared library
        "NOMINMAX",       -- Prevent Windows macros
        "WIN32_LEAN_AND_MEAN" -- Minimize Windows headers
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"


-- Main Application Project
project "Arcnum"
    location "Arcnum"
    kind "ConsoleApp"
    language "C++"
    cdialect "C17"
    cppdialect "C++20"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files {
        "Arcnum/src/**.cpp",
        "Arcnum/src/**.hpp"
    }

    includedirs {
        "Spark/include"
    }

    libdirs {
        "bin/" .. outputdir .. "/Spark", -- For linking shared library
    }

    links {
        "Spark", -- Default to linking the shared library
    }

    postbuildcommands {
        -- Copy SparkShared.dll to the Arcnum output directory
        "{COPY} ../bin/" .. outputdir .. "/Spark/*.dll %{cfg.targetdir}"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
