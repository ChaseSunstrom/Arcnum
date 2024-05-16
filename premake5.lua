-- Premake configuration for Arcnum
workspace "Arcnum"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Arcnum"

-- Common Boost library prefix and suffix
local boost_lib_prefix = "libboost_"
local debug_suffix = "-vc143-mt-gd-x64-1_84"
local release_suffix = "-vc143-mt-x64-1_84"

rule "Shader"
    fileextension ".vert"
    fileextension ".frag"
    fileextension ".geom"
    fileextension ".tese"
    fileextension ".tesc"
    fileextension ".comp"

    -- Assume Vulkan SDK is available in the environment variable VULKAN_SDK
    -- Output directory is constructed using tokens and known structure, avoiding direct cfg reference
    buildcommands {
    "\"${VULKAN_SDK}/Bin/glslc\" %{file.relpath} -o %{file.directory}/%{file.basename}.spv --verbose"
    }

    buildoutputs { "%{file.directory}/%{file.basename}.spv" }



-- Configuration for the Arcnum project
project "Arcnum"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    cppdialect "C++20"
    dependson { "Spark" }
    files { "src/**.cpp", "src/**.hpp", "src/**.h" }
    rules {"Shader"}

   includedirs {
       "Spark",
        "Spark/include",
        "Spark/include/BOOST",
        "Spark/include/IRRKLANG",
        "Spark/include/GLEW",
        "Spark/include/GLFW",
        "Spark/include/GLM",
        "Spark/include/VULKAN",
        "Spark/include/RFL",
        "Spark/include/OTHER"
    }

    libdirs {
        "Spark/lib/**"
    }
    links {
        "Spark",
        "glfw3",
        "glew32",
        "opengl32.lib",
        "irrKlang",
    }

    postbuildcommands {
    -- Copy .dll files from Spark/lib to the appropriate bin/{buildcfg} directory
    "{COPY} %{wks.location}Spark/lib/*.dll bin/%{cfg.buildcfg}"
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
    files { "Spark/core/**.cpp", "Spark/core/**.hpp", "Spark/core/**.h", "Spark/include/IMGUI/**.cpp", "Spark/include/IMGUI/**.h"}
    rules {"Shader"}
    includedirs {
        "Spark/include",
        "Spark/include/BOOST",
        "Spark/include/IRRKLANG",
        "Spark/include/GLEW",
        "Spark/include/GLFW",
        "Spark/include/GLM",
        "Spark/include/RFL",
        "Spark/include/VULKAN",
        "Spark/include/OTHER"
    }

    libdirs {
        "Spark/lib/**"
    }
    links {
        "glfw3",
        "glew32",
        "opengl32.lib",
        "d3d11",
        "irrKlang",
        "vulkan-1"
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