#!/usr/bin/env python
import os
from glob import glob
from pathlib import Path
import subprocess
import platform
import shutil
import sys

# TODO: Do not copy environment after godot-cpp/test is updated <https://github.com/godotengine/godot-cpp/blob/master/test/SConstruct>.
env = SConscript("godot-cpp/SConstruct")

# Add source files.
env.Append(CPPPATH=["src/", "nyan/nyan/", "nyan/build/nyan/"])
sources = Glob("src/*.cpp")

# Find gdextension path even if the directory or extension is renamed (e.g. project/addons/example/example.gdextension).
(extension_path,) = glob("project/addons/*/*.gdextension")

# Get the addon path (e.g. project/addons/example).
addon_path = Path(extension_path).parent

# Get the project name from the gdextension file (e.g. example).
project_name = Path(extension_path).stem

scons_cache_path = os.environ.get("SCONS_CACHE")
if scons_cache_path != None:
    CacheDir(scons_cache_path)
    print("Scons cache enabled... (path: '" + scons_cache_path + "')")

# Create the library target (e.g. libexample.linux.debug.x86_64.so).
debug_or_release = "release" if env["target"] == "template_release" else "debug"
if env["platform"] == "macos":
    library = env.SharedLibrary(
        "{0}/bin/lib{1}.{2}.{3}.framework/{1}.{2}.{3}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "{}/bin/lib{}.{}.{}.{}{}".format(
            addon_path,
            project_name,
            env["platform"],
            debug_or_release,
            env["arch"],
            env["SHLIBSUFFIX"],
        ),
        source=sources,
    )

# CMake build integration
def build_cmake(target, source, env):
    build_dir = os.path.abspath("nyan/build")
    install_dir = os.path.abspath("nyan/build/install")
    
    try:
        # Clean up previous directories to avoid conflicts
        print("Cleaning build and install directories...")
        if os.path.exists(build_dir):
            shutil.rmtree(build_dir)
        if os.path.exists(install_dir):
            shutil.rmtree(install_dir)
        
        print("Creating fresh build directory...")
        os.makedirs(build_dir)
        
        build_type = "Release" if env["target"] == "template_release" else "Debug"
        print(f"Configuring CMake with build type: {build_type}")
        
        # Configure CMake with install prefix to keep everything local
        subprocess.run([
            "cmake", 
            "-B", build_dir, 
            "-S", "nyan",
            f"-DCMAKE_BUILD_TYPE={build_type}",
            f"-DCMAKE_INSTALL_PREFIX={install_dir}",
            "-DREGISTER_USERPACKAGE=OFF"  # Disable global registration
        ], check=True)
        
        # Build using CMake
        print("Building with CMake...")
        subprocess.run([
            "cmake",
            "--build", build_dir,
            "--config", build_type
        ], check=True)
        
        # Install using CMake
        print("Installing with CMake...")
        subprocess.run([
            "cmake",
            "--install", build_dir,
            "--config", build_type
        ], check=True)
        
        print("CMake build completed successfully")
        return None
        
    except subprocess.CalledProcessError as e:
        print(f"Error during CMake build process: {e}", file=sys.stderr)
        print(f"Command that failed: {e.cmd}", file=sys.stderr)
        print(f"Output: {e.output if e.output else 'No output'}", file=sys.stderr)
        raise
    except Exception as e:
        print(f"Unexpected error during CMake build: {e}", file=sys.stderr)
        raise

cmake_target = env.Command('nyan_cmake_build', [], build_cmake)
Depends(cmake_target, library)  # Make CMake build after the main library

Default([library, cmake_target])  # Make both targets build by default
