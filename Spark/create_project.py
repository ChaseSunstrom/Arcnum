import os
import shutil
import sys
import subprocess
import requests
from packaging import version
from zipfile import ZipFile

def download_premake(destination_dir):
    premake_url = "https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-windows.zip"
    zip_path = os.path.join(destination_dir, "premake.zip")

    response = requests.get(premake_url, stream=True)
    if response.status_code == 200:
        with open(zip_path, 'wb') as f:
            f.write(response.content)

        # Unzip the downloaded file into a "premake" folder
        premake_dir = os.path.join(destination_dir, "premake")
        os.makedirs(premake_dir, exist_ok=True)
        with ZipFile(zip_path, 'r') as zip_ref:
            zip_ref.extractall(premake_dir)
        os.remove(zip_path)
        print("Premake downloaded and unzipped successfully into the 'premake' directory.")
    else:
        print("Failed to download Premake.")
        sys.exit(1)


def run_premake(project_dir, project_type):
    premake_executable = "premake5.exe"
    premake_path = os.path.join(project_dir, "premake", premake_executable)

    # Check if premake5 is in the project directory, if not, download it
    if not os.path.isfile(premake_path):
        print("Premake5 not found, downloading...")
        download_premake(project_dir)

    # Change directory to the project directory
    os.chdir(project_dir)

    # Check if premake5.lua exists
    if not os.path.isfile(os.path.join(project_dir, "premake5.lua")):
        print("premake5.lua not found in the project directory.")
        return

    # Run Premake
    try:
        subprocess.run([premake_path, project_type], check=True)
        print(f"Premake executed successfully for project type: {project_type}")
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while running Premake: {e}")
    except FileNotFoundError:
        print("Premake5 executable not found. Please ensure it is downloaded and located in the project directory.")


def copy_glew_dll(spark_dir, project_dir):
    glew_dll_path = os.path.join(spark_dir, 'glew32.dll')
    if os.path.isfile(glew_dll_path):
        # Copy glew32.dll to the Debug and Release folders
        for config in ['Debug', 'Release']:
            dest_dir = os.path.join(project_dir, 'bin', config)
            os.makedirs(dest_dir, exist_ok=True)  # Create the directory if it does not exist
            shutil.copy(glew_dll_path, dest_dir)
        print("glew32.dll copied to 'Debug' and 'Release' folders.")
    else:
        print("glew32.dll not found in the Spark directory.")

def get_latest_release_tag(github_repo):
    api_url = f"https://api.github.com/repos/{github_repo}/tags"
    response = requests.get(api_url)
    if response.status_code == 200:
        tags = response.json()
        if tags:
            latest_tag = tags[0]['name']
            return latest_tag
        else:
            print("No tags found in the repository.")
            return None
    else:
        print(f"Failed to fetch tags. Status Code: {response.status_code}")
        return None

def get_version_from_file(file_path):
    if os.path.exists(file_path):
        with open(file_path, 'r') as file:
            content = file.readline().strip()
            version_number = content.split()[1]
            return version_number
    return "0.0.0"


def download_and_extract_repo(github_repo, temp_dir, tag_name):
    repo_url = f'https://github.com/{github_repo}/archive/refs/tags/{tag_name}.zip'
    response = requests.get(repo_url)
    if response.status_code != 200:
        print(f"Failed to download the repository. Status Code: {response.status_code}")
        sys.exit(1)

    zip_path = os.path.join(temp_dir, 'download.zip')
    with open(zip_path, 'wb') as file:
        file.write(response.content)

    with ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extractall(temp_dir)
    os.remove(zip_path)

    extracted_dir_name = [name for name in os.listdir(temp_dir) if os.path.isdir(os.path.join(temp_dir, name))][0]
    extracted_path = os.path.join(temp_dir, extracted_dir_name)
    return extracted_path

def update_spark_if_newer(spark_dir, temp_dir, github_repo):
    tag_name = get_latest_release_tag(github_repo)
    if not tag_name:
        print("Unable to get the latest version tag.")
        return

    latest_version_tag = tag_name.lstrip('v')
    local_version = get_version_from_file(os.path.join(spark_dir, 'version.txt'))

    # Check if the local version is less than the latest version
    if local_version < latest_version_tag:
        print(f"Updating Spark from version {local_version} to {latest_version_tag}.")
        extracted_path = download_and_extract_repo(github_repo, temp_dir, tag_name)
        if os.path.exists(spark_dir):
            shutil.rmtree(spark_dir)
        shutil.move(extracted_path, spark_dir)
        print("Spark has been updated successfully.")

        # Write the new version to the version.txt file
        version_file_path = os.path.join(spark_dir, 'version.txt')
        with open(version_file_path, 'w') as version_file:
            version_file.write(f"VERSION {latest_version_tag}")
        print(f"Updated version.txt to VERSION {latest_version_tag}.")
    else:
        print(f"Local Spark is up to date with version {local_version}.")


def create_project(project_name, project_type, project_dir):
    default_spark_dir = "C:/Spark"
    temp_dir = 'C:/SparkDownloadTemp'
    github_repo = 'ChaseSunstrom/Spark'

    # Check and update Spark if necessary
    os.makedirs(temp_dir, exist_ok=True)
    tag_name = get_latest_release_tag(github_repo)
    
    if tag_name:
        update_spark_if_newer(default_spark_dir, temp_dir, github_repo)

    if os.path.exists(temp_dir) and not os.listdir(temp_dir):
        os.rmdir(temp_dir)

    new_project_dir = os.path.join(project_dir, project_name)
    new_spark_dir = os.path.join(new_project_dir, "Spark")
    src_dir = os.path.join(new_project_dir, "src")

    # Create the new project directory
    if not os.path.exists(new_project_dir):
        os.makedirs(new_project_dir)

    # Copy the updated Spark directory into the new project directory
    shutil.copytree(default_spark_dir, new_spark_dir)

    update_script_src = os.path.join(default_spark_dir, 'update_project.py')

    # Define the destination path (where you want to copy update_project.py)
    update_script_dest = os.path.join(new_project_dir, 'update_project.py')

    # Check if the file exists and then copy it
    if os.path.isfile(update_script_src):
        shutil.copy(update_script_src, update_script_dest)
        print(f"Copied 'update_project.py' to {new_project_dir}")
    else:
        print("Warning: 'update_project.py' not found in the Spark directory.")

    # Create Premake file
    premake_content = f'''
-- Premake configuration for {project_name}
workspace "{project_name}"
    architecture "x64"
    configurations {{"Debug", "Release"}}
    startproject "{project_name}"  -- Set the startup project

project "{project_name}"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{{cfg.buildcfg}}"

    cppdialect "C++20"

    dependson {{"Spark"}}

    files {{"src/**.cpp", "src/**.hpp"}}

    includedirs {{"Spark", "Spark/include/**"}}
    libdirs {{"Spark/lib/**"}}
    links {{"Spark", "glfw3", "glew32", "opengl32.lib"}}

    filter "configurations:Debug"
        defines {{"DEBUG"}}
        symbols "On"
        optimize "On"

    filter "configurations:Release"
        defines {{"NDEBUG"}}
        optimize "Full"

-- Configuration for the Spark project
project "Spark"
    kind "StaticLib"
    language "C++"
    targetdir "bin/%{{cfg.buildcfg}}"

    cppdialect "C++20"

    files {{"Spark/core/**.cpp", "Spark/core/**.hpp"}}

    includedirs {{"Spark/include/**"}}
    libdirs {{"Spark/lib/**"}}
    links {{"glfw3", "glew32", "opengl32.lib"}}

    filter "configurations:Debug"
        defines {{"DEBUG"}}
        symbols "On"
        optimize "On"

    filter "configurations:Release"
        defines {{"NDEBUG"}}
        optimize "Full"
    '''
    with open(os.path.join(new_project_dir, "premake5.lua"), "w") as file:
        file.write(premake_content)

    # Create core_include.hpp
    core_include_content = f'''
#ifndef {project_name.upper()}_CORE_INCLUDE_HPP
#define {project_name.upper()}_CORE_INCLUDE_HPP

// Core includes for {project_name}
#include <core/core.hpp>
#include <core/ecs/ecs.hpp>
#include <core/events/event.hpp>
#include <core/events/sub.hpp>
#include <core/logging/log.hpp>
#include <core/renderer/batcher.hpp>
#include <core/renderer/renderer.hpp>
#include <core/renderer/layer.hpp>
#include <core/renderer/layer_stack.hpp>
#include <core/window/window.hpp>
#include <core/util/file.hpp>
#include <core/util/wrap.hpp>
#include <core/scene/scene_manager.hpp>
#include <core/app/app_interface.hpp>

#endif // {project_name.upper()}_CORE_INCLUDE_HPP
'''

    if not os.path.exists(src_dir):
        os.makedirs(src_dir)
    with open(os.path.join(src_dir, "core_include.hpp"), "w") as file:
        file.write(core_include_content)

    # Create game.cpp
    game_cpp_content = '''
#include "core_include.hpp"
#include "game.hpp"

// required function
void on_start()
{
	// Default delta time
	ac::renderer::s_fixed_delta_time = 0.005;

	// Default tick speed
	ac::renderer::s_tick_speed = 60;
}

// required function
void on_update()
{

}

// required function
// will recieve events from everything, it is automatically subscribed to recieve
// every event
bool on_event(std::shared_ptr<ac::event> event)
{
	return true;
}

// Registers functions to be called at different points in the application
// automatically
void register_functions()
{
	ac::app_functions::register_functions(on_start, on_update, on_event);
}
'''
    with open(os.path.join(src_dir, "game.cpp"), "w") as file:
        file.write(game_cpp_content)

    game_hpp_content = '''
#ifndef GAME_HPP
#define GAME_HPP

#include <core/events/event.hpp>

void on_start();
void on_update();
bool on_event(std::shared_ptr<ac::event> event);

void register_functions();

#endif
'''
    with open(os.path.join(src_dir, "game.hpp"), "w") as file:
        file.write(game_hpp_content)

    main_cpp_contents = '''
#include <core/core_main.hpp>
#include <core/logging/log.hpp>

#include "game.hpp"

int main()
{
	A_CORE_INFO("[COMPILED AT]: " << __DATE__ << " " << __TIME__);

	// Dont change these
	register_functions();
	ac::core_main();
}
'''
    with open(os.path.join(src_dir, "main.cpp"), "w") as file:
        file.write(main_cpp_contents)

    copy_glew_dll(default_spark_dir, new_project_dir)

    print(f"Project '{project_name}' created successfully at '{new_project_dir}'.")

    run_premake(new_project_dir, project_type)

    print(f"Project '{project_name}' created successfully at '{new_project_dir}'.")


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python script.py [project_name] [project_type] [project_directory]")
        sys.exit(1)
    project_name = sys.argv[1]
    project_type = sys.argv[2]
    project_dir = sys.argv[3]

    # Check if the specified project directory exists, if not, create it
    if not os.path.exists(project_dir + "/" + project_name):
        os.makedirs(project_dir + "/" + project_name)
        create_project(project_name, project_type, project_dir)

    else:
        print(f'Project already exists {project_dir + "/" + project_name}, skipping project creation.')