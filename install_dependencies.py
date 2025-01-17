#!/usr/bin/env python3
import os
import sys
import platform
import subprocess
import pathlib


def run_command(command: str, cwd: str = None, env: dict = None) -> None:
    """
    Run a shell command from an optional working directory with optional environment.
    If the command returns a non-zero exit code, exit the script.
    """
    print(f'Executing command: {command}')
    process = subprocess.run(command, cwd=cwd, shell=True, env=env)
    if process.returncode != 0:
        print(f'Error: Command failed with exit code {process.returncode}')
        sys.exit(process.returncode)


def get_modified_env(installer_dir: pathlib.Path) -> dict:
    """
    Create a modified environment dictionary with updated PATH.
    """
    vcpkg_dir = str(installer_dir / 'vcpkg')
    env = os.environ.copy()
    env['PATH'] = vcpkg_dir + os.pathsep + env.get('PATH', '')
    return env


def update_path_permanent(installer_dir: pathlib.Path) -> None:
    """
    For Windows only: Permanently update the user PATH to include vcpkg.
    Properly preserves existing PATH entries.
    """
    if platform.system() != "Windows":
        return

    vcpkg_dir = str(installer_dir / 'vcpkg')

    # Get the current user PATH (not system PATH)
    try:
        result = subprocess.run('reg query "HKEY_CURRENT_USER\\Environment" /v PATH',
                                capture_output=True, text=True, shell=True)
        if result.returncode == 0:
            # Extract current PATH from reg query output - be very careful with parsing
            path_lines = result.stdout.strip().split('\n')
            for line in path_lines:
                if 'REG_SZ' in line or 'REG_EXPAND_SZ' in line:
                    # The PATH value comes after REG_SZ or REG_EXPAND_SZ
                    current_path = line.split('REG_SZ')[-1].strip() if 'REG_SZ' in line else \
                    line.split('REG_EXPAND_SZ')[-1].strip()
                    break
            else:
                # If we didn't find the PATH, get it from the environment
                current_path = os.environ.get('PATH', '')
        else:
            # Fallback to environment PATH if reg query fails
            current_path = os.environ.get('PATH', '')
    except subprocess.SubprocessError:
        # Fallback to environment PATH if subprocess fails
        current_path = os.environ.get('PATH', '')

    # Check if vcpkg_dir is already in the PATH (ignore case)
    if vcpkg_dir.lower() in current_path.lower():
        print("vcpkg directory already in user PATH.")
        return

    # Prepare new path while preserving existing entries
    new_path = current_path + os.pathsep + vcpkg_dir if current_path else vcpkg_dir

    print(f"Current PATH: {current_path}")
    print(f"Adding to PATH: {vcpkg_dir}")
    print(f"New PATH will be: {new_path}")

    # Confirm with user before proceeding
    response = input("Proceed with PATH update? (y/n): ").lower().strip()
    if response != 'y':
        print("PATH update aborted.")
        return

    # Use setx for user PATH (without /M flag)
    result = subprocess.run(f'setx PATH "{new_path}"', shell=True)

    if result.returncode != 0:
        print("Failed to update PATH. The new PATH might be too long or another error occurred.")
    else:
        print("User PATH updated. Please restart your terminal for the change to take effect.")

def install_vcpkg(installer_dir: pathlib.Path) -> None:
    """
    Clone the vcpkg repository and bootstrap it with modified environment.
    """
    vcpkg_dir = installer_dir / 'vcpkg'
    if not vcpkg_dir.exists():
        print("vcpkg not found. Cloning the vcpkg repository...")
        run_command("git clone https://github.com/microsoft/vcpkg.git",
                    cwd=str(installer_dir))
    else:
        print("vcpkg already exists. Skipping clone step.")

    # Use modified environment for bootstrap
    env = get_modified_env(installer_dir)
    current_platform = platform.system()

    if current_platform == 'Windows':
        print("Bootstrapping vcpkg using bootstrap-vcpkg.bat for Windows.")
        run_command("bootstrap-vcpkg.bat", cwd=str(vcpkg_dir), env=env)
    else:
        print("Bootstrapping vcpkg using bootstrap-vcpkg.sh for Unix/Linux/macOS.")
        run_command("./bootstrap-vcpkg.sh", cwd=str(vcpkg_dir), env=env)


def vcpkg_integrate(installer_dir: pathlib.Path) -> None:
    """
    Run vcpkg integration so Visual Studio and other tools know where to find libraries.
    """
    vcpkg_dir = installer_dir / "vcpkg"
    current_platform = platform.system()
    vcpkg_executable = str(vcpkg_dir / ("vcpkg.exe" if current_platform == "Windows" else "vcpkg"))
    print("Integrating vcpkg with Visual Studio...")
    run_command(f'"{vcpkg_executable}" integrate install', cwd=str(vcpkg_dir))


def get_triplet() -> str:
    """
    Determine the appropriate vcpkg triplet based on the current platform.
    """
    current_platform = platform.system()
    if current_platform == "Windows":
        triplet = "x64-windows"
    elif current_platform == "Linux":
        triplet = "x64-linux"
    elif current_platform == "Darwin":
        triplet = "x64-osx"
    else:
        print("Unknown platform. Exiting.")
        sys.exit(1)
    print(f"Using vcpkg triplet: {triplet}")
    return triplet


def load_dependencies(file_path: pathlib.Path) -> list:
    """
    Read the dependency list from a file.
    Each non-empty and non-comment line is a package name for vcpkg.
    """
    if not file_path.exists():
        print(f'Error: Dependency file "{file_path}" does not exist.')
        sys.exit(1)

    dependencies = []
    with file_path.open("r", encoding="utf-8") as file_handle:
        for line in file_handle:
            stripped_line = line.strip()
            if stripped_line and not stripped_line.startswith("#"):
                dependencies.append(stripped_line)
    return dependencies


def install_libraries(dependency_list: list, triplet: str, installer_dir: pathlib.Path) -> None:
    """
    Install libraries from the dependency list with vcpkg.
    """
    current_platform = platform.system()
    vcpkg_dir = installer_dir / "vcpkg"
    vcpkg_executable = str(vcpkg_dir / ("vcpkg.exe" if current_platform == "Windows" else "vcpkg"))
    for library in dependency_list:
        install_cmd = f'"{vcpkg_executable}" install {library}:{triplet}'
        print(f'Installing library "{library}" for triplet "{triplet}".')
        run_command(install_cmd)

def main() -> None:
    INSTALLER_DIR = pathlib.Path.home() / ".vcpkg_installer"
    if not INSTALLER_DIR.exists():
        print(f"Creating installer directory at {INSTALLER_DIR}")
        INSTALLER_DIR.mkdir(parents=True, exist_ok=True)

    # Create modified environment once
    modified_env = get_modified_env(INSTALLER_DIR)

    # On Windows, update the user PATH permanently
    if platform.system() == "Windows":
        update_path_permanent(INSTALLER_DIR)

    script_dir = pathlib.Path(__file__).parent
    dependency_file = script_dir / "dependencies.txt"

    print("Starting installation process for vcpkg and libraries...")
    install_vcpkg(INSTALLER_DIR)
    triplet = get_triplet()

    dependency_list = load_dependencies(dependency_file)
    if not dependency_list:
        print(f'No dependencies found in "{dependency_file}". Exiting.')
        sys.exit(0)

    # Use modified environment for all commands
    install_libraries(dependency_list, triplet, INSTALLER_DIR)
    vcpkg_integrate(INSTALLER_DIR)

    print("All tasks completed successfully.")


if __name__ == "__main__":
    main()