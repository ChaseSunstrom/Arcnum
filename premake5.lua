-- Premake configuration for Arcnum
workspace "Arcnum"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Arcnum"

-- Common Boost library prefix and suffix
local boost_lib_prefix = "libboost_"
local debug_suffix = "-vc143-mt-gd-x64-1_83"
local release_suffix = "-vc143-mt-x64-1_83"

-- Configuration for the Arcnum project
project "Arcnum"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    cppdialect "C++20"
    dependson { "Spark" }
    files { "src/**.cpp", "src/**.hpp", "src/**.h" }

   includedirs {
       "Spark",
        "Spark/include",
        "Spark/include/BOOST",
        "Spark/include/IRRKLANG",
        "Spark/include/GLEW",
        "Spark/include/GLFW",
        "Spark/include/GLM",
        "Spark/include/OTHER"
    }

    libdirs {
        "Spark/lib",
        "Spark/lib/BOOST",
        "Spark/lib/IRRKLANG",
        "Spark/lib/GLEW",
        "Spark/lib/GLFW",
    }
    links {
        "Spark",
        "glfw3",
        "glew32",
        "opengl32.lib",
        "irrKlang",
    }
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        optimize "Off"
        links {
            boost_lib_prefix .. "system" .. debug_suffix,
            boost_lib_prefix .. "serialization" .. debug_suffix,
        }
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Full"
        links {
            boost_lib_prefix .. "system" .. release_suffix,
            boost_lib_prefix .. "serialization" .. release_suffix,
        }
    filter {} -- Reset filter for other settings

-- Configuration for the Spark project
project "Spark"
    kind "StaticLib"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}/lib"
    cppdialect "C++20"
    files { "Spark/core/**.cpp", "Spark/core/**.hpp", "Spark/core/**.h" }
    includedirs {
        "Spark/include",
        "Spark/include/BOOST",
        "Spark/include/IRRKLANG",
        "Spark/include/GLEW",
        "Spark/include/GLFW",
        "Spark/include/GLM",
        "Spark/include/OTHER"
    }

    libdirs {
        "Spark/lib",
        "Spark/lib/BOOST",
        "Spark/lib/IRRKLANG",
        "Spark/lib/GLEW",
        "Spark/lib/GLFW",
    }
    links {
        "glfw3",
        "glew32",
        "opengl32.lib",
        "irrKlang",
    }
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        optimize "Off"
        links {
            boost_lib_prefix .. "system" .. debug_suffix,
            boost_lib_prefix .. "serialization" .. debug_suffix,
        }
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Full"
        links {
            boost_lib_prefix .. "system" .. release_suffix,
            boost_lib_prefix .. "serialization" .. release_suffix,
        }
    filter {} -- Reset filter for other settings
